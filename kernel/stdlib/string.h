/* File string.h 
 * Anton CLAES
 * Contains string handling functions, such as strcmp */

#ifndef STRING_H_
#define STRING_H_

/* compares 2 strings, returns 0 upon equality */
int strcmp(char* str1, char* str2);

/* Turns a string representing an integer to an integer */
int atoi(char* buffer);

#endif