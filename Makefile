# Makefile for Captain Comic II: Fractured Reality Disassembly
# Based on Captain Comic 1 build process

NASM = nasm
DJLINK = djlink
# Assuming djlink is in the path or in a specific directory.
# For now, we'll just define the commands.

ASFLAGS = -f obj
LDFLAGS = 

TARGET = comic2.exe
SRC = comic2.asm
OBJ = comic2.obj

all: $(TARGET)

$(OBJ): $(SRC)
	$(NASM) $(ASFLAGS) $(SRC) -o $(OBJ)

$(TARGET): $(OBJ)
	$(DJLINK) $(OBJ) $(TARGET)

clean:
	rm -f $(OBJ) $(TARGET)

.PHONY: all clean
