import math
a = int(input("Please enter a positive integer: "))
def func1(x):
    if (x%2) == 0:
        x=1
    else:
        x=0
print (a)
while (1 <= a):
    if (func1(a) == 1):
        print(func1(a))
        a = a - 1
    else:
         a = a - 1
        
