import ghidra.app.script.GhidraScript;
import ghidra.program.model.address.Address;
import ghidra.program.model.listing.*;
import ghidra.program.model.mem.Memory;
import ghidra.program.model.mem.MemoryBlock;

import java.io.File;
import java.io.PrintWriter;

public class ExportGhidraListing extends GhidraScript {
    @Override
    public void run() throws Exception {
        File outFile = new File("/Users/hedge/workspace/comic2-rea/ghidra_code0_listing.txt");
        PrintWriter writer = new PrintWriter(outFile);

        Listing listing = currentProgram.getListing();
        Memory mem = currentProgram.getMemory();
        Address start = toAddr("1000:0000");
        Address end = toAddr("1000:d5cf");

        Address cur = start;
        while (cur.compareTo(end) <= 0) {
            Instruction inst = listing.getInstructionAt(cur);
            if (inst != null) {
                int len = inst.getLength();
                byte[] bytes = new byte[len];
                mem.getBytes(cur, bytes);
                StringBuilder hex = new StringBuilder();
                for (byte b : bytes) hex.append(String.format("%02x", b));

                writer.println(String.format("INST\t%04x\t%s\t%s\t%s",
                        cur.getOffset() & 0xffff,
                        hex.toString(),
                        inst.getMnemonicString(),
                        inst.getDefaultOperandRepresentation(0) != null ? inst.toString().substring(inst.getMnemonicString().length()).trim() : ""));
                cur = cur.add(len);
            } else {
                Data data = listing.getDataAt(cur);
                if (data != null && data.isDefined()) {
                    int len = data.getLength();
                    byte[] bytes = new byte[len];
                    mem.getBytes(cur, bytes);
                    StringBuilder hex = new StringBuilder();
                    for (byte b : bytes) hex.append(String.format("%02x", b));
                    writer.println(String.format("DATA\t%04x\t%s\t%s",
                            cur.getOffset() & 0xffff,
                            hex.toString(),
                            data.getDataType().getName()));
                    cur = cur.add(len);
                } else {
                    byte b = mem.getByte(cur);
                    writer.println(String.format("BYTE\t%04x\t%02x\tundefined",
                            cur.getOffset() & 0xffff,
                            b & 0xff));
                    cur = cur.add(1);
                }
            }
        }

        writer.close();
        println("Exported full CODE_0 listing to " + outFile.getAbsolutePath());
    }
}
