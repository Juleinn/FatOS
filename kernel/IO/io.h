/* file io.h 
 * Anton CLAES
 * Contains port reading functions */

/* outputs byte b on port p */
void outb(unsigned char b, short p);

/* reads byte on port p */
unsigned char inb(short port);