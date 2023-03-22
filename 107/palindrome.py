"""
File name: palindrome.py

Author: anonymous

Date: March 24

Assignment: Lab 8
"""

def palindrome(strin):
    """palindrome takes an input string and converts it into a list. It then takes the list and 
    checks to see if the first and last values are equal. If they are, it removes them and calls the
    function again. If not, it returns false.
    Once it gets to the last entry (either a final letter, or an empty list), it returns true."""
    if strin == []:
        return True
    elif len(strin) < 2:
        return True
    elif strin[0] == strin[-1]:
        strin.remove(strin[0])
        if strin == []:
            return True
        else:
            strin.remove(strin[-1])
            if strin == []:
                return True
            else:
                palindrome(strin)
    else:
        return False



def main():
    """main asks the user for an input, and then after it gets a return value, it issues
    a silly statement. """
    aci = input("Please enter a phrase:")
    ray = list(aci)
    palindrome(ray)
    if palindrome(ray):
        print("Huzzuh! A Palindrome!")
    else:
        print("No Palidromes? Disappointing.")

main()
