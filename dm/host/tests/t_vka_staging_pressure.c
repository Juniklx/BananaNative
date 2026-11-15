// Pure-unit tests for pressure-triggered Android staging-budget reduction.
#include "../../gfx/vk_android/vka_staging_pressure.h"
#include <stdio.h>
#define CHECK(x) do { if (!(x)) { fprintf(stderr, "failed at line %d: %s\n", __LINE__, #x); return 1; } } while (0)
int main(void) {
    const uint64_t M = VKSP_MIB;
    const uint64_t total = 7224 * 1024ull;
    const uint64_t reserve = total / 8;
    CHECK(!vksp_pressed(total, 2700*1024ull, -1.0));
    CHECK(!vksp_pressed(total, reserve+1, 0.0));
    CHECK(vksp_pressed(total, reserve, 0.0));
    CHECK(vksp_pressed(total, 709*1024ull, -1.0));
    CHECK(vksp_pressed(total, 2700*1024ull, 5.0));
    CHECK(!vksp_pressed(0, 0, -1.0));
    CHECK(!vksp_pressed(total, 0, 4.9));
    CHECK(vksp_pressed(0, 0, 10.0));
    CHECK(vksp_idle_limit(64*M, 0) == 64*M);
    CHECK(vksp_idle_limit(16*M, 0) == 16*M);
    CHECK(vksp_idle_limit(64*M, 1) == 0);
    CHECK(vksp_idle_limit(0, 0) == 0);
    CHECK(vksp_upload_limit(256.0*M, 0) == 256.0*M);
    CHECK(vksp_upload_limit(256.0*M, 1) == 128.0*M);
    CHECK(vksp_upload_limit(64.0*M, 1) == 64.0*M);
    CHECK(vksp_upload_limit(128.0*M, 1) == 128.0*M);
    puts("t_vka_staging_pressure: ok");
    return 0;
}
