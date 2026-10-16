import ghidra.app.script.GhidraScript;
import ghidra.program.model.address.Address;
import ghidra.program.model.listing.*;
import ghidra.program.model.symbol.Reference;

import java.io.File;
import java.io.PrintWriter;
import java.util.*;

public class DeepAnalyzeAllFunctions extends GhidraScript {
    @Override
    public void run() throws Exception {
        File outFile = new File("/Users/hedge/workspace/comic2-rea/all_functions_detail.txt");
        PrintWriter writer = new PrintWriter(outFile);

        Listing listing = currentProgram.getListing();
        FunctionManager fm = currentProgram.getFunctionManager();
        FunctionIterator fIter = fm.getFunctions(true);

        while (fIter.hasNext()) {
            Function f = fIter.next();
            writer.println("================================================================================");
            writer.println("FUNCTION: " + f.getName() + " @ " + f.getEntryPoint() + " (Body size: " + f.getBody().getNumAddresses() + ")");
            writer.println("================================================================================");

            // Print all callers
            Set<Function> callers = f.getCallingFunctions(monitor);
            StringBuilder callerSb = new StringBuilder("CALLERS: ");
            for (Function c : callers) callerSb.append(c.getName()).append(" ");
            writer.println(callerSb.toString());

            // Print all instructions
            InstructionIterator iIter = listing.getInstructions(f.getBody(), true);
            while (iIter.hasNext()) {
                Instruction inst = iIter.next();
                StringBuilder refSb = new StringBuilder();
                for (Reference ref : inst.getReferencesFrom()) {
                    refSb.append(" -> ").append(ref.getToAddress());
                }
                writer.println(String.format("  %-12s %-30s %s", inst.getAddress(), inst, refSb.toString()));
            }
            writer.println();
        }

        writer.close();
        println("Exported full details to " + outFile.getAbsolutePath());
    }
}
