import ghidra.app.script.GhidraScript;
import ghidra.program.model.address.Address;
import ghidra.program.model.listing.Instruction;
import ghidra.program.model.listing.InstructionIterator;

public class CheckGapInstructions extends GhidraScript {
    @Override
    public void run() throws Exception {
        int[][] gaps = {
            {0x0308, 0x0599},
            {0x0a5e, 0x18e0},
            {0x5090, 0x5ceb},
            {0x5e52, 0x61c8},
            {0x633c, 0x6545}
        };

        for (int[] g : gaps) {
            Address start = toAddr(String.format("1000:%04x", g[0]));
            Address end = toAddr(String.format("1000:%04x", g[1]));
            InstructionIterator iter = currentProgram.getListing().getInstructions(currentProgram.getAddressFactory().getAddressSet(start, end), true);
            int count = 0;
            while (iter.hasNext()) {
                iter.next();
                count++;
            }
            println(String.format("Gap 1000:%04x - 1000:%04x : Instructions currently defined: %d", g[0], g[1], count));
        }
    }
}
