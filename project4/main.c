#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "params.h"

char getNumFromBits(unsigned char num, unsigned char offset, unsigned char bitcount);
char* bytestr(unsigned char byte);
unsigned char translateAddress(char* memory, unsigned char pid, unsigned char addr);

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
	printf("Finished init\n");

	//Now start reading from stdin... over and over and over again!
	while (1){
		int size = 256;
		char* input = (char*)malloc(256);
		if (getline(&input, &size, stdin) == -1){
			printf("EOF reached, exiting!\n");
			break;
		}

		//Parse the string into four values: pid, instruction, addr, val. All of these should be numbers
		input[strlen(input) - 1] = '\0'; //chop off the newline
		const unsigned char pid = (unsigned char)atoi(strtok(input, ","));
		const unsigned char ist = (unsigned char)atoi(strtok(NULL, ","));
		const unsigned char adr = (unsigned char)atoi(strtok(NULL, ","));
		const unsigned char val = (unsigned char)atoi(strtok(NULL, ","));

		if (ist == MAP){
			printf("Trying to allocate a new page to process %d\n", pid);
			//Try to allocate a page
			int success = 0;
			for (int i = 0; i < 4; i++){
				printf("Current memory: %s\n", bytestr(memory[i]));
				if (memory[i] & B_OPEN){
					printf("Allocating page %d to PID %s\n", i, bytestr(pid));
					memory[i] |= pid << 5;
					memory[i] &= ~B_OPEN;
					if (val == 1)
						memory[i] |= B_WRTE;
					printf("New memory: %s\n", bytestr(memory[i]));
					success = 1;
					break;
				}
			}

			if (!success)
				printf("Couldn't map new page!\n");
		}
		if (ist == STR) {
			printf("Instruction: STORE\n");
			unsigned char newAddr = translateAddress(memory, pid, adr);
			if (newAddr == 0)
				continue;
			printf("Translated address %d to address %d\n", adr, newAddr);
			memory[newAddr] = val;
		}
		if (ist == LDM) {
			printf("Instruction: LOAD\n");
			unsigned char newAddr = translateAddress(memory, pid, adr);
			if (newAddr == 0)
				continue;
			printf("Translated address %d to address %d\n", adr, newAddr);
			printf("Memory at %d: %d\n", newAddr, memory[newAddr]);
		}
	}

	return 0;
}


unsigned char translateAddress(char* memory, unsigned char pid, unsigned char addr) {
	printf("Virtual address: %s\n", bytestr(addr));
	const unsigned char vpn = MID(addr, 4, 7);
	printf("Translating VPN %d\n", vpn);

	unsigned char pfn = 0;
	int count = -1;
	for (int i = 0; i < 4; i++) {
		printf("Reading page entry %s\n", bytestr(memory[i]));
		printf("Extracted pid %d from table\n", MID(memory[i], 5, 7));
		if (MID(memory[i], 5, 7) == pid) {
			count++;
			if (count == vpn) {
				pfn = i;
				break;
			}
		}
	}

	if (pfn != 0)
		printf("Translated vpn %d to pfn %d\n", vpn, pfn);
	else {
		printf("Failed to translate vpn to pfn!\n");
		return 0;
	}
	unsigned char trnsAddr = MID(addr, 0, 4);
	printf("Masking out vpn and replacing with pfn: %s -> %s\n", bytestr(addr), bytestr(trnsAddr));
	printf("Applying translation: %s ->", bytestr(trnsAddr));
	trnsAddr |= pfn << 4;
	printf("%s\n", bytestr(trnsAddr));
	return trnsAddr;
}



char* bytestr(unsigned char byte) {
	char *str = malloc(8); //hey look, a memory leak!
	for (char i = 0; i < 8; i++)
		str[7 - i] = BIT(i) & byte ? '1' : '0';
	return str;
}