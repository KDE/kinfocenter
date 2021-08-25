
#include <sys/param.h>
#if __NetBSD_Version__ > 103080000
#define UVM
#endif
#if defined(__OpenBSD__)
#define UVM
#endif

#include <stdlib.h>
#include <sys/sysctl.h>
#include <sys/types.h>
#include <unistd.h>
#ifdef UVM
#include <uvm/uvm_extern.h>
#else
#include <vm/vm_swap.h>
#endif

void KCMMemory::fetchValues()
{
    int mib[2];
    size_t len;
#ifdef UVM
#if __NetBSD_Version__ > 106000000
    struct uvmexp_sysctl uvmexp;
#else
    struct uvmexp uvmexp;
#endif
#else
    struct swapent *swaplist;
    int64_t nswap, rnswap, totalswap, freeswap, usedswap;
#endif
#if __NetBSD_Version__ > 106170000 /* 1.6Q+ */
    quad_t memory;
#else
    int memory;
#endif

    /* memory */
#if __NetBSD_Version__ > 106170000 /* 1.6Q+ */
    mib[0] = CTL_HW;
    mib[1] = HW_PHYSMEM64;
#else
    mib[0] = CTL_HW;
    mib[1] = HW_PHYSMEM;
#endif
    len = sizeof(memory);
    if (sysctl(mib, 2, &memory, &len, NULL, 0) < 0)
        memoryInfos[TOTAL_MEM] = NO_MEMORY_INFO;
    else
        memoryInfos[TOTAL_MEM] = memory;

#ifdef UVM
    mib[0] = CTL_VM;
#if __NetBSD_Version__ > 106000000
    mib[1] = VM_UVMEXP2;
#else
    mib[1] = VM_UVMEXP;
#endif
    len = sizeof(uvmexp);
    if (sysctl(mib, 2, &uvmexp, &len, NULL, 0) < 0) {
        memoryInfos[FREE_MEM] = NO_MEMORY_INFO;
        memoryInfos[ACTIVE_MEM] = NO_MEMORY_INFO;
        memoryInfos[INACTIVE_MEM] = NO_MEMORY_INFO;
        memoryInfos[SWAP_MEM] = NO_MEMORY_INFO;
        memoryInfos[FREESWAP_MEM] = NO_MEMORY_INFO;
        memoryInfos[CACHED_MEM] = NO_MEMORY_INFO;
    } else {
        t_memsize pgsz = MEMORY(uvmexp.pagesize);
        memoryInfos[FREE_MEM] = pgsz * uvmexp.free;
        memoryInfos[ACTIVE_MEM] = pgsz * uvmexp.active;
        memoryInfos[INACTIVE_MEM] = pgsz * uvmexp.inactive;
        memoryInfos[SWAP_MEM] = pgsz * uvmexp.swpages;
        memoryInfos[FREESWAP_MEM] = pgsz * (uvmexp.swpages - uvmexp.swpginuse);
#if __NetBSD_Version__ > 106000000
        memoryInfos[CACHED_MEM] = pgsz * (uvmexp.filepages + uvmexp.execpages);
#else
        memoryInfos[CACHED_MEM] = NO_MEMORY_INFO;
#endif
    }
#else
    memoryInfos[FREE_MEM] = NO_MEMORY_INFO;
    memoryInfos[ACTIVE_MEM] = NO_MEMORY_INFO;
    memoryInfos[INACTIVE_MEM] = NO_MEMORY_INFO;

    /* swap */
    totalswap = freeswap = usedswap = 0;
    nswap = swapctl(SWAP_NSWAP, 0, 0);
    if (nswap > 0) {
        if ((swaplist = (struct swapent *)malloc(nswap * sizeof(*swaplist)))) {
            rnswap = swapctl(SWAP_STATS, swaplist, nswap);
            if (rnswap < 0 || rnswap > nswap)
                totalswap = freeswap = -1; /* Error */
            else {
                while (rnswap-- > 0) {
                    totalswap += swaplist[rnswap].se_nblks;
                    usedswap += swaplist[rnswap].se_inuse;
                }
                freeswap = totalswap - usedswap;
            }
        } else
            totalswap = freeswap = -1; /* Error */

        if (totalswap == -1) {
            memoryInfos[SWAP_MEM] = NO_MEMORY_INFO;
            memoryInfos[FREESWAP_MEM] = NO_MEMORY_INFO;
        } else {
            memoryInfos[SWAP_MEM] = MEMORY(totalswap);
            memoryInfos[FREESWAP_MEM] = MEMORY(freeswap);
        }
    }
#endif
}
