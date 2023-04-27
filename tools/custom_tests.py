import math

from framework import Test, TestSpec, Task, Matrix, randint

@Test(seed=100)
def test_example(test: TestSpec):
    test.add_task(Task(Matrix.random(100, 99), Matrix.random(10, 9)))


@Test(seed=61)
def my_custom_test(test: TestSpec):
   
     for i in range(2, 29):
        rows_a = randint(math.floor(1.25**i), math.floor(1.25 ** (i + 1)))
        cols_a = randint(math.floor(1.25**i), math.floor(1.25 ** (i + 1)))
        rows_b = randint(math.floor(rows_a / 4), math.floor(rows_a / 2))
        cols_b = randint(math.floor(cols_a / 4), math.floor(cols_a / 2))
        test.add_task(
            Task(Matrix.random(rows_a, cols_a), Matrix.random(rows_b, cols_b))
        )