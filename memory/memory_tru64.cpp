/*
 * This is memory_tru64.cpp to retrieve memory information under Tru64/Alpha.
 *
 * Implemented by Tom Leitner, Tom@radar.tu-graz.ac.at
 *
 * WARNING: This module requires linking with -lmach
 *
 * This routine is based on m_decosf1.c from the "top" program written by:
 *
 * AUTHOR:  Anthony Baxter, <anthony@aaii.oz.au>
 *
 */

#include <stdio.h>
#include <unistd.h>
#include <sys/socket.h>
#include <sys/mbuf.h>
#include <net/route.h>
#include <sys/table.h>
extern "C" {
#include <mach/mach_traps.h>
}
#include <mach/vm_statistics.h>

#define pagetob(size) (MEMORY(1024L) * ((long) (size) << (long) pageshift))
#define LOG1024       10

extern "C" void vm_statistics(task_t, vm_statistics_data_t*);

void KCMMemory::fetchValues()
{
  int pageshift;          /* log base 2 of the pagesize */
  register int pagesize;
  vm_statistics_data_t vmstats;
  int swap_pages=0,swap_free=0,i;
  struct tbl_swapinfo swbuf;

  /* get the page size with "getpagesize" and calculate pageshift from it */

  pagesize = getpagesize();
  pageshift = 0;
  while (pagesize > 1) {
      pageshift++;
      pagesize >>= 1;
  }

  /* we only need the amount of log(2)1024 for our conversion */

  pageshift -= LOG1024;

  /* memory information */
  /* this is possibly bogus - we work out total # pages by */
  /* adding up the free, active, inactive, wired down, and */
  /* zero filled. Anyone who knows a better way, TELL ME!  */
  /* Change: don't use zero filled. */

  (void) ::vm_statistics(::task_self(), &vmstats);

  /* thanks DEC for the table() command. No thanks at all for   */
  /* omitting the man page for it from OSF/1 1.2, and failing   */
  /* to document SWAPINFO in the 1.3 man page. Lets hear it for */
  /* include files. */

  i=0;
  while(table(TBL_SWAPINFO,i,&swbuf,1,sizeof(struct tbl_swapinfo))>0) {
      swap_pages += swbuf.size;
      swap_free  += swbuf.free;
      i++;
  }
  memoryInfos[TOTAL_MEM]    = pagetob((vmstats.free_count +
                                       vmstats.active_count +
                                       vmstats.inactive_count +
                                       vmstats.wire_count));
  memoryInfos[FREE_MEM]     = pagetob(vmstats.free_count);
  memoryInfos[SHARED_MEM]   = NO_MEMORY_INFO; 		    /* FIXME ?? */
  memoryInfos[BUFFER_MEM]   = NO_MEMORY_INFO; 		    /* FIXME ?? */
#ifdef __GNUC__
#warning "FIXME: memoryInfos[CACHED_MEM]"
#endif
  memoryInfos[CACHED_MEM]   = NO_MEMORY_INFO; /* cached memory in ram */
  memoryInfos[SWAP_MEM]     = pagetob(swap_pages);
  memoryInfos[FREESWAP_MEM] = pagetob(swap_free);
}
