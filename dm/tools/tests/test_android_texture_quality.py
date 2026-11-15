"""Zelda texture quality UI: verify JVM mapping of each choice to Vulkan soft texture switches."""
import shutil
import subprocess
import tempfile
import unittest
from pathlib import Path

ROOT = Path(__file__).resolve().parents[3]
SRC = ROOT / 'android/app/src/main/java/dev/nxnative/android/TextureQuality.java'
TEST = ROOT / 'android/app/src/jvmtest/java/dev/nxnative/android/TextureQualityJvmTest.java'


@unittest.skipUnless(shutil.which('javac') and shutil.which('java'), 'no JDK')
class TextureQualityTest(unittest.TestCase):
    def test_env(self):
        with tempfile.TemporaryDirectory() as tmp:
            subprocess.run(['javac', '-Xlint:all', '-Werror', '-encoding', 'UTF-8',
                            '-d', tmp, str(SRC), str(TEST)], check=True, capture_output=True, text=True)
            r = subprocess.run(['java', '-cp', tmp, 'dev.nxnative.android.TextureQualityJvmTest'],
                               check=True, capture_output=True, text=True)
            self.assertEqual(r.stdout.strip(), 'ok')


if __name__ == '__main__':
    unittest.main()
