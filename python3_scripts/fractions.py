""" 
A program to utilize tuple expressions and pass them between multiple functions. 
This program is able to comput basic fraction math by converting multiple string inputs into tuple, and list
expressions useful for singular and multi-parameter functions. 

File: fractions.py

Author: anonymous

Date: March, 9. 

Assignment: Lab 5.
"""
import math
def  reduce(fra1):
    """ This function takes a fraction string, and reduces it to the simplest value.
    The simplest value is a fraction expression equal in value to to the original fraction, 
    but has lower numbers. 
    This function uses the .gcd function to see if there are any common multiples between the
    numerator and the denomonator that it can divide by. If not, it will return the original 
    value."""
    num = fra1.split("/")
    gilbert = math.gcd(int(num[0]), int(num[1]))
    reduction1 = (int(num[0])/gilbert)
    reduction2 = (int(num[1])/gilbert)
    finalred = [int(reduction1), '/', int(reduction2)]
    seeingred = ""
    y = 0
    while y < len(finalred):
        seeingred = seeingred + str(finalred[y])
        y= y + 1
    return seeingred

def add(harry, marv):
    """This function adds two fraction results into a new single fraction.
    It multiplies them so the share a denominator, adds them, and returns 
    the smallest expression after running reduce(). """
    tom = harry.split("/")
    jerry = marv.split("/")
    z = (int(tom[1]) * int(jerry[1]))
    l = (int(tom[0])*int(jerry[1])) + (int(tom[1])*int(jerry[0]))
    first_add = [l, '/', z]
    q = ""
    i = 0
    while i < len(first_add):
        q = q + str(first_add[i])
        i = i + 1
    final_add = reduce(q)
    return final_add

def multiply(dumb, dumber):
    """ This function can take two fraction strings, and multiply them by multiplying the numerators and 
    denominators. It returns the simplest expression by using reduce(). """
    garfield = dumb.split("/")
    odie = dumber.split("/")
    m = (int(garfield[0])*int(odie[0]))
    j = (int(garfield[1])*int(odie[1]))
    mult_first = [m, '/', j]
    mult_mid = ""
    h = 0
    while h < len(mult_first):
        mult_mid = mult_mid + str(mult_first[h])
        h = h + 1
    mult_last = reduce(mult_mid)
    return mult_last

def divide(wallace, gromit):
    """This function can divide two fraction strings. It is identical to multiply(), but flips one
    of the fractions since (2/3)/(3/4) = (2/3) * (4/3). It also finds the simplest expression using 
    reduce()."""
    calvin = wallace.split("/")
    hobbes = gromit.split("/")
    s = (int(calvin[0])*int(hobbes[1]))
    w = (int(calvin[1])*int(hobbes[0]))
    div_first = [s, '/', w]
    div_mid = ""
    r = 0
    while r < len(div_first):
        div_mid = div_mid + str(div_first[r])
        r = r + 1
    div_last = reduce(div_mid)
    return div_last

def main():
    """ This is the main function. It asks for user input, runs all the functions, and prints the returned 
    values of those functions. """ 
    zzz = input("Enter a fraction:")
    aaa = input("Enter a fraction:")
    print("Reduced fractions to", reduce(zzz), "and", reduce(aaa))
    print("Sum of the fractions:", add(zzz, aaa))
    print("Multiplications of the fractions:", multiply(zzz, aaa))
    print("Division of the fractions:", divide(zzz, aaa))
if __name__ == "__main__":
    main()
