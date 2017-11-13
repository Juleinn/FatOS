/* File string.h
 * Anton CLAES
 * Contains string handling functions, such as strcmp */

#ifndef STRING_H_
#define STRING_H_

/* compares 2 strings, returns 0 upon equality */
int strcmp(char* str1, char* str2);

/* Turns a string representing an integer to an integer */
int atoi(char* buffer);

/* Measures the length of given string before \0 (without \0)*/
int strlen(char * str);

/* Returns the index of the first occurence of char c in string str 
strIndexOf("abc", 'b') = 1
*/
int strIndexOf(char* str, char c);

/* Counts the occurences of a given char */
int chrCount(const char* str, const char c);

/* Performs a deep copy of s1 into s2 */
void strcpy(const char * str1, char * str2);

#endif
