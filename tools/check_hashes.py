from pathlib import Path
import hashlib
import sys


proj_dir_path = Path(__file__).parent.parent

starter_file_hashes = {
    "Makefile": "e0e21a247bab8cfd16accbbf0511cb27",
    "src/compute.h": "2517f5b78999da553924cc3bdc88d661",
    "src/coordinator.h": "66caa337c52639a200cd70bf881abb6b",
    "src/io.h": "cd438edde87210d259d7425731be6e37",
    "src/io.o": "f7ba065d844a3301ebbf0c4eee963226",
    "tools/check_output.sh": "43080162b7a58662375962846b2b0967",
    "tools/create_tests.py": "5823d2ae6060755b474d6a45932d72eb",
    "tools/framework.py": "b3c627db754b9ccb09d8f6e676afa38f",
    "tools/run_test.sh": "34b93fe836972a6306317f119dd5ecf4",
    "tools/staff_tests.py": "d32cf284f1303d9d4c80b7551afcc067",
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
