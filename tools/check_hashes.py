from pathlib import Path
import hashlib
import sys


proj_dir_path = Path(__file__).parent.parent

starter_file_hashes = {
    "Makefile": "320e67b439e7efcadf433d5f9992f89b",
    "src/compute.h": "2517f5b78999da553924cc3bdc88d661",
    "src/coordinator.h": "66caa337c52639a200cd70bf881abb6b",
    "src/io.h": "cd438edde87210d259d7425731be6e37",
    "src/io.o": "f7ba065d844a3301ebbf0c4eee963226",
    "tools/check_output.sh": "634302e09eb1c23ff1ab5cc93bcdcf24",
    "tools/create_tests.py": "73ff64750f8679e29b5d6d4d62895d3c",
    "tools/framework.py": "c4b7d96c3275dfefba8b5a848684cc34",
    "tools/run_test.sh": "7ba5df8e0a5198d712a0e6efab6d1634",
    "tools/staff_tests.py": "8e76468e7278fab047c521329a1c1e3e",
}

def check_hash(rel_path_str):
    if rel_path_str not in starter_file_hashes:
        return (True, f"Starter does not have hash for {rel_path_str}")
    path = proj_dir_path / rel_path_str
    with path.open("rb") as f:
        contents = f.read()
    contents = contents.replace(b"\r\n", b"\n")
    hashed_val = hashlib.md5(contents).hexdigest()
    if hashed_val != starter_file_hashes[rel_path_str]:
        return (False, f"{rel_path_str} was changed from starter")
    return (True, f"{rel_path_str} matches starter file")

passed_all = True
for rel_path_str in starter_file_hashes.keys():
    passed, reason = check_hash(rel_path_str)
    if not passed:
        passed_all = False
        print(f"Error: {reason}", file=sys.stderr)
if not passed_all:
    sys.exit(1)
