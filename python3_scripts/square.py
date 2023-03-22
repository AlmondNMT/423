import math
a = int(input("Please enter a positive integer: "))


def func1(x):
    if (math.sqrt(x) % 2 == 0):
        g = 'true'
    
    else:
        g = 'false'


while (1 <= a):
    func1(a)
    if (g = 'true'):
        print(a)
        a = a-1
    else:
        a = a-1

       