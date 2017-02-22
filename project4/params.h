#pragma once

//MACROS
#define BIT(x) (1 << x)
#define LAST(k,n) ((k) & ((1<<(n))-1)) //"Inspired" by stack overflow after my own attempt at turning my masking function into a macro failed
#define MID(k,m,n) LAST((k)>>(m),((n)-(m)))


//FLAGS
#define B_OPEN		BIT(7)
#define B_READ		BIT(4)
#define B_WRTE		BIT(3)

//INSTRUCTIONS	Syntax: pid,instruction,addr,val
#define MAP			0x01	//Allocate a physical page to a given process. 1=rw, 0=r.
#define STR			0x02	//Write supplied value into memory at given location.
#define LDM			0x03	//Load supplied value from memory at given location.

//PARAMETERS
#define MEMSIZE 	64							//bytes
#define PAGECOUNT	4
#define PAGESIZE	(MEMSIZE / PAGECOUNT)		//bytes
#define MAX_PIDC	4							//pids 0 to (MAX_PIDC-1)


