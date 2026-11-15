// Conservative, opt-in deferred PAGEOUT admission policy. No Vulkan, no locks.
// A caller must still verify the mapping and GPU lifetime under g_pl.
#ifndef VKA_FILEBACK_RECLAIM_H
#define VKA_FILEBACK_RECLAIM_H
#include <stdint.h>
#define VKFR_MIB (1ull << 20)
#define VKFR_POLL_SEC 5u
#define VKFR_AGE_NS (10ull * 1000000000ull)
#define VKFR_COOLDOWN_NS (30ull * 1000000000ull)
#define VKFR_MIN_RESIDENT (32ull * VKFR_MIB)
static inline int vkfr_candidate(uint64_t size, uint64_t bytes_copied) {
    return size >= 64 * VKFR_MIB && bytes_copied >= 16 * VKFR_MIB;
}
static inline int vkfr_pressure(uint64_t total_kb, uint64_t available_kb, unsigned consecutive) {
    if (!total_kb || !available_kb || consecutive < 2) return 0;
    uint64_t reserve = total_kb / 8;
    if (reserve < 512ull * 1024) reserve = 512ull * 1024;
    return available_kb <= reserve;
}
static inline int vkfr_due(uint64_t now_ns, uint64_t created_ns, uint64_t last_try_ns) {
    if (!created_ns || now_ns < created_ns || now_ns - created_ns < VKFR_AGE_NS) return 0;
    return !last_try_ns || (now_ns >= last_try_ns && now_ns - last_try_ns >= VKFR_COOLDOWN_NS);
}
static inline int vkfr_resident(uint64_t resident_bytes) {
    return resident_bytes >= VKFR_MIN_RESIDENT;
}
// A slice must still have the original file-offset identity; an address alone
// is not enough after placement, finalization, or re-use of a guest memory range.
static inline int vkfr_piece_covers(uintptr_t piece_lo, uintptr_t piece_hi, uint64_t piece_off,
                                     uintptr_t slice_lo, uintptr_t slice_hi, uint64_t slice_off) {
    if (piece_lo > slice_lo || piece_hi < slice_hi || slice_lo >= slice_hi) return 0;
    uint64_t delta = (uint64_t)(slice_lo - piece_lo);
    return piece_off <= UINT64_MAX - delta && piece_off + delta == slice_off;
}
#endif
