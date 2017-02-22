#include <stdio.h>
#include <stdlib.h>
#include "params.h"

char getNumFromBits(unsigned char num, unsigned char offset, unsigned char bitcount);
char* bytestr(unsigned char byte);

int main() {
	/* For this version, all addresses should be 6 bits at most, and all offsets should be 4 bits. The page table shall
	 * be the first 4 bytes of the first page, which itself is marked as out of bounds. A page table entry shall follow
	 * the following format:
	 *
	 * ------------------------------------------------------------------
	 * |	7	|	6	|	5	|	4	|	3	|	2	|	1	|	0	|
	 * ------------------------------------------------------------------
	 * |  Open? |   Owner PID	| Read	| Write	|		Reserved		|
	 */

	char memory[MEMSIZE];

	//Initialize the page table
	for (int i = 0; i < 4; i++)
		memory[i] = B_OPEN | B_READ;

	//Set the first page (containing the page table) as off-limits
	memory[0] &= ~B_OPEN;

	//Now we read from stdin

}


char* bytestr(unsigned char byte){
	char* str = malloc(8); //hey look, a memory leak!
	for (char i = 0; i < 8; i++)
		str[7-i] = BIT(i) & byte ? '1' : '0';
	return str;
}