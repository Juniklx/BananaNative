package dev.nxnative.android;

import java.util.List;

/** Pure JVM tests for the launcher texture quality switch. */
public final class TextureQualityJvmTest {
    private static void check(boolean ok, String what) { if (!ok) throw new AssertionError(what); }

    public static void main(String[] args) {
        String oot = "0400486024DEC000";
        check(TextureQuality.supports(oot), "Zelda enabled");
        check(!TextureQuality.supports("0100000000001000") && !TextureQuality.supports(null), "only Zelda");
        check(TextureQuality.NAMES.length == 4, "four choices");
        check(TextureQuality.clamp(-1) == 0 && TextureQuality.clamp(4) == 0, "invalid persisted level");
        check(TextureQuality.env(oot, 0).equals(List.of("DM_VK_TEX_SOFT=0")), "original disables reduction");
        for (int n = 1; n <= 3; n++)
            check(TextureQuality.env(oot, n).equals(List.of("DM_VK_TEX_SOFT=1", "DM_VK_TEX_SOFT_LEVELS=" + n)), "levels " + n);
        check(TextureQuality.env(oot, 9).equals(List.of("DM_VK_TEX_SOFT=0")), "invalid -> original");
        check(TextureQuality.env("0100000000001000", 2).isEmpty(), "other games untouched");
        System.out.println("ok");
    }
}
