""" 
A program to demonstrate methods of string/list conversions to overcome challenges in unmutable data types.
This program reverses values of any input string. It uses string concatonation to better combine elements 
of string values. 
File: cipher.py

Author: anonymous

Date: March, 9. 

Assignment: Lab 5.
"""

def main():
    """ This function reverses any phrase by generating a new string and adding all the values of the 
    original user input in reverse order. """
    stringcheese = input("Enter a phrase.... any phrase:")
    stringi = list(stringcheese)
    s = ""
    y = 1
    while y <= len(stringi):
        z = stringi[(-y)+(len(stringi))]
        s = s + z
        y = y + 1
    print(s)
main()

