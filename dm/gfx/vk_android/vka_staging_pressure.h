// Pure pressure and resource budget rules for the Android Vulkan staging allocator.
// Does not free resources: the scratch allocator still waits for GPU completion.
#ifndef VKA_STAGING_PRESSURE_H
#define VKA_STAGING_PRESSURE_H
#include <stdint.h>
#define VKSP_MIB (1ull << 20)
#define VKSP_POLL_NS 500000000ull
#define VKSP_PRESSURE_INFLIGHT (128ull * VKSP_MIB)
// Unknown measurements cannot trigger a low-memory decision by themselves.
static inline int vksp_pressed(uint64_t total_kb, uint64_t avail_kb, double psi_some10) {
    uint64_t reserve_kb = total_kb / 8;
    if (reserve_kb < 512ull * 1024) reserve_kb = 512ull * 1024;
    return (total_kb && avail_kb && avail_kb <= reserve_kb) || psi_some10 >= 5.0;
}
static inline uint64_t vksp_idle_limit(uint64_t configured_bytes, int pressed) {
    return pressed ? 0 : configured_bytes;
}
static inline double vksp_upload_limit(double configured_bytes, int pressed) {
    return pressed && configured_bytes > (double)VKSP_PRESSURE_INFLIGHT
            ? (double)VKSP_PRESSURE_INFLIGHT : configured_bytes;
}
#endif
