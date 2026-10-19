#!/usr/bin/env python3
"""check_mcp.py - smoke-test a running GhidraMCP server.

GhidraMCP (https://github.com/LaurieWired/GhidraMCP) has two moving parts:

  1. a Ghidra plugin that runs an embedded HTTP server (default
     http://127.0.0.1:8080) exposing program data as plain-text lines, and
  2. an optional MCP bridge (bridge_mcp_ghidra.py) that forwards MCP tool
     calls to that HTTP server and can also expose an SSE endpoint
     (default http://127.0.0.1:8081/sse).

This script checks whether either/both are up and answering. It only performs
read-only operations (GETs plus an optional /decompile POST) and never calls a
mutating endpoint such as /renameFunction. Standard library only, matching the
rest of port/tools.

Usage:
    python check_mcp.py                       # probe defaults (8080 + 8081/sse)
    python check_mcp.py --ghidra-url http://127.0.0.1:8080
    python check_mcp.py --skip-sse --decompile
    python check_mcp.py --json

Exit code is 0 when the Ghidra HTTP API answers every required endpoint,
otherwise 1, so it can gate a CI step or an agent workflow.
"""
import argparse
import json
import socket
import urllib.error
import urllib.parse
import urllib.request

DEFAULT_GHIDRA_URL = "http://127.0.0.1:8080"
DEFAULT_SSE_URL = "http://127.0.0.1:8081/sse"

# Read-only GET endpoints the bridge relies on. The 'required' ones must answer
# HTTP 200 for the server to count as healthy; the rest are informational.
GET_ENDPOINTS = [
    ("methods",       {"offset": 0, "limit": 5}, True),
    ("classes",       {"offset": 0, "limit": 5}, True),
    ("segments",      {"offset": 0, "limit": 5}, True),
    ("imports",       {"offset": 0, "limit": 5}, False),
    ("exports",       {"offset": 0, "limit": 5}, False),
    ("namespaces",    {"offset": 0, "limit": 5}, False),
    ("data",          {"offset": 0, "limit": 5}, False),
    ("strings",       {"offset": 0, "limit": 5}, False),
    ("functionCount", {},                        False),
]


def normalize_url(url):
    """Add a scheme if missing and drop a trailing slash on the path."""
    if "://" not in url:
        url = "http://" + url
    parts = urllib.parse.urlsplit(url)
    return urllib.parse.urlunsplit(
        (parts.scheme, parts.netloc, parts.path.rstrip("/"), "", ""))


def host_port(url):
    parts = urllib.parse.urlsplit(normalize_url(url))
    host = parts.hostname or "127.0.0.1"
    port = parts.port or (443 if parts.scheme == "https" else 80)
    return host, port


def tcp_reachable(host, port, timeout):
    """True if a TCP connection to host:port succeeds (the port is open)."""
    try:
        with socket.create_connection((host, port), timeout=timeout):
            return True, ""
    except OSError as exc:
        return False, str(exc)


def _join(base, endpoint):
    return base.rstrip("/") + "/" + endpoint.lstrip("/")


def _do(req, timeout):
    """Return (status, body, error). status 0 means the request never landed."""
    try:
        with urllib.request.urlopen(req, timeout=timeout) as resp:
            return resp.status, resp.read().decode("utf-8", "replace"), ""
    except urllib.error.HTTPError as exc:
        body = ""
        try:
            body = exc.read().decode("utf-8", "replace")
        except Exception:
            pass
        return exc.code, body, ""
    except urllib.error.URLError as exc:
        return 0, "", str(getattr(exc, "reason", exc))
    except socket.timeout:
        return 0, "", "timed out"


def http_get(base, endpoint, params, timeout):
    url = _join(base, endpoint)
    if params:
        url += "?" + urllib.parse.urlencode(params)
    req = urllib.request.Request(url, headers={"Accept": "text/plain"})
    return _do(req, timeout)


def http_post(base, endpoint, data, timeout):
    url = _join(base, endpoint)
    if isinstance(data, dict):
        body = urllib.parse.urlencode(data).encode("utf-8")
    else:
        body = data.encode("utf-8")
    req = urllib.request.Request(url, data=body, method="POST")
    return _do(req, timeout)


def probe_endpoints(base, timeout, limit):
    """GET each read-only endpoint and classify the result."""
    results = []
    for name, params, required in GET_ENDPOINTS:
        params = dict(params)
        if "limit" in params:
            params["limit"] = limit
        status, body, err = http_get(base, name, params, timeout)
        lines = [ln for ln in body.splitlines() if ln.strip()]
        if status == 200:
            state = "OK" if lines else "EMPTY"
        elif status == 0:
            state = "DOWN"
        else:
            state = "HTTP %d" % status
        results.append({
            "endpoint": name,
            "required": required,
            "state": state,
            "status": status,
            "lines": len(lines),
            "sample": lines[0][:100] if lines else "",
            "error": err,
        })
    return results


def probe_decompile(base, timeout, method_line):
    """POST a function name to /decompile (read-only) and check for output."""
    name = (method_line or "").split(";")[0].strip()
    if not name:
        return {"state": "SKIP", "detail": "no method name available"}
    status, body, err = http_post(base, "decompile", name, timeout)
    if status == 200 and body.strip():
        return {"state": "OK",
                "detail": "decompiled %r (%d chars)" % (name, len(body))}
    if status == 0:
        return {"state": "DOWN", "detail": err}
    return {"state": "HTTP %d" % status, "detail": (body.strip()[:100] or err)}


def probe_sse(url, timeout):
    """Check the MCP bridge SSE endpoint (connect + headers, no full read)."""
    host, port = host_port(url)
    ok, err = tcp_reachable(host, port, timeout)
    if not ok:
        return {"state": "DOWN", "detail": err}
    req = urllib.request.Request(url, headers={"Accept": "text/event-stream"})
    try:
        with urllib.request.urlopen(req, timeout=timeout) as resp:
            ctype = resp.headers.get("Content-Type", "")
            return {"state": "OK",
                    "detail": "HTTP %d content-type=%s" % (resp.status,
                                                           ctype or "?")}
    except urllib.error.HTTPError as exc:
        return {"state": "HTTP %d" % exc.code, "detail": exc.reason}
    except urllib.error.URLError as exc:
        return {"state": "DOWN", "detail": str(getattr(exc, "reason", exc))}
    except socket.timeout:
        return {"state": "OK", "detail": "connected, stream open (no bytes yet)"}


def build_parser():
    p = argparse.ArgumentParser(description="Smoke-test a running GhidraMCP server.")
    p.add_argument("--ghidra-url", default=DEFAULT_GHIDRA_URL,
                   help="Ghidra plugin HTTP base URL (default %(default)s)")
    p.add_argument("--sse-url", default=DEFAULT_SSE_URL,
                   help="MCP bridge SSE URL (default %(default)s)")
    p.add_argument("--skip-sse", action="store_true",
                   help="do not probe the MCP bridge SSE endpoint")
    p.add_argument("--decompile", action="store_true",
                   help="also POST a function name to /decompile")
    p.add_argument("--limit", type=int, default=5,
                   help="pagination limit sent to listing endpoints (default 5)")
    p.add_argument("--timeout", type=float, default=5.0,
                   help="per-request timeout in seconds (default 5)")
    p.add_argument("--json", action="store_true", help="print a JSON report")
    return p


def print_hint(url, reason):
    print()
    print("  GhidraMCP is not answering at %s (%s)." % (url, reason))
    print("  Checklist:")
    print("    - Ghidra is running with a program open in the CodeBrowser.")
    print("    - GhidraMCPPlugin is enabled: File > Configure > Developer.")
    print("    - The port matches: Edit > Tool Options > GhidraMCP HTTP Server"
          " (default 8080).")
    print("    - For the MCP bridge (SSE):"
          " python bridge_mcp_ghidra.py --transport sse")


def emit(report, as_json):
    if as_json:
        print(json.dumps(report, indent=2))
        return
    print("GhidraMCP smoke test")
    print("  target   :", report["ghidra_url"])
    if not report["reachable"]:
        print("  status   : UNREACHABLE")
        print("  reason   :", report.get("error", ""))
        print_hint(report["ghidra_url"], report.get("error", ""))
        return
    print("  status   :",
          "HEALTHY" if report["healthy"] else "REACHABLE (but unhealthy)")
    print("  %-15s %-8s %-8s %-5s %s" %
          ("endpoint", "req?", "state", "lines", "sample"))
    for e in report["endpoints"]:
        print("  %-15s %-8s %-8s %-5d %s" %
              (e["endpoint"], "yes" if e["required"] else "no",
               e["state"], e["lines"], e["sample"]))
    if report.get("decompile"):
        d = report["decompile"]
        print("  decompile       %-8s %s" % (d["state"], d["detail"]))
    if report.get("sse"):
        s = report["sse"]
        print("  sse             %-8s %s  (%s)" %
              (s["state"], s["url"], s["detail"]))


def run(args):
    base = normalize_url(args.ghidra_url)
    report = {"ghidra_url": base, "reachable": False, "healthy": False,
              "endpoints": [], "decompile": None, "sse": None}

    host, port = host_port(base)
    reachable, err = tcp_reachable(host, port, args.timeout)
    report["reachable"] = reachable
    if not reachable:
        report["error"] = err
        emit(report, args.json)
        return 1

    report["endpoints"] = probe_endpoints(base, args.timeout, args.limit)
    report["healthy"] = all(
        e["status"] == 200 for e in report["endpoints"] if e["required"])

    if args.decompile:
        first = next((e["sample"] for e in report["endpoints"]
                      if e["endpoint"] == "methods" and e["sample"]), "")
        report["decompile"] = probe_decompile(base, args.timeout, first)

    if not args.skip_sse:
        sse_url = normalize_url(args.sse_url)
        report["sse"] = {"url": sse_url}
        report["sse"].update(probe_sse(sse_url, args.timeout))

    emit(report, args.json)
    return 0 if report["healthy"] else 1


def main(argv=None):
    return run(build_parser().parse_args(argv))


if __name__ == "__main__":
    raise SystemExit(main())

