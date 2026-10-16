import ghidra.app.script.GhidraScript;
import ghidra.program.model.address.Address;
import ghidra.program.model.listing.Function;
import ghidra.program.model.listing.Instruction;
import ghidra.program.model.listing.Listing;

public class CheckCodeCoverage extends GhidraScript {
    @Override
    public void run() throws Exception {
        Listing listing = currentProgram.getListing();
        Address cur = toAddr("1000:7e40");
        Address end = toAddr("1000:d5cf");

        int codeCount = 0;
        int dataCount = 0;
        int undefinedCount = 0;

        while (cur.compareTo(end) < 0) {
            Instruction inst = listing.getInstructionAt(cur);
            if (inst != null) {
                codeCount++;
                cur = cur.add(inst.getLength());
            } else {
                Function f = getFunctionAt(cur);
                if (f != null) {
                    println("Found function at " + cur + ": " + f.getName());
                }
                cur = cur.add(1);
                undefinedCount++;
            }
        }
        println(String.format("Between 1000:7e40 and 1000:d5cf: Instructions found: %d, Non-instruction bytes: %d", codeCount, undefinedCount));

        // Also check if any instructions exist after 7e40
        Instruction nextInst = listing.getInstructionAfter(toAddr("1000:7e40"));
        if (nextInst != null) {
            println("Next instruction after 7e40 is at: " + nextInst.getAddress() + " : " + nextInst);
        } else {
            println("No instructions defined after 7e40!");
        }

        // Let's preview bytes at 1000:7e50
        byte[] buf = new byte[64];
        currentProgram.getMemory().getBytes(toAddr("1000:7e50"), buf);
        StringBuilder sb = new StringBuilder();
        StringBuilder ascii = new StringBuilder();
        for (byte b : buf) {
            sb.append(String.format("%02x ", b));
            char c = (char) (b & 0xff);
            ascii.append(c >= 32 && c <= 126 ? c : '.');
        }
        println("Bytes at 1000:7e50:\n" + sb + "\n" + ascii);
    }
}
