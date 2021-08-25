#include <sys/sysctl.h>
#include <sys/types.h>
#include <sys/vmmeter.h>

#include <kvm.h>
#include <vm/vm_param.h>

#include <fcntl.h>
#include <paths.h>
#include <unistd.h>

void KCMMemory::fetchValues()
{
    /* Stuff for sysctl */
    size_t len;

    unsigned long memory;
    len = sizeof(memory);
    sysctlbyname("hw.physmem", &memory, &len, NULL, 0);

    // Numerical values

    // total physical memory (without swap space)
    memoryInfos[TOTAL_MEM] = MEMORY(memory);

    unsigned int cached;
    len = sizeof(cached);
    if (sysctlbyname("vm.stats.vm.v_cache_count", &cached, &len, NULL, 0) == -1 || !len)
        memoryInfos[CACHED_MEM] = NO_MEMORY_INFO;
    else
        memoryInfos[CACHED_MEM] = MEMORY(cached) * PAGE_SIZE;

    unsigned int free;
    len = sizeof(free);
    if (sysctlbyname("vm.stats.vm.v_free_count", &free, &len, NULL, 0) == -1 || !len)
        memoryInfos[FREE_MEM] = NO_MEMORY_INFO;
    else
        memoryInfos[FREE_MEM] = MEMORY(free) * PAGE_SIZE;

    // added by Brad Hughes bhughes@trolltech.com
    struct vmtotal vmem;

    len = sizeof(vmem);
    if (sysctlbyname("vm.vmtotal", &vmem, &len, NULL, 0) == -1 || !len)
        memoryInfos[SHARED_MEM] = NO_MEMORY_INFO;
    else
        memoryInfos[SHARED_MEM] = MEMORY(vmem.t_armshr) * PAGE_SIZE;

    long buffers;
    len = sizeof(buffers);
    if ((sysctlbyname("vfs.bufspace", &buffers, &len, NULL, 0) == -1) || !len)
        memoryInfos[BUFFER_MEM] = NO_MEMORY_INFO;
    else
        memoryInfos[BUFFER_MEM] = MEMORY(buffers);

    struct kvm_swap swap[1];
    kvm_t *kvm_handle;
    kvm_handle = kvm_open(NULL, _PATH_DEVNULL, NULL, O_RDONLY, "kvm_open");

    if (kvm_handle != NULL && kvm_getswapinfo(kvm_handle, swap, 1, 0) != -1) {
        memoryInfos[SWAP_MEM] = MEMORY(swap[0].ksw_total) * PAGE_SIZE;
        memoryInfos[FREESWAP_MEM] = MEMORY(swap[0].ksw_total - swap[0].ksw_used) * PAGE_SIZE;
    }

    if (kvm_handle != NULL)
        kvm_close(kvm_handle);
}
