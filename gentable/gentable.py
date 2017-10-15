f = open("table", "wb");

cont = True

while cont:
    name = input("FileName :")
    size = int(input("Length in Bytes : "))
    firstSector = int(input("First sector of file in LBA mode : "))
    f.write(bytes(name, 'utf-8'));
    f.write((0x00).to_bytes(1, byteorder='little'))
    f.write(size.to_bytes(2, byteorder='little'))
    f.write(firstSector.to_bytes(2, byteorder='little'))
    if input("Continue(y/n)?") != "y":
        cont=False;
