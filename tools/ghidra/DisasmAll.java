//DisasmAll.java - dump the instruction listing per function.
//
//   analyzeHeadless <projDir> <proj> -import Galaxy.dll \
//       -postScript DisasmAll.java -scriptPath tools/ghidra
//
// Writes re/exports/<module>/<module>.disasm.txt with
//   ==== <entry>  <name> ====          (function boundary marker)
//   <addr>  <mnemonic operands>
//
//@category AoW

import java.io.File;
import java.io.PrintWriter;

import ghidra.app.script.GhidraScript;
import ghidra.program.model.listing.Function;
import ghidra.program.model.listing.Instruction;
import ghidra.program.model.listing.InstructionIterator;
import ghidra.program.model.listing.Listing;

public class DisasmAll extends GhidraScript {

    @Override
    public void run() throws Exception {
        String outRoot = System.getenv("AOW_OUT");
        if (outRoot == null || outRoot.isEmpty()) {
            outRoot = new File(getScriptArgs().length > 0
                    ? getScriptArgs()[0] : ".").getAbsolutePath();
        }
        String module = currentProgram.getName().replaceAll("\\.[A-Za-z0-9]+$", "");
        File dir = new File(outRoot, module);
        dir.mkdirs();

        Listing lst = currentProgram.getListing();
        InstructionIterator it = lst.getInstructions(true);
        int n = 0;
        try (PrintWriter w = new PrintWriter(
                new File(dir, module + ".disasm.txt"), "UTF-8")) {
            while (it.hasNext() && !monitor.isCancelled()) {
                Instruction ins = it.next();
                Function f = getFunctionContaining(ins.getAddress());
                if (f != null && ins.getAddress().equals(f.getEntryPoint())) {
                    w.println();
                    w.println("==== " + f.getEntryPoint() + "  " + f.getName() + " ====");
                }
                w.println(ins.getAddress() + "  " + ins);
                ++n;
            }
        }
        println("DisasmAll: " + module + " " + n + " instructions -> "
                + dir.getAbsolutePath());
    }
}
