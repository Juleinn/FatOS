all:run

run:
	dd if=/dev/zero of=FATandList.bin bs=512 count=11
	gcc gentable.c -o gentable
	./gentable
