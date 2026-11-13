// Standalone test for the Android Vulkan scratch-block cache policy.
// Never release work whose submission/upload timeline has not completed.
#include "../../gfx/vk_android/vka_scratch_policy.h"
#include <stdio.h>
#define CHECK(x) do { if (!(x)) { fprintf(stderr, "failed at line %d: %s\n", __LINE__, #x); return 1; } } while (0)
int main(void) {
    const uint64_t M = 1ull << 20;
    // Previous 64 MiB default: keep up to four 16 MiB idle blocks.
    CHECK(!vksc_trim_block(1, 16*M, 0, 64*M));
    CHECK(!vksc_trim_block(1, 16*M, 48*M, 64*M));
    CHECK(vksc_trim_block(1, 16*M, 64*M, 64*M));
    // A smaller cache is opt-in; never trim a GPU-in-flight block, even at zero budget.
    CHECK(!vksc_trim_block(0, 16*M, 0, 0));
    CHECK(!vksc_trim_block(0, 128*M, 0, 0));
    CHECK(vksc_trim_block(1, 16*M, 0, 0));
    CHECK(!vksc_trim_block(1, 16*M, 0, 16*M));
    CHECK(vksc_trim_block(1, 16*M, 16*M, 16*M));
    CHECK(vksc_trim_block(1, 16*M, 16*M, 24*M));
    CHECK(!vksc_trim_block(1, 16*M, 0, 24*M));
    // Large temporary allocations were already discarded when idle.
    CHECK(vksc_trim_block(1, 32*M, 0, 64*M));
    // Use the subtraction path: large unsigned counters must not overflow.
    CHECK(vksc_trim_block(1, 16*M, UINT64_MAX-8, 16*M));
    puts("t_vka_scratch_policy: ok");
    return 0;
}
