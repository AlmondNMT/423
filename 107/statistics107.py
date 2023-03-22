"""
A program to use mathematical functions to give rise to the fury of the elements.

file: statistics 107.py

author: anonymous

date: Feb 28

assignment: Lab 4
"""

def max(elements):
    """This function returns the largest element in the given list."""
    maximum = elements[0]
    for i in range(len(elements)):
        if maximum < elements[i]:
            maximum = elements[i]
    return maximum


def min(elements):
    minimum = elements[0]
    for i in range(len(elements)):
        if minimum > elements[i]:
            minimum = elements[i]
    return minimum



def sum(elements):
    i = 0
    if len(elements) <= 0:
        return 0
    total = elements[i]
    i = i + 1
    while i >= 1 and i < len(elements):
        total = total + elements[i]
        i = i + 1
    return total 

def mean(elements):
    i = 0
    total = elements[i] 
    i = i + 1
    while i >= 1 and i < len(elements):
        total = total + elements[i]
        i = i + 1
    total = sum(elements)
    return sum(elements) / len(elements)


def odds(elements):
    #what are the odds?
    oddities = []
    for i in range(len(elements)):
        if elements[i]%2 == 0:
            i = i + 1
        else:
            oddities.append(elements[i])
            i = i + 1

    return oddities


def evens(elements):
    icanteven = []
    for i in range(len(elements)):
        if elements[i] % 2 == 0:
            icanteven.append(elements[i])
            i = i + 1
        else:
            i = i + 1
    return icanteven


def every_other(elements):
    newlizst = []
    for i in range(len(elements)):
        if i % 2 == 0:
            newlizst.append(elements[i])
            i = i + 1
        else:
            i = i + 1
    return newlizst


def every_other_odd(elements):
    skipperino = []
    j = 1
    for i in range(len(elements)):
        if elements[i]%2 == 0:
            i = i + 1
        else:
            if j % 2 == 0:
                i = i + 1
                j = j + 1
            else:
                skipperino.append(elements[i])
                i = i + 1
                j = j + 1
    return skipperino



def every_other_even(elements):
    latveriacafeteria = []
    j = 1
    for i in range(len(elements)):
        if elements[i]%2 == 0:
            if j % 2 == 0:
                i = i + 1
                j = j + 1
            else:
                latveriacafeteria.append(elements[i])
                i = i + 1
                j = j + 1
        else:
            i = i + 1
            
    return latveriacafeteria


def run_tests():
    """This function tests each function defined in this module."""

    # A single test is a tuple containing the function
    # which is being tested, a sample input, and the correct output.
    # Tests should be written using the unittest or pytest modules,
    # but, hopefully, this code is easier to understand.
    tests = [
        # This test will be used to check if `sum([1, 2, 3])` is `6`.
        (sum, [1, 2, 3], 6),
        (sum, [], 0),
        (sum, [-1, 0, 1], 0),
        (sum, [10000, 1000, 100, 10, 1], 11111),
        (max, [3, 2, 1, 2, 3], 3),
        (max, [-1, 4, -2, 10, 1, 5], 10),
        (min, [3, 2, 1, 2, 3], 1),
        (min, [-1, 4, -2, 10, 1, 5], -2),
        (odds, [1, 2, 3, 4], [1, 3]),
        (odds, [1, 100, 45, 23, 10, 2, 4, 13], [1, 45, 23, 13]),
        (odds, [], []),
        (evens, [1, 2, 3, 4], [2, 4]),
        (evens, [1, 100, 45, 23, 10, 2, 4, 13], [100, 10, 2, 4]),
        (evens, [], []),
        (every_other, [1, 2, 3, 4], [1, 3]),
        (every_other, [1, 100, 45, 23, 10, 2, 4, 13], [1, 45, 10, 4]),
        (every_other, [], []),
        (every_other_odd, [1, 2, 3, 4], [1]),
        (every_other_odd, [1, 100, 45, 23, 10, 2, 4, 13], [1, 23]),
        (every_other_odd, [], []),
        (every_other_even, [1, 2, 3, 4], [2]),
        (every_other_even, [1, 100, 45, 23, 10, 2, 4, 13], [100, 2]),
        (every_other_even, [], []),
    ]

    passed = 0
    failed = 0

    # Test each function.
    # Print a detailed error message whenever a function fails a test.
    for test in tests:
        f = (test[0])
        result = f(test[1])
        expected = test[2]
        if result == expected:
            passed += 1
        else:
            failed += 1

            # f.__name__ is the function's name
            # for example, evens.__name__ is the string "evens"
            print("Function '{}' given argument {}".format(f.__name__, test[1]))
            print("Expected {}, but returned {}".format(expected, result))
    print("\nPassed {} out of {} tests.".format(passed, passed + failed))


if __name__ == "__main__":
    run_tests()
