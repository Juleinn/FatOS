#include "string.h"

int strcmp(char* str1, char* str2)
{
	while(*str1 || *str2)
	{
		if(*str1 != *str2)
			return 1;
		str1++;
		str2++;
	}
	return 0;
}

int atoi(char* buffer)
{
	int mult = 1;	// handle neg. numbers
	int result = 0;
	if(*buffer == '-')
	{
		mult = -1;
		buffer++;
	}

	while(*buffer)
	{
		result *= 10;
		result += (*buffer - '0');
		buffer++;
	}
	return result * mult;
}

int strlen(char * str)
{
	int len = 0;
	while(*str)
	{
		len++;
		str++;
	}
	return len;
}
