from framework import Test, TestSpec, Task, Matrix, randint


@Test(seed=100)
def test_example(test: TestSpec):
    test.add_task(Task(Matrix.random(100, 99), Matrix.random(10, 9)))


@Test()
def my_custom_test(test: TestSpec):
    for _ in range(100):
        rows_a = randint(15, 25)
        cols_a = randint(15, 25)
        rows_b = rows_a - randint(1, 7)
        cols_b = cols_a - randint(1,7)
        test.add_task(
            Task(Matrix.random(rows_a, cols_a), Matrix.random(rows_b, cols_b))
        )



    # rows_a = 11
    # cols_a = 9
    # rows_b = 2
    # cols_b = 1

    # test.add_task(Task(Matrix.random(rows_a, cols_a, min_value=-2, max_value = 2), Matrix.random(rows_b, cols_b, min_value=0, max_value= 5)))
