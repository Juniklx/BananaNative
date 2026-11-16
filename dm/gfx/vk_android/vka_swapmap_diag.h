// Bounded, optional process smaps attribution for Android low-memory debugging.
// Only parses kernel-provided text; no access to guest memory or Vulkan resources.
#ifndef VKA_SWAPMAP_DIAG_H
#define VKA_SWAPMAP_DIAG_H
#include <stdint.h>
#include <stdio.h>
#include <string.h>
#define VKSM_NAME 96
#define VKSM_TOP 8
#define VKSM_MAX_BYTES (32ull << 20)
#define VKSM_POLL_SEC 1u
// Early diagnostic threshold, intentionally separate from the more severe
// file-backed PAGEOUT threshold. Captures SwapPss before the game stalls at
// <1 fps; never changes memory mapping, eviction, or allocator policy.
static inline int vksm_diag_due(uint64_t mem_total_kb, uint64_t mem_avail_kb) {
    if (!mem_total_kb || !mem_avail_kb) return 0;
    uint64_t reserve = mem_total_kb / 8;
    if (reserve < 512ull * 1024) reserve = 512ull * 1024;
    return mem_avail_kb <= reserve * 2;
}
typedef struct { uint64_t lo, hi, rss_kb, swap_kb; char name[VKSM_NAME]; } vksm_vma;
enum { VKSM_GUEST, VKSM_GPU, VKSM_ANON, VKSM_FILE, VKSM_GROUPS };
static inline int vksm_map_line(const char *line, vksm_vma *v) {
    unsigned long long lo = 0, hi = 0, offset = 0, ino = 0;
    unsigned major = 0, minor = 0; char perm[5] = {0}, name[VKSM_NAME] = {0};
    int n = sscanf(line, "%llx-%llx %4s %llx %x:%x %llu %95[^\n]",
                   &lo, &hi, perm, &offset, &major, &minor, &ino, name);
    if (n < 7 || hi <= lo) return 0;
    *v = (vksm_vma){.lo=lo, .hi=hi};
    snprintf(v->name, sizeof v->name, "%s", n == 8 ? name : "(anonymous)");
    return 1;
}
static inline int vksm_field_kb(const char *line, const char *key, uint64_t *out) {
    size_t n = strlen(key);
    if (strncmp(line, key, n)) return 0;
    unsigned long long val = 0;
    if (sscanf(line + n, " %llu kB", &val) != 1) return 0;
    *out = val;
    return 1;
}
static inline int vksm_group(const char *name) {
    if (strstr(name, "guest") || strstr(name, "Guest")) return VKSM_GUEST;
    if (strstr(name, "kgsl") || strstr(name, "/dev/dri") ||
        strstr(name, "mali") || strstr(name, "xclipse")) return VKSM_GPU;
    if (name[0] == '/') return VKSM_FILE;
    return VKSM_ANON;
}
#endif
