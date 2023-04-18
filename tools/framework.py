from pathlib import Path
from typing import List, Dict
import hashlib
import json
import numpy as np
import os
import random
import struct
import subprocess

project_dir = Path(__file__).parent.resolve().parent
tests_dir = project_dir / "tests"
oracle_path = Path("/home/ff/cs61c/sp23/proj4/convolve_oracle")

all_tests: Dict[str, "TestSpec"] = {}


def run_oracle(a_path: Path, b_path: Path, out_path: Path):
    if not oracle_path.exists():
        raise RuntimeError("Oracle does not exist, please run on the hive machines")
    subprocess.run([oracle_path, a_path, b_path, out_path])


def randint(lower, upper, **kwargs):
    return np.random.randint(lower, upper + 1, **kwargs)


def md5sum(path: Path) -> str:
    with path.open("rb") as f:
        contents = f.read()
    contents = contents.replace(b"\r\n", b"\n")
    return hashlib.md5(contents).hexdigest()


class Matrix:
    def random(rows: int, cols: int, min_value=-1000, max_value=1000) -> "Matrix":
        values = list(randint(min_value, max_value, size=rows * cols) & 0xFFFFFFFF)
        return Matrix(rows, cols, values)

    def __init__(self, rows: int, cols: int, data: List[int]):
        self.rows = rows
        self.cols = cols
        self.data = data

    def generate(self, path: Path):
        with path.open("wb") as f:
            # Write row and column counts
            f.write(struct.pack("i", self.rows))
            f.write(struct.pack("i", self.cols))

            # Write matrix elements as bytes
            f.write(struct.pack("I" * self.rows * self.cols, *self.data))


class Task:
    def __init__(self, a_matrix: Matrix, b_matrix: Matrix):
        self.a_matrix = a_matrix
        self.b_matrix = b_matrix

    def generate(self, path: Path):
        path.mkdir(exist_ok=True)
        self.a_matrix.generate(path / "a.bin")
        self.b_matrix.generate(path / "b.bin")
        a_md5 = md5sum(path / "a.bin")
        b_md5 = md5sum(path / "b.bin")

        try:
            with (path / ".hashes.json").open("r") as f:
                hashes_json = json.load(f)
                if hashes_json["a.bin"] == a_md5 and hashes_json["b.bin"] == b_md5:
                    return
        except:
            pass

        with (path / ".hashes.json").open("w") as f:
            f.write(json.dumps({"a.bin": a_md5, "b.bin": b_md5}))
        run_oracle(path / "a.bin", path / "b.bin", path / "ref.bin")


class TestSpec:
    def __init__(self, name: str):
        self.path = tests_dir / name
        self.tasks: List[Task] = []

    def add_task(self, task: Task):
        self.tasks.append(task)

    def generate(self):
        self.path.mkdir(exist_ok=True, parents=True)
        with (self.path / "input.txt").open("w") as f:
            f.write(str(len(self.tasks)) + "\n")
            for i, task in enumerate(self.tasks):
                task_path = self.path / f"task{i}"
                task.generate(task_path)
                rel_task_dir = os.path.relpath(task_path, self.path)
                f.write(f"./{rel_task_dir}\n")


def Test(name: str = None, seed: int = 42):
    def decorator(func):
        nonlocal name
        name = name or str(func.__name__)
        random.seed(seed)
        np.random.seed(seed)

        test = TestSpec(name)
        func(test)
        all_tests[name] = test

    return decorator
