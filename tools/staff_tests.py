from framework import Test, TestSpec, Task, Matrix, randint


# This file will be updated with more tests when the autograder is released
@Test()
def test_1d_small(test: TestSpec):
    for _ in range(100):
        cols_a = randint(15, 25)
        cols_b = randint(5, 15)
        test.add_task(Task(Matrix.random(1, cols_a), Matrix.random(1, cols_b)))


@Test()
def test_1d_large(test: TestSpec):
    for _ in range(1000):
        cols_a = randint(100, 200)
        cols_b = randint(5, 100)
        test.add_task(Task(Matrix.random(1, cols_a), Matrix.random(1, cols_b)))


@Test()
def test_2d_small(test: TestSpec):
    for _ in range(100):
        rows_a = randint(15, 25)
        cols_a = randint(15, 25)
        rows_b = randint(5, 15)
        cols_b = randint(5, 15)
        test.add_task(
            Task(Matrix.random(rows_a, cols_a), Matrix.random(rows_b, cols_b))
        )


@Test()
def test_2d_large(test: TestSpec):
    for _ in range(1000):
        rows_a = randint(100, 200)
        cols_a = randint(100, 200)
        rows_b = randint(5, 100)
        cols_b = randint(5, 100)
        test.add_task(
            Task(Matrix.random(rows_a, cols_a), Matrix.random(rows_b, cols_b))
        )
