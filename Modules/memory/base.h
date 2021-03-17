#ifndef BASE_H_
#define BASE_H_

#include <QColor>

#define COLOR_USED_MEMORY QColor(0x83,0xDD, 0xF5)
//start : 0x00, 0x71, 0xBC
//end : 0x83,0xDD, 0xF5

//#define COLOR_USED_MEMORY QColor(236,91,47)

#define COLOR_USED_SWAP   QColor(255,134,64)

//#define COLOR_FREE_MEMORY QColor(0x00, 0x71, 0xBC)
#define COLOR_FREE_MEMORY QColor(216, 231, 227)

/* better to use quint64, because some 32bit-machines have more total
 memory (with swap) than just the 4GB which fits into a 32bit-long */
typedef quint64 t_memsize;


enum { /* entries for memoryInfos[] */
	TOTAL_MEM = 0, /* total physical memory (without swaps) */
    FREE_MEM, /* total free physical memory (without swaps) */
#if !defined(__NetBSD__) && !defined(__OpenBSD__)
	SHARED_MEM, /* shared memory size */
    BUFFER_MEM, /* buffered memory size */
#endif
	CACHED_MEM, /* cache memory size (located in ram) */
	SWAP_MEM, /* total size of all swap-partitions */
	FREESWAP_MEM, /* free memory in swap-partitions */
	MEM_LAST_ENTRY,
};


#define MEMORY(x)	((t_memsize) (x))	/* it's easier... */
#define NO_MEMORY_INFO	MEMORY(-1)		/* DO NOT CHANGE */
#define ZERO_IF_NO_INFO(value) ((value) != NO_MEMORY_INFO ? (value) : 0)


#define SPACING 16

#endif /*BASE_H_*/
