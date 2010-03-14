
#include <stdio.h>
#include <unistd.h>
#include <sys/sysmp.h>
#include <sys/ipc.h>

// The following define is needed for SGI IRIX 6.2
#define _KMEMUSER
#include <sys/shm.h>

#include <sys/param.h>
#include <sys/swap.h>

#ifndef UBSIZE
#define UBSIZE 512
#endif


void KCMMemory::fetchValues()
{
  int pagesize = getpagesize();

  struct rminfo rmi;
  if( sysmp(MP_SAGET, MPSA_RMINFO, &rmi, sizeof(rmi)) == -1 )
    return;
  memoryInfos[TOTAL_MEM]    = MEMORY(rmi.physmem) * pagesize; // total physical memory (without swaps)
  memoryInfos[FREE_MEM]     = MEMORY(rmi.freemem) * pagesize; // total free physical memory (without swaps)
  memoryInfos[BUFFER_MEM]   = MEMORY(rmi.bufmem)  * pagesize;


  //FIXME: memoryInfos[CACHED_MEM]"
  memoryInfos[CACHED_MEM] = NO_MEMORY_INFO; // cached memory in ram
  
  long val;
  swapctl(SC_GETSWAPTOT, &val);
  memoryInfos[SWAP_MEM]     = MEMORY(val) * UBSIZE; // total size of all swap-partitions

  swapctl(SC_GETFREESWAP, &val);
  memoryInfos[FREESWAP_MEM] = MEMORY(val) * UBSIZE; // free memory in swap-partitions

#ifndef MPKA_SHMINFO
  /* Irix 6.5 (also 6.4?) */
  memoryInfos[SHARED_MEM]   = NO_MEMORY_INFO;
#else
  FILE *kmem = fopen("/dev/kmem", "r");
  if( kmem == 0 ) {
    memoryInfos[SHARED_MEM]   = NO_MEMORY_INFO; 
    return;
  }

  long shmip = sysmp(MP_KERNADDR, MPKA_SHMINFO);
  fseek( kmem, shmip, 0 );
  struct shminfo shmi;
  fread( &shmi, sizeof(shmi), 1, kmem );

  long shmem = sysmp(MP_KERNADDR, MPKA_SHM);

  val = 0;
  long pos;
  struct shmid_ds shmid;
  for( int i=0 ; i<shmi.shmmni ; i++ ) {
    fseek( kmem, shmem, 0 );
	shmem += sizeof(shmem);
    fread( &pos, sizeof(shmem), 1, kmem );
	if(pos != 0) {
      fseek( kmem, pos, 0 );
      fread( &shmid, sizeof(shmid), 1, kmem );
      val += shmid.shm_segsz;
    }
  }
  memoryInfos[SHARED_MEM]   = MEMORY(val);

  fclose(kmem);
#endif
}
