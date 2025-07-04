#!/usr/bin/env python3
"""Generate ESP settings.h and settings.c using revk_settings."""
import subprocess
from pathlib import Path
import sys

ROOT = Path(__file__).resolve().parent
COMPONENT = ROOT / "ESP" / "components" / "ESP32-RevK"
REVK_SETTINGS = COMPONENT / "revk_settings"


def run(cmd, **kwargs):
    print("Running:", " ".join(cmd))
    subprocess.check_call(cmd, **kwargs)


def build_tool():
    if REVK_SETTINGS.exists():
        return
    # compile the helper tool
    include_dir = ROOT / "ESP" / "include"
    cmd = [
        "gcc",
        "-O",
        "-o", str(REVK_SETTINGS),
        str(COMPONENT / "revk_settings.c"),
        "-g",
        "-Wall",
        "--std=gnu99",
        f"-I{include_dir}",
        "-lpopt",
    ]
    try:
        run(cmd, cwd=str(COMPONENT))
    except subprocess.CalledProcessError:
        # Fall back to bundled static library if system libpopt is unavailable
        popt_lib = ROOT / "ESP" / "libpopt.a"
        cmd[-1] = str(popt_lib)
        run(cmd, cwd=str(COMPONENT))


def main():
    # ensure submodules present
    sub = ROOT / "setup_submodules.py"
    if sub.exists():
        run(["python", str(sub)])

    build_tool()

    def_files = [COMPONENT / "settings.def", ROOT / "ESP" / "main" / "settings.def"]
    hfile = ROOT / "ESP" / "main" / "settings.h"
    cfile = ROOT / "ESP" / "main" / "settings.c"

    cmd = [str(REVK_SETTINGS)] + [str(f) for f in def_files] + ["-h", str(hfile), "-c", str(cfile)]
    run(cmd)
    print("Generated", hfile, "and", cfile)


if __name__ == "__main__":
    try:
        main()
    except subprocess.CalledProcessError as exc:
        sys.exit(exc.returncode)
