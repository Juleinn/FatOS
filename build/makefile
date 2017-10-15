all: genclr flp 
	mv fatos.flp ../

OBJ=kernel.o $(filter-out kernel.o,$(wildcard *.o))
# OBJ = kernel.o interruption.o idt.o    isrs.o  kernel.o  memory.o  print.o



flp: mbr.bin stage2.bin kernel.bin
	# MBR
	dd if=mbr.bin of=fatos.flp conv=notrunc seek=0 bs=512
	# FAT & file list file (generated)
	dd if=FATandList.bin of=fatos.flp seek=1 bs=512
	# stage 2
	dd if=stage2.bin of=fatos.flp conv=notrunc bs=512 seek=12
	# copy kernel
	dd if=kernel.bin of=fatos.flp conv=notrunc bs=512 seek=13

# generates a file of 10 empty sectors of 512 bytes
genclr:
	dd if=/dev/zero of=fatos.flp bs=512 count=2880

kernel.bin: $(OBJ)
	i386-elf-ld -o kernel.bin -Ttext 0x100000 -e main -nostdlib $^ --oformat binary

kernel.elf: $(OBJ)
	i386-elf-ld -o kernel.elf -Ttext 0x100000 -e main -nostdlib $^
