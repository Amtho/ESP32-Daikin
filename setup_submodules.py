#!/usr/bin/env python3
"""Utility to initialise and update all git submodules."""
import subprocess
import os
import sys

def main() -> None:
    repo_root = os.path.dirname(os.path.abspath(__file__))
    os.chdir(repo_root)
    cmd = ["git", "submodule", "update", "--init", "--recursive"]
    print("Running:", " ".join(cmd))
    try:
        subprocess.check_call(cmd)
    except subprocess.CalledProcessError as exc:
        print(f"Failed: {exc}", file=sys.stderr)
        sys.exit(exc.returncode)

if __name__ == "__main__":
    main()
