import ghidra.app.cmd.disassemble.DisassembleCommand;
import ghidra.app.script.GhidraScript;
import ghidra.program.model.address.Address;
import ghidra.program.model.address.AddressSet;
import ghidra.program.model.listing.Instruction;
import ghidra.program.model.listing.InstructionIterator;

public class DisassembleHiddenCode extends GhidraScript {
    @Override
    public void run() throws Exception {
        String[] targets = {
            "1000:05c2", // INT 09h Keyboard ISR
            "1000:067c", // INT 23h Break handler
            "1000:0683", // INT 08h Timer/Sound ISR
            "1000:08c7", // INT 03h Custom dispatcher
            "1000:093a"  // INT 15h
        };

        for (String t : targets) {
            Address addr = toAddr(t);
            println("Disassembling at " + addr + "...");
            DisassembleCommand cmd = new DisassembleCommand(addr, null, true);
            cmd.applyTo(currentProgram, monitor);

            createFunction(addr, "ISR_" + t.replace(":", "_"));

            InstructionIterator iter = currentProgram.getListing().getInstructions(new AddressSet(addr, addr.add(64)), true);
            int count = 0;
            while (iter.hasNext() && count < 20) {
                Instruction inst = iter.next();
                println(String.format("  %s:  %-24s", inst.getAddress(), inst));
                count++;
            }
        }
    }
}
