// Unit checks for deferred reclaim admission; no file or memory lifetime effects.
#include "../../gfx/vk_android/vka_fileback_reclaim.h"
#include <stdio.h>
#define CHECK(x) do { if (!(x)) { fprintf(stderr, "FAILED line %d: %s\n", __LINE__, #x); return 1; } } while (0)
int main(void) {
    const uint64_t M = VKFR_MIB;
    const uint64_t total = 7224ull * 1024, reserve = total / 8;
    CHECK(vkfr_candidate(232*M, 217*M));
    CHECK(vkfr_candidate(2432*M, 259*M));
    CHECK(!vkfr_candidate(63*M, 60*M));
    CHECK(!vkfr_candidate(200*M, 15*M));
    CHECK(!vkfr_candidate(0, 0));
    CHECK(!vkfr_pressure(total, 632*1024, 1));
    CHECK(vkfr_pressure(total, 632*1024, 2));
    CHECK(vkfr_pressure(total, reserve, 2));
    CHECK(!vkfr_pressure(total, reserve+1, 2));
    CHECK(!vkfr_pressure(total, 0, 2));
    CHECK(!vkfr_pressure(0, 632*1024, 2));
    CHECK(!vkfr_pressure(total, 3293*1024, 3));
    CHECK(!vkfr_due(0, 0, 0));
    CHECK(!vkfr_due(9ull * 1000000000, 1, 0));
    CHECK(vkfr_due(11ull * 1000000000, 1, 0));
    CHECK(!vkfr_due(20ull * 1000000000, 1, 15ull * 1000000000));
    CHECK(vkfr_due(46ull * 1000000000, 1, 15ull * 1000000000));
    CHECK(!vkfr_due(5, 10, 0));
    CHECK(!vkfr_resident(8*M-1));
    CHECK(vkfr_resident(8*M));
    CHECK(vkfr_resident(32*M));
    CHECK(VKFR_SCAN_MAX == 8);
    CHECK(VKFR_SLICE_BYTES == 64*M);
    CHECK(vkfr_next_cursor(0, 232*M, VKFR_SLICE_BYTES) == 64*M);
    CHECK(vkfr_next_cursor(64*M, 232*M, VKFR_SLICE_BYTES) == 128*M);
    CHECK(vkfr_next_cursor(128*M, 232*M, VKFR_SLICE_BYTES) == 192*M);
    CHECK(vkfr_next_cursor(192*M, 232*M, VKFR_SLICE_BYTES) == 0);
    CHECK(vkfr_next_cursor(0, 64*M, VKFR_SLICE_BYTES) == 0);
    CHECK(vkfr_next_cursor(0, 0, VKFR_SLICE_BYTES) == 0);
    CHECK(vkfr_next_cursor(0, 232*M, 0) == 0);
    CHECK(vkfr_next_cursor(232*M, 232*M, VKFR_SLICE_BYTES) == 0);
    CHECK(vkfr_next_cursor(UINT64_MAX-10, UINT64_MAX, 64) == 0);
    CHECK(vkfr_piece_covers(0x100000, 0x200000, 0x400000, 0x180000, 0x190000, 0x480000));
    CHECK(!vkfr_piece_covers(0x180000, 0x200000, 0x480000, 0x100000, 0x110000, 0x400000));
    CHECK(!vkfr_piece_covers(0x100000, 0x200000, 0x500000, 0x180000, 0x190000, 0x480000));
    CHECK(!vkfr_piece_covers(0x100000, 0x200000, 0x400000, 0x180000, 0x210000, 0x480000));
    CHECK(!vkfr_piece_covers(0x100000, 0x200000, UINT64_MAX, 0x180000, 0x190000, 0x7ffff));
    puts("t_vka_fileback_reclaim: ok");
    return 0;
}
