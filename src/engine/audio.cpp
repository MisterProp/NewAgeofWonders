// audio.cpp - WAV / VAG decoding, WAV writing and WinMM waveOut playback.
//
// Part of the AoW1 C++ port.  See include/aow/audio.hpp and docs/05_AUDIO.md.
#include "aow/audio.hpp"

#include <algorithm>
#include <atomic>
#include <cstring>
#include <fstream>

#ifndef _WIN32
#error "aow::AudioOut currently targets the WinMM waveOut API (Windows)."
#endif

#include <windows.h>
#include <mmsystem.h>

namespace aow {
namespace {

std::uint32_t rd_u32(const std::uint8_t* p) {
    return static_cast<std::uint32_t>(p[0]) | (static_cast<std::uint32_t>(p[1]) << 8) |
           (static_cast<std::uint32_t>(p[2]) << 16) | (static_cast<std::uint32_t>(p[3]) << 24);
}
std::uint16_t rd_u16(const std::uint8_t* p) {
    return static_cast<std::uint16_t>(p[0] | (p[1] << 8));
}
std::uint32_t rd_be32(const std::uint8_t* p) {
    return (static_cast<std::uint32_t>(p[0]) << 24) | (static_cast<std::uint32_t>(p[1]) << 16) |
           (static_cast<std::uint32_t>(p[2]) << 8) | static_cast<std::uint32_t>(p[3]);
}

void set_err(std::string* e, const std::string& msg) {
    if (e) *e = msg;
}

std::int16_t clamp16(int v) {
    if (v < -32768) return -32768;
    if (v > 32767) return 32767;
    return static_cast<std::int16_t>(v);
}

void push_stereo(std::vector<std::int16_t>& out, std::int16_t l, std::int16_t r) {
    out.push_back(l);
    out.push_back(r);
}

}  // namespace

// --- RIFF/WAVE ------------------------------------------------------------

bool load_wav(const std::string& path, PcmBuffer& out, std::string* error) {
    std::ifstream f(path, std::ios::binary);
    if (!f) {
        set_err(error, "cannot open " + path);
        return false;
    }
    std::vector<std::uint8_t> d((std::istreambuf_iterator<char>(f)),
                                std::istreambuf_iterator<char>());
    if (d.size() < 44 || std::memcmp(d.data(), "RIFF", 4) != 0 ||
        std::memcmp(d.data() + 8, "WAVE", 4) != 0) {
        set_err(error, "not a RIFF/WAVE file: " + path);
        return false;
    }

    int channels = 0, bits = 0, format = 0;
    std::uint32_t rate = 0;
    const std::uint8_t* data = nullptr;
    std::size_t dataSize = 0;

    std::size_t i = 12;
    while (i + 8 <= d.size()) {
        const std::uint8_t* id = d.data() + i;
        std::uint32_t sz = rd_u32(d.data() + i + 4);
        std::size_t body = i + 8;
        if (std::memcmp(id, "fmt ", 4) == 0 && body + 16 <= d.size()) {
            format = rd_u16(d.data() + body);
            channels = rd_u16(d.data() + body + 2);
            rate = rd_u32(d.data() + body + 4);
            bits = rd_u16(d.data() + body + 14);
            if (format == 0xFFFE && sz >= 40) {  // WAVE_FORMAT_EXTENSIBLE
                format = rd_u16(d.data() + body + 24);
            }
        } else if (std::memcmp(id, "data", 4) == 0) {
            data = d.data() + body;
            dataSize = std::min<std::size_t>(sz, d.size() - body);
        }
        i = body + sz + (sz & 1);
    }

    if (format != 1 || channels < 1 || (bits != 8 && bits != 16 && bits != 24 && bits != 32) ||
        data == nullptr) {
        set_err(error, "unsupported WAV (need PCM 8/16/24/32-bit): " + path);
        return false;
    }

    out.sample_rate = static_cast<int>(rate ? rate : 22050);
    out.samples.clear();
    int bytes = bits / 8;
    std::size_t frameBytes = static_cast<std::size_t>(bytes) * channels;
    std::size_t frames = frameBytes ? dataSize / frameBytes : 0;
    out.samples.reserve(frames * 2);
    for (std::size_t n = 0; n < frames; ++n) {
        const std::uint8_t* p = data + n * frameBytes;
        std::int16_t ch[2] = {0, 0};
        for (int c = 0; c < channels && c < 2; ++c) {
            const std::uint8_t* q = p + static_cast<std::size_t>(c) * bytes;
            if (bits == 8) {
                ch[c] = static_cast<std::int16_t>((static_cast<int>(q[0]) - 128) * 256);
            } else if (bits == 16) {
                ch[c] = static_cast<std::int16_t>(rd_u16(q));
            } else if (bits == 24) {
                std::int32_t x = q[0] | (q[1] << 8) | (q[2] << 16);
                if (x & 0x800000) x |= ~0xFFFFFF;
                ch[c] = clamp16(x >> 8);
            } else {
                std::int32_t x = static_cast<std::int32_t>(rd_u32(q));
                ch[c] = clamp16(x >> 16);
            }
        }
        push_stereo(out.samples, ch[0], channels > 1 ? ch[1] : ch[0]);
    }
    return true;
}

// --- PlayStation VAG (4-bit ADPCM) ---------------------------------------

bool load_vag(const std::string& path, PcmBuffer& out, std::string* error) {
    std::ifstream f(path, std::ios::binary);
    if (!f) {
        set_err(error, "cannot open " + path);
        return false;
    }
    std::vector<std::uint8_t> d((std::istreambuf_iterator<char>(f)),
                                std::istreambuf_iterator<char>());
    if (d.size() < 48 || std::memcmp(d.data(), "VAGp", 4) != 0) {
        set_err(error, "not a VAG file: " + path);
        return false;
    }
    std::uint32_t dataSize = rd_be32(d.data() + 12);
    std::uint32_t rate = rd_be32(d.data() + 16);
    if (dataSize == 0 || dataSize > d.size() - 48)
        dataSize = static_cast<std::uint32_t>(d.size() - 48);

    static const int f0[5] = {0, 60, 115, 98, 122};
    static const int f1[5] = {0, 0, -52, -55, -60};

    out.sample_rate = static_cast<int>(rate ? rate : 44100);
    out.samples.clear();
    out.samples.reserve((dataSize / 16) * 28 * 2);

    const std::uint8_t* p = d.data() + 48;
    std::size_t blocks = dataSize / 16;
    int s1 = 0, s2 = 0;
    for (std::size_t b = 0; b < blocks; ++b, p += 16) {
        int shift = p[0] & 0x0F;
        int filter = (p[0] >> 4) & 0x0F;
        if (filter > 4) filter = 0;
        for (int n = 0; n < 28; ++n) {
            int nib = (p[2 + n / 2] >> ((n & 1) ? 4 : 0)) & 0x0F;
            int s = (nib << 12);
            if (s & 0x8000) s |= ~0xFFFF;  // sign-extend
            s >>= shift;
            s += (s1 * f0[filter] + s2 * f1[filter] + 32) / 64;
            s = clamp16(s);
            s2 = s1;
            s1 = s;
            push_stereo(out.samples, static_cast<std::int16_t>(s), static_cast<std::int16_t>(s));
        }
    }
    return true;
}

// --- WAV writer -----------------------------------------------------------

bool load_sample(const std::string& path, PcmBuffer& out, std::string* error) {
    std::ifstream f(path, std::ios::binary);
    if (!f) {
        set_err(error, "cannot open " + path);
        return false;
    }
    char magic[4] = {0, 0, 0, 0};
    f.read(magic, 4);
    f.close();
    if (std::memcmp(magic, "VAGp", 4) == 0) return load_vag(path, out, error);
    return load_wav(path, out, error);
}

bool write_wav(const std::string& path, const PcmBuffer& in, std::string* error) {
    std::ofstream f(path, std::ios::binary);
    if (!f) {
        set_err(error, "cannot write " + path);
        return false;
    }
    std::uint32_t dataBytes = static_cast<std::uint32_t>(in.samples.size() * 2);
    std::uint32_t rate = static_cast<std::uint32_t>(in.sample_rate);
    auto w16 = [&](std::uint16_t v) {
        f.put(static_cast<char>(v & 0xFF));
        f.put(static_cast<char>(v >> 8));
    };
    auto w32 = [&](std::uint32_t v) {
        f.put(static_cast<char>(v & 0xFF));
        f.put(static_cast<char>((v >> 8) & 0xFF));
        f.put(static_cast<char>((v >> 16) & 0xFF));
        f.put(static_cast<char>((v >> 24) & 0xFF));
    };
    f.write("RIFF", 4);
    w32(36 + dataBytes);
    f.write("WAVE", 4);
    f.write("fmt ", 4);
    w32(16);
    w16(1);
    w16(2);
    w32(rate);
    w32(rate * 4);
    w16(4);
    w16(16);
    f.write("data", 4);
    w32(dataBytes);
    for (std::int16_t s : in.samples) w16(static_cast<std::uint16_t>(s));
    return static_cast<bool>(f);
}

// --- WinMM waveOut --------------------------------------------------------

struct AudioOut::Impl {
    HWAVEOUT dev = nullptr;
    static const int kBuffers = 4;
    WAVEHDR hdr[kBuffers];
    std::vector<std::vector<std::int16_t>> mem;
    HANDLE ev = nullptr;
    int next = 0;
    std::atomic<int> inflight{0};
    bool used[4] = {};
    bool open = false;
    int rate = 44100, ch = 2, bits = 16;

    static void CALLBACK wave_cb(HWAVEOUT, UINT msg, DWORD_PTR inst, DWORD_PTR, DWORD_PTR) {
        if (msg == WOM_DONE) {
            auto* self = reinterpret_cast<Impl*>(inst);
            if (self) {
                --self->inflight;
                SetEvent(self->ev);
            }
        }
    }

    // Reuse only a buffer that has finished playing (WHDR_DONE) or that was
    // never queued.  Picking buffers round-robin instead could rewrite one that
    // was still playing, which is why streaming used to fall silent.
    void write_block(const std::int16_t* data, std::size_t frames) {
        int i = -1;
        for (;;) {
            for (int j = 0; j < kBuffers; ++j) {
                if (!used[j] || (hdr[j].dwFlags & WHDR_DONE)) { i = j; break; }
            }
            if (i >= 0) break;
            WaitForSingleObject(ev, 100);
        }
        std::size_t bytes = frames * 2 * sizeof(std::int16_t);
        std::size_t cap = mem[i].size() * sizeof(std::int16_t);
        if (bytes > cap) bytes = cap;
        std::memcpy(mem[i].data(), data, bytes);
        WAVEHDR& h = hdr[i];
        h.dwBufferLength = static_cast<DWORD>(bytes);
        h.dwFlags &= ~(WHDR_DONE | WHDR_INQUEUE);
        waveOutWrite(dev, &h, sizeof(h));
        used[i] = true;
        next = (i + 1) % kBuffers;
        ++inflight;
    }
};

AudioOut::AudioOut() : impl_(new Impl) {}
AudioOut::~AudioOut() { close(); }

bool AudioOut::open(int sample_rate, int channels, int bits, std::string* error) {
    close();
    Impl& s = *impl_;
    WAVEFORMATEX wf{};
    wf.wFormatTag = WAVE_FORMAT_PCM;
    wf.nChannels = static_cast<WORD>(channels);
    wf.nSamplesPerSec = static_cast<DWORD>(sample_rate);
    wf.wBitsPerSample = static_cast<WORD>(bits);
    wf.nBlockAlign = static_cast<WORD>(channels * bits / 8);
    wf.nAvgBytesPerSec = wf.nSamplesPerSec * wf.nBlockAlign;
    s.ev = CreateEventA(nullptr, FALSE, FALSE, nullptr);
    MMRESULT r = waveOutOpen(&s.dev, WAVE_MAPPER, &wf,
                             reinterpret_cast<DWORD_PTR>(&Impl::wave_cb),
                             reinterpret_cast<DWORD_PTR>(&s), CALLBACK_FUNCTION);
    if (r != MMSYSERR_NOERROR) {
        if (s.ev) { CloseHandle(s.ev); s.ev = nullptr; }
        s.dev = nullptr;
        set_err(error, "waveOutOpen failed (no audio device?)");
        return false;
    }
    s.mem.assign(Impl::kBuffers, std::vector<std::int16_t>(8192 * 2, 0));
    for (int i = 0; i < Impl::kBuffers; ++i) {
        std::memset(&s.hdr[i], 0, sizeof(WAVEHDR));
        s.hdr[i].lpData = reinterpret_cast<LPSTR>(s.mem[i].data());
        s.hdr[i].dwBufferLength = static_cast<DWORD>(s.mem[i].size() * sizeof(std::int16_t));
        s.used[i] = false;
        waveOutPrepareHeader(s.dev, &s.hdr[i], sizeof(WAVEHDR));   // prepared once, reused
    }
    s.next = 0;
    s.inflight = 0;
    s.rate = sample_rate;
    s.ch = channels;
    s.bits = bits;
    s.open = true;
    return true;
}

void AudioOut::close() {
    Impl& s = *impl_;
    if (s.dev) {
        waveOutReset(s.dev);
        while (s.inflight > 0) WaitForSingleObject(s.ev, 100);
        for (int i = 0; i < Impl::kBuffers; ++i)
            waveOutUnprepareHeader(s.dev, &s.hdr[i], sizeof(WAVEHDR));
        waveOutClose(s.dev);
        s.dev = nullptr;
    }
    if (s.ev) { CloseHandle(s.ev); s.ev = nullptr; }
    for (int i = 0; i < Impl::kBuffers; ++i) s.used[i] = false;
    s.open = false;
    s.inflight = 0;
}

bool AudioOut::is_open() const { return impl_->open; }

void AudioOut::play_raw(const std::int16_t* data, std::size_t frames) {
    Impl& s = *impl_;
    if (!s.open) return;
    const std::size_t chunk = 4096;
    while (frames > 0) {
        std::size_t n = std::min(frames, chunk);
        s.write_block(data, n);
        data += n * 2;
        frames -= n;
    }
}

void AudioOut::play(const PcmBuffer& buf) {
    play_raw(buf.samples.data(), buf.frames());
    Impl& s = *impl_;
    if (s.open) {
        while (s.inflight > 0) WaitForSingleObject(s.ev, 100);
    }
}

void AudioOut::stop() {
    Impl& s = *impl_;
    if (s.dev) {
        waveOutReset(s.dev);
        while (s.inflight > 0) WaitForSingleObject(s.ev, 100);
    }
}

}  // namespace aow

