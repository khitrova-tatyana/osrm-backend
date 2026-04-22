#ifndef MEMINFO_HPP
#define MEMINFO_HPP

#include "util/log.hpp"
#include <cstddef>
#include <iomanip>

#ifndef _WIN32
#include <sys/resource.h>
#else
#include <windows.h>
#include <psapi.h>
#endif

namespace osrm::util
{

inline size_t PeakRAMUsedInBytes()
{
#ifndef _WIN32
    rusage usage;
    getrusage(RUSAGE_SELF, &usage);
#ifdef __linux__
    // Under linux, ru.maxrss is in kb
    return usage.ru_maxrss * 1024;
#else  // __linux__
    // Under BSD systems (OSX), it's in bytes
    return usage.ru_maxrss;
#endif // __linux__
#else  // _WIN32
    PROCESS_MEMORY_COUNTERS pmc;
    if (GetProcessMemoryInfo(GetCurrentProcess(), &pmc, sizeof(pmc)))
        return pmc.PeakWorkingSetSize;
    return 0;
#endif // _WIN32
}

inline void DumpMemoryStats()
{
    util::Log() << "Peak RAM: " << std::setprecision(3)
                << static_cast<double>(PeakRAMUsedInBytes()) /
                       static_cast<double>(1024 * 1024)
                << "MB";
}
} // namespace osrm::util

#endif