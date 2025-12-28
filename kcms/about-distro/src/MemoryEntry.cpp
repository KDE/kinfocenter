/*
    SPDX-FileCopyrightText: 2012-2020 Harald Sitter <sitter@kde.org>
    SPDX-License-Identifier: GPL-2.0-only OR GPL-3.0-only OR LicenseRef-KDE-Accepted-GPL
*/

#include "MemoryEntry.h"

#include <KFormat>

#ifdef Q_OS_LINUX
#include <sys/sysinfo.h>
#ifdef UDEV_FOUND
#include <libudev.h>
#endif
#elif defined(Q_OS_FREEBSD) || defined(Q_OS_OPENBSD)
// clang-format off
#include <unistd.h>
#include <sys/types.h>
#include <sys/sysctl.h>
// clang-format on
#endif

MemoryEntry::MemoryEntry()
    : Entry(ki18n("Memory:"), QString() /* overridden here */)
{
}

std::optional<qlonglong> MemoryEntry::calculateTotalRam()
{
#if defined(Q_OS_LINUX) && defined(UDEV_FOUND)
    std::unique_ptr<struct udev, decltype(&udev_unref)> udev(udev_new(), &udev_unref);
    if (!udev) {
        return {};
    }

    std::unique_ptr<struct udev_device, decltype(&udev_device_unref)> dmi(udev_device_new_from_syspath(udev.get(), "/sys/class/dmi/id/"), &udev_device_unref);
    if (!dmi) {
        return {};
    }

    const char *numMemoryDevicesCStr = udev_device_get_property_value(dmi.get(), "MEMORY_ARRAY_NUM_DEVICES");
    if (!numMemoryDevicesCStr) {
        return {};
    }

    bool ok;
    int numMemoryDevices = QByteArray(numMemoryDevicesCStr).toInt(&ok);
    if (!ok) {
        return {};
    }

    qlonglong totalBytes = 0;
    for (int i = 0; i < numMemoryDevices; ++i) {
        const char *memoryBytesCStr = udev_device_get_property_value(dmi.get(), QStringLiteral("MEMORY_DEVICE_%1_SIZE").arg(i).toLatin1());
        qlonglong memoryBytes = QByteArray(memoryBytesCStr).toLongLong(&ok);
        if (ok) {
            totalBytes += memoryBytes;
        }
    }

    return totalBytes;
#elif defined(Q_OS_OPENBSD)
    int64_t memSize = 0;
    int mib[] = {CTL_HW, HW_PHYSMEM64};
    size_t sz = sizeof(memSize);
    if (sysctl(mib, 2, &memSize, &sz, NULL, 0) == 0) {
        return memSize;
    }
#endif

    /*
     * TODO: A FreeBSD impl is likely possible, but it appears that
     *         sysctlbyname() cannot get what we want with either "hw.physmem",
     *         "hw.usermem" or "hw.realmem".
     *       On a system with 2 x 4 GiB memory modules installed, we would need
     *         to return a value of 8 GiB in bytes.
     */

    return {};
}

std::optional<qlonglong> MemoryEntry::calculateAvailableRam()
{
#ifdef Q_OS_LINUX
    struct sysinfo info;
    if (sysinfo(&info) == 0) {
        // manpage: "sizes are given as multiples of mem_unit bytes"
        return qlonglong(info.totalram) * info.mem_unit;
    }
#elif defined(Q_OS_OPENBSD)
    struct uvmexp uvmexp;
    int mib[] = {CTL_VM, VM_UVMEXP};
    size_t sz = sizeof(uvmexp);
    if (sysctl(mib, 2, &uvmexp, &sz, NULL, 0) == 0) {
        int64_t pagesize = uvmexp.pagesize;
        int64_t available = (int64_t)(uvmexp.free + uvmexp.inactive) * pagesize;
        return available;
    }

#elif defined(Q_OS_FREEBSD)
    /* Stuff for sysctl */
    unsigned long memory;
    size_t len = sizeof(memory);
    if (sysctlbyname("hw.physmem", &memory, &len, NULL, 0) == 0) {
        return memory;
    }
#endif

    return {};
}

QString MemoryEntry::localizedValue(Language language) const
{
    auto precisionForGiB = [](std::optional<qlonglong> bytes) -> int {
        if (!bytes.has_value()) {
            return 0;
        }

        constexpr qlonglong GiB = 1024 * 1024 * 1024;
        return (bytes.value() % GiB == 0) ? 0 : 1;
    };

    const int totalRamPrecision = precisionForGiB(m_totalRam);
    const int availableRamPrecision = precisionForGiB(m_availableRam);

    if (m_totalRam.has_value() && m_availableRam.has_value()) {
        // Both known
        const auto string = ki18nc("@label, %1 is the total amount of installed system memory, %2 is the amount of which is usable, both expressed as 7.7 GiB",
                                   "%1 of RAM (%2 usable)")
                                .subs(KFormat(localeForLanguage(language)).formatByteSize(m_totalRam.value(), totalRamPrecision))
                                .subs(KFormat(localeForLanguage(language)).formatByteSize(m_availableRam.value(), availableRamPrecision));
        return localize(string, language);
    }

    if (m_totalRam.has_value() && !m_availableRam.has_value()) {
        // Known total, unknown available
        const auto string = ki18nc("@label, %1 is the amount of installed system memory expressed as 7.7 GiB", "%1 of RAM")
                                .subs(KFormat(localeForLanguage(language)).formatByteSize(m_totalRam.value(), totalRamPrecision));
        return localize(string, language);
    }

    if (!m_totalRam.has_value() && m_availableRam.has_value()) {
        // Unknown total, known available
        const auto string = ki18nc("@label, %1 is the amount of usable system memory expressed as 7.7 GiB", "%1 of usable RAM")
                                .subs(KFormat(localeForLanguage(language)).formatByteSize(m_availableRam.value(), availableRamPrecision));
        return localize(string, language);
    }

    // Both unknown
    return localize(ki18nc("@label, Unknown amount of system memory", "Unknown"), language);
}

QString MemoryEntry::localizedHelp(Language language) const
{
    if (m_totalRam.has_value() && m_availableRam.has_value()) {
        // Both known
        return localize(ki18nc("@info:tooltip, referring to system memory or RAM",
                               "Some memory is reserved for use by the kernel or system hardware such as integrated graphics memory."),
                        language);
    }

    if (m_totalRam.has_value() && !m_availableRam.has_value()) {
        // Known total, unknown available
        return localize(
            ki18nc("@info:tooltip, referring to system memory or RAM",
                   "The amount of usable memory may be lower than the displayed amount because some memory is reserved for use by the kernel or system "
                   "hardware, such as integrated graphics memory."),
            language);
    }

    if (!m_totalRam.has_value() && m_availableRam.has_value()) {
        // Unknown total, known available
        return localize(
            ki18nc("@info:tooltip, referring to system memory or RAM",
                   "The amount of memory displayed may be lower than the installed amount because some memory is reserved for use by the kernel or system "
                   "hardware, such as integrated graphics memory."),
            language);
    }

    // Both unknown
    return QString();
}
