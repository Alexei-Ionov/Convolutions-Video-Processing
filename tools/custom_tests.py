from framework import Test, TestSpec, Task, Matrix


@Test(seed=100)
def test_example(test: TestSpec):
    test.add_task(Task(Matrix.random(100, 99), Matrix.random(10, 9)))


@Test()
def my_custom_test(test: TestSpec):
    rows_a = 5
    cols_a = 5
    rows_b = 3
    cols_b = 3
    test.add_task(Task(Matrix.random(rows_a, cols_a), Matrix.random(rows_b, cols_b)))
