#include "console.h"

void console()
{
	#define CSL_CMD_BUF_LGHT 50 		// length of the buffer
	char cmd_buf[CSL_CMD_BUF_LGHT]={0};

	prints("Anton's console. type \"help\" for a list of commands\n");

	do
	{
		// wipe buffer to avoid old buffer bits 
		memset(0, cmd_buf, CSL_CMD_BUF_LGHT);

		prints("#>");
		int cmdlen = getline(cmd_buf, CSL_CMD_BUF_LGHT);
		int cmdid = console_command_id(cmd_buf);
		printc('\n');
		console_launch(cmdid, (char*) (cmd_buf + strlen(command_list[cmdid])+1));

	}while(strcmp(cmd_buf, "exit") != 0);
	prints("exiting console\n");
}

int console_command_id(char* cmd)
{
	int i;
	for(i=0; i<CONSOLE_COMMAND_COUNT; i++)
	{
		if(cmdCmp(command_list[i], cmd) == 0)
			return i;
	}
	return -1;
}

void console_launch(int id, char * args)
{
	switch(id)
	{
		case 0:
			// exit : do nothing
			break;
		case 1:
			/*static pos printing : clear screen */
			clear_screen();
			print_memorymap(0x0F);
			break;
		case 2:
			clear_screen();
			set_cursor_pos(0,0);
			break;
		case 3:
			help();
			break;
		case 4:
			console();
			break;
		case 5:
			print_floppy_drives();
			break;
		case 6:
			floppy_read();
			break;
		case 7:
			// print_FFS_sector_table();
			break;
		case 8:
			memorymanager_print();
			break;
		case 9:
			filesystems_driveList(args);
			break;
		default:
			prints("Unknown command. Type \"help\" for a list of commands\n");
	}
}

void help()
{
	prints("List of commands : \n");
	int i;
	for(i=0; i<CONSOLE_COMMAND_COUNT; i++)
	{
		prints(command_list[i]);
		prints("\n");
	}
}

int cmdCmp(const char *str1, const char * str2)
{
	while(*str1 && *str2 && *str1 != ' ' && *str2 != ' '){
		if(*str1 != *str2)
			return 1;
		str1++;
		str2++;
	}
	return 0;
}