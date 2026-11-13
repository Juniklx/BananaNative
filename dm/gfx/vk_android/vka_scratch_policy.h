// Pure scratch-cache trimming rule, shared by the Android Vulkan allocator and a
// standalone Linux regression test. GPU work must be completed before freeing a block.
#ifndef VKA_SCRATCH_POLICY_H
#define VKA_SCRATCH_POLICY_H
#include <stdint.h>
#define VKSC_STANDARD_BLOCK (16ull << 20)
#define VKSC_DEFAULT_IDLE (64ull << 20)
// Return 1 only for completed blocks. Keep at most the requested idle budget,
// while still discarding nonstandard (>16 MiB) blocks as the original allocator did.
static inline int vksc_trim_block(int complete, uint64_t block, uint64_t idle_kept, uint64_t limit) {
    return complete && (block > VKSC_STANDARD_BLOCK || idle_kept >= limit || block > limit - idle_kept);
}
#endif
