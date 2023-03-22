"""
A program to demonstrate while loop

file: fizzbuzz.py

author: anonymous

date: Feb 16

assignment: Lab3
"""

import math 
def main():
    x = input("Please enter a positive integer: ")
    x = int(x)
    if x < 0:
        print("Not a positive number!")
        quit()
    y= 0
    while y <= x:
        if y % 3 == 0 and y % 5 == 0:
            print("{} FizzBuzz".format(y))
            y = y + 1
        elif y % 3 == 0:
            print("{} Fizz".format(y))
            y = y + 1
        elif y % 5 == 0:
            print("{} Buzz".format(y))
            y = y + 1
        else:
            print("{}".format(y))
            y = y + 1

if __name__ == '__main__':
    main()
