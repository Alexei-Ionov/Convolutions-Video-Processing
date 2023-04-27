import math

from framework import Test, TestSpec, Task, Matrix, randint

@Test(seed=100)
def test_example(test: TestSpec):
    test.add_task(Task(Matrix.random(100, 99), Matrix.random(10, 9)))


@Test()
def my_custom_test(test: TestSpec):
   
    for _ in range(50):
        rows_a = randint(25, 500)
        cols_a = randint(25, 500)
        rows_b = randint(math.floor(rows_a / 4), math.floor(rows_a / 2))
        cols_b = randint(math.floor(cols_a / 4), math.floor(cols_a / 2))
        test.add_task(
            Task(Matrix.random(rows_a, cols_a), Matrix.random(rows_b, cols_b))
        )