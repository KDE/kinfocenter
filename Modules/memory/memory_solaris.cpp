/*
 *  memory_solaris.cpp
 *
 *  Torsten Kasch <tk@Genetik.Uni-Bielefeld.DE>
 */

#include <unistd.h>
#include <stdlib.h>
#include <kstat.h>

/* Stop <sys/swap.h> from crapping out on 32-bit architectures. */

#if !defined(_LP64) && _FILE_OFFSET_BITS == 64
# undef _FILE_OFFSET_BITS
# define _FILE_OFFSET_BITS 32
#endif

#include <sys/stat.h>
#include <sys/swap.h>
#include <vm/anon.h>

#define PAGETOK(a) (( (t_memsize) sysconf( _SC_PAGESIZE )) *  (t_memsize) a)

void KCMMemory::fetchValues() {

	kstat_ctl_t	*kctl;
	kstat_t		*ksp;
	kstat_named_t	*kdata;

	/*
	 *  get a kstat handle first and update the user's kstat chain
	 */
	if( (kctl = kstat_open()) == NULL )
		return;
	while( kstat_chain_update( kctl ) != 0 )
		;

	/*
	 *  traverse the kstat chain to find the appropriate kstat
	 */
	if( (ksp = kstat_lookup( kctl, "unix", 0, "system_pages" )) == NULL )
		return;

	if( kstat_read( kctl, ksp, NULL ) == -1 )
		return;

	/*
	 *  lookup the data
	 */
#if 0
	kdata = (kstat_named_t *) kstat_data_lookup( ksp, "physmem" );
	if( kdata != NULL ) {
		memoryInfos[TOTAL_MEM] = PAGETOK(kdata->value.ui32);
	}
#endif
	memoryInfos[TOTAL_MEM] = PAGETOK(sysconf(_SC_PHYS_PAGES));

	kdata = (kstat_named_t *) kstat_data_lookup( ksp, "freemem" );
	if( kdata != NULL )
		memoryInfos[FREE_MEM] = PAGETOK(kdata->value.ui32);
#ifdef __GNUC__
#warning "FIXME: memoryInfos[CACHED_MEM]"
#endif	
	memoryInfos[CACHED_MEM] = NO_MEMORY_INFO; // cached memory in ram
	  
	kstat_close( kctl );

	/*
	 *  Swap Info
	 */

	struct anoninfo		am_swap;
	long			swaptotal;
	long			swapfree;
	long			swapused;

	swaptotal = swapused = swapfree = 0L;

	/*
	 *  Retrieve overall swap information from anonymous memory structure -
	 *  which is the same way "swap -s" retrieves it's statistics.
	 *
	 *  swapctl(SC_LIST, void *arg) does not return what we are looking for.
	 */

	if (swapctl(SC_AINFO, &am_swap) == -1)
		return;

	swaptotal = am_swap.ani_max;
	swapused = am_swap.ani_resv;
	swapfree = swaptotal - swapused;

	memoryInfos[SWAP_MEM]     = PAGETOK(swaptotal);
	memoryInfos[FREESWAP_MEM] = PAGETOK(swapfree);
}
