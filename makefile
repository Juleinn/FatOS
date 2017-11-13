CC = i386-elf-gcc
SRC=$(wildcard kernel/*.c) $(wildcard kernel/*/*.c) $(wildcard kernel/*/*/*.c) $(wildcard kernel/*/*.as)
OBJ=$(SRC:%.c=%.o) $(SRC:%.as=%.o)
BUILDIR=build/
HEADER_DIRS=$(addprefix -I./, $(sort $(dir $(wildcard kernel/*/)))) $(addprefix -I./, $(sort $(dir $(wildcard kernel/*/*/))))

all: binary

run: binary
	qemu-system-i386 -fda fatos.flp

runonly:
	qemu-system-i386 -fda fatos.flp

debug: flp mbr.asm printstring.asm readdrive.asm
	# generate mbr and corresponding debug symbol file
	# must strip off the org directive at beginning of file
	rm mbrelf.asm
	dd if=mbr.asm of=mbrelf.asm bs=1 conv=notrunc skip=13
	nasm mbrelf.asm -f elf -F dwarf -g -o mbr.elf
	# start qemu with gdb debugging
	gdb -x gdbcmd

binary: kernel
	make -C MBR/
	make -C stage2/
	make -C build/

%.o: %.c
	@echo $<
	$(CC) -ffreestanding -nostdlib -c $< -o $(BUILDIR)$(notdir $@) $(HEADER_DIRS)

%.o: %.as
	i386-elf-as $< -o $(BUILDIR)$(notdir $@)

kernel: $(OBJ)

clean:
	-find -name '*.o' -delete
	-find -name '*.bin' -delete
