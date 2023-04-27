from framework import Test, TestSpec, Task, Matrix


@Test(seed=100)
def test_example(test: TestSpec):
    test.add_task(Task(Matrix.random(100, 99), Matrix.random(10, 9)))


@Test()
def my_custom_test(test: TestSpec):
    rows_a = 50
    cols_a = 50
    rows_b = 1
    cols_b = 12

    test.add_task(Task(Matrix.random(rows_a, cols_a, min_value=-2, max_value = 2), Matrix.random(rows_b, cols_b, min_value=0, max_value= 5)))
