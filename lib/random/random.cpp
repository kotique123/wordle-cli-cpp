#include <stdlib.h>
#ifdef _WIN32
#include <windows.h>
#include <processthreadsapi.h>
#else
#include <unistd.h>
#include <fstream>
#endif
#include <chrono>
#include "random.hpp"
#include <random>
// Returns a random int in [lower, higher] using time and device info as seed
int random_int(int lower, int higher) {
    // Get current time
    auto now = std::chrono::system_clock::now();
    auto now_time = std::chrono::system_clock::to_time_t(now);
    unsigned int device_val = 0;
#ifdef _WIN32
    // Use process ID and system tick count for entropy on Windows
    DWORD pid = GetCurrentProcessId();
    DWORD ticks = GetTickCount();
    device_val = static_cast<unsigned int>(pid) ^ static_cast<unsigned int>(ticks);
#else
    // Use process ID and /proc/self/stat for entropy on Unix
    pid_t pid = getpid();
    std::ifstream stat_file("/proc/self/stat");
    int stat_val = 0;
    if (stat_file.is_open()) {
        stat_file >> stat_val;
        stat_file.close();
    }
    device_val = static_cast<unsigned int>(pid) ^ static_cast<unsigned int>(stat_val);
#endif
    // Combine for seed
    unsigned int seed = static_cast<unsigned int>(now_time) ^ device_val;
    srand(seed);
    int range = higher - lower + 1;
    return lower + rand() % range;
}
