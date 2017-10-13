AS=nasm
BUILDIR=../build/
all: mbr.bin

mbr.bin: mbr.asm printstring.asm readdrive.asm
	$(AS) mbr.asm -f bin -o $(BUILDIR)mbr.bin