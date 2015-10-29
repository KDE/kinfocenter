#include <sys/sysinfo.h>
#include <unistd.h>
#include <stdlib.h>
#include <QFile>

void KCMMemory::fetchValues()
{
  struct sysinfo info;
  
  sysinfo(&info);	/* Get Information from system... */
  
  /* 
   * The sysinfo.mem_unit member variable is not available in older 2.4 kernels.
   * If you have troubles compiling this code, set mem_unit to "1".
   */
    
  const int mem_unit = info.mem_unit;

  memoryInfos[TOTAL_MEM]    = MEMORY(info.totalram)  * mem_unit; // total physical memory (without swaps)
  memoryInfos[FREE_MEM]     = MEMORY(info.freeram)   * mem_unit; // total free physical memory (without swaps)
  memoryInfos[SHARED_MEM]   = MEMORY(info.sharedram) * mem_unit; 
  memoryInfos[BUFFER_MEM]   = MEMORY(info.bufferram) * mem_unit; 
  memoryInfos[SWAP_MEM]     = MEMORY(info.totalswap) * mem_unit; // total size of all swap-partitions
  memoryInfos[FREESWAP_MEM] = MEMORY(info.freeswap)  * mem_unit; // free memory in swap-partitions
  
  QFile file(QStringLiteral("/proc/meminfo"));
  if (file.open(QIODevice::ReadOnly)) {
	char buf[512];
	while (file.readLine(buf, sizeof(buf) - 1) > 0) {
		if (strncmp(buf,"Cached:",7)==0) {
			unsigned long v;
			v = strtoul(&buf[7],NULL,10);			
			memoryInfos[CACHED_MEM] = MEMORY(v) * 1024; // Cached memory in RAM
			break;
		}
	}
	file.close();
  }
}

