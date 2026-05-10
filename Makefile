# Makefile for Captain Comic II: Fractured Reality Disassembly
# Based on Captain Comic 1 build process

NASM = nasm
DJLINK = tools/djlink/djlink

ASFLAGS = -f obj
LDFLAGS = 

TARGET = comic2.exe
SRC = comic2.asm
OBJ = comic2.obj

all: $(TARGET)

$(DJLINK):
	$(MAKE) -C tools/djlink djlink

$(OBJ): $(SRC)
	$(NASM) $(ASFLAGS) $(SRC) -o $(OBJ)

$(TARGET): $(OBJ) $(DJLINK)
	$(DJLINK) $(OBJ) -o $(TARGET)

clean:
	rm -f $(OBJ) $(TARGET)
	$(MAKE) -C tools/djlink clean

.PHONY: all clean
