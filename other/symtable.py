file = open("symtable.sym", "r")

while True:
	line = file.readline()
	if line =="":
		break;
	if line[0] == '0' and line[17:22] == ".text": # actually interesting
		print ((hex(int("0x" + line[:8], 16) + 31744)) + line[30:])[2:],
		# print line[:8] + line[30:],
