import math

from framework import Test, TestSpec, Task, Matrix, randint

@Test(seed=100)
def test_example(test: TestSpec):
    test.add_task(Task(Matrix.random(100, 99), Matrix.random(10, 9)))


@Test(seed=61)
def custom_tests(test: TestSpec):
    test.add_task(Task(Matrix.random(1000, 1000), Matrix.random(10, 10)))
    for _ in range(500):
        rows_a = randint(50, 100)
        cols_a = randint(50, 100)
        rows_b = randint(25, 50)
        cols_b = randint(25, 50)
        test.add_task(
            Task(Matrix.random(rows_a, cols_a), Matrix.random(rows_b, cols_b))
        )