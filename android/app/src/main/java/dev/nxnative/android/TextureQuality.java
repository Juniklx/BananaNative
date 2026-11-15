package dev.nxnative.android;

import java.util.List;

/**
 * Zelda-only opt-in Vulkan soft textures. These are NOT a global texture resize:
 * the host considers only large packaged block-compressed 2D textures with mipmaps.
 */
final class TextureQuality {
    private TextureQuality() {}
    private static final String ZELDA = "0400486024DEC000";
    static final String[] NAMES = {
            "Original (full texture resolution)",
            "Reduced (omit the finest mip level)",
            "Low (omit up to 2 finest mip levels)",
            "Very low (omit up to 3 finest mip levels)"
    };

    static boolean supports(String titleId) { return ZELDA.equals(titleId); }
    static int clamp(int n) { return n >= 0 && n < NAMES.length ? n : 0; }

    /** Generated before user env.txt and per-game environment lines, which take precedence. */
    static List<String> env(String titleId, int level) {
        if (!supports(titleId)) return List.of();
        int n = clamp(level);
        return n == 0 ? List.of("DM_VK_TEX_SOFT=0")
                : List.of("DM_VK_TEX_SOFT=1", "DM_VK_TEX_SOFT_LEVELS=" + n);
    }
}
