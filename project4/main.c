#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <zconf.h>
#include "params.h"

char* bytestr(unsigned char byte);
unsigned char translateAddress(char* memory, unsigned char pid, unsigned char addr, FILE* swapfile, char* swap);
int writeToSwap(unsigned char* memory, unsigned char pfn, unsigned char* swap, FILE* swapfile);
unsigned char evictPage(unsigned char* memory, unsigned char* swap, FILE* swapfile);
unsigned char getNotPresent(unsigned char* memory, unsigned char* swap, FILE* swapfile, int vpn, int pid);
unsigned char getNumPages(unsigned char* memory, int pid);

int main() {
	/* For this version, all addresses should be 6 bits at most, and all offsets should be 4 bits. The page table shall
	 * be the first 4 bytes of the first page, which itself is marked as out of bounds. A page table entry shall follow
	 * the following format:
	 *
	 * ------------------------------------------------------------------
	 * |	7	|	6	|	5	|	4	|	3	|	2	|	1	|	0	|
	 * ------------------------------------------------------------------
	 * |  Open? |   Owner PID	|Present| Write	|  		   VPN			|
	 */

	unsigned char memory[MEMSIZE] = { 0 };
	FILE* swapfile = fopen("swap.bin", "wb+");
	unsigned char* swap = (unsigned char*)malloc(SWAPSIZE+1);
	for (int i = 0; i < SWAPSIZE; i++)
		swap[i] = 0;
	//fread(swap, 1, SWAPSIZE, swapfile);

	//Initialize the page table
	for (int i = 0; i < 4; i++)
		memory[i] = B_OPEN;

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
			for (int i = 0; i < PAGECOUNT; i++){
				printf("Current memory: %s\n", bytestr(memory[i]));
				if (memory[i] & B_OPEN){
					printf("Allocating page %d to PID %s\n", i, bytestr(pid));
					memory[i] |= getNumPages(memory, pid);
					memory[i] |= pid << 5;
					memory[i] |= B_PRESENT;
					memory[i] &= ~B_OPEN;
					if (val == 1)
						memory[i] |= B_WRTE;
					printf("New memory: %s\n", bytestr(memory[i]));
					success = 1;
					break;
				}
			}

			if (!success){
				printf("Failed to allocate a page in memory, swapping out another page to make room!\n");
				const int evicted = evictPage(memory, swap, swapfile);
				printf("Allocating page %d to PID %s\n", evicted , bytestr(pid));
				memory[evicted] |= getNumPages(memory, pid);
				memory[evicted] |= pid << 5;
				memory[evicted] &= ~B_OPEN;
				if (val == 1)
					memory[evicted] |= B_WRTE;
				printf("New memory: %s\n", bytestr(memory[evicted]));
			}
		}
		if (ist == STR) {
			printf("Instruction: STORE\n");
			unsigned char newAddr = translateAddress(memory, pid, adr, swapfile, swap);
			if (newAddr == 0)
				continue;
			printf("Translated address %d to address %d\n", adr, newAddr);
			memory[newAddr] = val;
		}
		if (ist == LDM) {
			printf("Instruction: LOAD\n");
			unsigned char newAddr = translateAddress(memory, pid, adr, swapfile, swap);
			if (newAddr == 0)
				continue;
			printf("Translated address %d to address %d\n", adr, newAddr);
			printf("Memory at %d: %d\n", newAddr, memory[newAddr]);
		}
	}

	fclose(swapfile);
	return 0;
}


unsigned char translateAddress(char* memory, unsigned char pid, unsigned char addr, FILE* swapfile, char* swap) {
	printf("Virtual address: %s\n", bytestr(addr));
	const unsigned char vpn = MID(addr, 4, 7);
	printf("Translating VPN %d\n", vpn);

	unsigned char pfn = 0;
	//Find the location of our memory in memory
	for (int i = 0; i < 16; i++)
	{
		printf("Current memory: %s\n", bytestr(memory[i]));
		printf("Reading%s present page table entry containing VPN:%d and PID:%d\n", memory[i] & B_PRESENT ? "" : " not", MID(memory[i], 0, 3), MID(memory[i], 5, 7));
		//If the VPN and the PID match...
		if (MID(memory[i], 0, 3) == vpn && MID(memory[i], 5, 7) == pid)
		{
			if (memory[i] & B_PRESENT)
				pfn = i;
			else
				pfn = getNotPresent(memory, swap, swapfile, vpn, pid);
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

unsigned char getNotPresent(unsigned char* memory, unsigned char* swap, FILE* swapfile, int vpn, int pid) {
	//Gets some memory that's in swap right now (indicated by a 0 on the present bit), returns its new PFN
	//First identify where in swap the memory is
	int success = 0;
	int swapFN = 0;
	for (int i = 4; i < 16; i++) {
		if (MID(memory[i], 0, 3) == vpn && MID(memory[i], 5, 7) == pid) {
			success = 1;
			swapFN = i;
			break;
		}
	}

	if (!success) {
		printf("Couldn't find vpn %d for pid %d!\n", vpn, pid);
		return -1;
	}
	success = 0;

	//Round-robin evict a page
	unsigned char lastEvicted = evictPage(memory, swap, swapfile);

	//Move the page we want into the newly freed slot
	memory[lastEvicted] = memory[swapFN];
	memory[lastEvicted] |= B_PRESENT; //this page is now present
	printf("Copying 16 bytes of memory from MEM:%d to SWP:%d", lastEvicted*16, (swapFN*16) - MEMSIZE);
	memcpy(&memory[lastEvicted*16], &swap[(swapFN*16) - MEMSIZE], 16);
	return lastEvicted;
}

int writeToSwap(unsigned char* memory, unsigned char pfn, unsigned char* swap, FILE* swapfile) {
	//Takes memory and a given physical page (numbered 1-3) writes the given page to the swap file
	if (pfn > 3) {
		printf("Can't swap out memory %d that's already in swap !\n", pfn);
		return 0;
	}

	printf("Trying to write pfn %d to swap\n", pfn);
	for (int i = 4; i < 16; i++) {
		//Look for the first open slot in swap space
		if (memory[i] & B_OPEN) {
			//Write to swap!
			memory[i] = memory[pfn];
			memory[i] &= ~B_PRESENT;
			memory[pfn] = B_OPEN; //Clear out our original pfn
			printf("New page table entry for pfn %d: %s\n", i, bytestr(memory[i]));
			//Finally, copy out the actual page data to its new home in the swap
			memcpy(&swap[(i*16) - MEMSIZE], &memory[pfn * 16], 16);
			//And now just flush fake-swap to real swap
			printf("fseek return: %d\n", (int)fseek(swapfile, 0, SEEK_SET));
			printf("Fwrite return: %d\n", fwrite(swap, 1, SWAPSIZE, swapfile));
			fflush(swapfile);
			return 1;
		}
	}
	printf("No room left in swap space!");
	return 0;
}

unsigned char evictPage(unsigned char* memory, unsigned char* swap, FILE* swapfile){
	//Round-robin evict a page
	static unsigned char lastEvicted = 2;
	lastEvicted = ((lastEvicted + 1) % 3) + 1; //possible values: 1, 2, 3
	printf("Round-robin evicting page %d!\n", lastEvicted);
	if (!writeToSwap(memory, lastEvicted, swap, swapfile))
		return 0;
	memory[lastEvicted] = B_OPEN;
	return lastEvicted;
}

unsigned char getNumPages(unsigned char* memory, int pid){
	unsigned char count = 0;
	for (int i = 1; i < 16; i++){
		if (MID(memory[i], 5, 7) == pid) {
			printf("Found page table entry for PID:%d at pfn %d\n", pid, i);
			count++;
		}
	}
	printf("PID %d has %d pages\n", pid, count);
	return count;
}

char* bytestr(unsigned char byte) {
	char *str = malloc(8); //hey look, a memory leak!
	for (char i = 0; i < 8; i++)
		str[7 - i] = BIT(i) & byte ? '1' : '0';
	return str;
}