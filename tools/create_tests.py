import sys

from framework import all_tests
import custom_tests, staff_tests

# Do not touch!
if __name__ == "__main__":
    tests = all_tests.keys()
    if len(sys.argv) > 1:
        tests = [t for t in all_tests if t == sys.argv[1]]
    for test in tests[:2]:
        print(f"Generating {test}")
        all_tests[test].generate()
