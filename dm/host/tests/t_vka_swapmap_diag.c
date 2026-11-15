// Synthetic smaps parsing checks; never access another process.
#include "../../gfx/vk_android/vka_swapmap_diag.h"
#include <stdio.h>
#define CHECK(x) do { if (!(x)) { fprintf(stderr, "FAILED %d: %s\n", __LINE__, #x); return 1; } } while (0)
int main(void) {
    vksm_vma v = {0}; uint64_t kb = 0;
    CHECK(vksm_map_line("1000-3000 rw-p 00000000 00:00 0 [anon:guest]\n", &v));
    CHECK(v.lo == 0x1000 && v.hi == 0x3000 && !strcmp(v.name, "[anon:guest]"));
    CHECK(vksm_group(v.name) == VKSM_GUEST);
    CHECK(vksm_map_line("7aa000-7ba000 r--p 00000000 00:00 0\n", &v));
    CHECK(!strcmp(v.name, "(anonymous)"));
    CHECK(vksm_group(v.name) == VKSM_ANON);
    CHECK(vksm_map_line("7000-9000 rw-s 00000000 00:05 123 /data/user/0/cache/pool (deleted)\n", &v));
    CHECK(vksm_group(v.name) == VKSM_FILE);
    CHECK(vksm_group("/dev/kgsl-3d0") == VKSM_GPU);
    CHECK(vksm_field_kb("SwapPss:       12345 kB\n", "SwapPss:", &kb) && kb == 12345);
    CHECK(vksm_field_kb("Rss:              33 kB\n", "Rss:", &kb) && kb == 33);
    CHECK(!vksm_field_kb("SwapPss: INVALID kB", "SwapPss:", &kb));
    CHECK(!vksm_field_kb("Swap: 120 kB", "SwapPss:", &kb));
    CHECK(!vksm_map_line("VmFlags: rd wr mr mw me ac\n", &v));
    CHECK(!vksm_map_line("1000-1000 rw-p 00000000 00:00 0\n", &v));
    puts("t_vka_swapmap_diag: ok"); return 0;
}
