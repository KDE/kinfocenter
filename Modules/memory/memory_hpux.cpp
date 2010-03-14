/*

    This file will be included by memory.cpp !

    HP-UX is really ugly to retrieve information from !
    Implemented on 24.04.1999 by Helge Deller (deller@gmx.de)
    Tested on an HP9000/715/64 under HPUX-10.20.
    Updated for QT 2.0 on 27.04.1999 (deller)

    Sorry, but I don't know, who implemented the first (little) version of 
    update(), but it showed wrong sizes !

    Many thanks goes also to Mike Romberg, who implemented such functions in
    the program "xosview". Here's his copyright:
    Copyright (c) 1994, 1995 by Mike Romberg ( romberg@fsl.noaa.gov )

*/

#include <stdio.h>
#include <unistd.h>
#include <sys/param.h>
#include <sys/pstat.h>


#define MAX_SWAP_AREAS 16

void KCMMemory::fetchValues()
{
  int  		page_size,i;
  unsigned long total_mem, total_free,
		total_physical, total_swap, free_physical,
		used_physical, used_swap, free_swap;

  struct 	pst_static pststatic;
  struct 	pst_dynamic stats;
  struct 	pst_vminfo vmstats;
  unsigned long fields_[4];
  struct 	pst_swapinfo swapinfo;

  pstat_getstatic( &pststatic, sizeof( struct pst_static ), (size_t)1, 0);
  total_physical = pststatic.physical_memory;
  page_size = (int)pststatic.page_size;

  pstat_getdynamic(&stats, sizeof( pst_dynamic ), (size_t)1, 0);
  pstat_getvminfo(&vmstats, sizeof(vmstats), (size_t)1, 0);

  fields_[0] = stats.psd_rmtxt + stats.psd_arm;  // TEXT
  fields_[1] = stats.psd_rm - stats.psd_rmtxt;	 // USED
  fields_[2] = total_physical - fields_[0] - fields_[1] - stats.psd_free; //OTHER
  fields_[3] = stats.psd_free;	// FREE

  used_physical   = (total_physical - fields_[3]) * page_size;
  total_physical *= page_size;
  free_physical  = (total_physical - used_physical);


  /* Now check the SWAP-AREAS !! */

  total_swap = free_swap  = 0;

  for (i = 0 ; i < MAX_SWAP_AREAS ; i++)
  {
      pstat_getswap(&swapinfo, sizeof(swapinfo), (size_t)1, i);
      if (swapinfo.pss_idx == (unsigned)i)
      {
	  swapinfo.pss_nfpgs *= 4;  // nfpgs is in 512 Byte Blocks....
	  if (swapinfo.pss_nblksenabled == 0) // == 0 ??
	      swapinfo.pss_nblksenabled = swapinfo.pss_nfpgs;
          total_swap += (((unsigned long)swapinfo.pss_nblksenabled) * 1024);
          free_swap  += (((unsigned long)swapinfo.pss_nfpgs       ) * 1024);
      }
  }

  used_swap = total_swap - free_swap;


  /* Now display the results */

  total_mem  = total_physical;   		// + total_swap;
  total_free = (total_physical - used_physical);// + free_swap;

  memoryInfos[TOTAL_MEM]    = MEMORY(total_mem); // total physical memory (without swaps)
  memoryInfos[FREE_MEM]     = MEMORY(total_free);// total free physical memory (without swaps)
  memoryInfos[SHARED_MEM]   = NO_MEMORY_INFO; 		    /* FIXME ?? */
  memoryInfos[BUFFER_MEM]   = MEMORY(fields_[2])*page_size; /* FIXME ?? */
  memoryInfos[SWAP_MEM]     = MEMORY(total_swap); // total size of all swap-partitions
  memoryInfos[FREESWAP_MEM] = MEMORY(free_swap);  // free memory in swap-partitions
#ifdef __GNUC__
#warning "FIXME: memoryInfos[CACHED_MEM]"
#endif
  memoryInfos[CACHED_MEM] = NO_MEMORY_INFO; // cached memory in ram
}
