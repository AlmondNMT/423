""" 
A program to demonstrate methods of string/list conversions to overcome challenges in unmutable data types.
This program alters an input number string, adds the values together, and divides the sum.
File: luhns.py

Author: anonymous

Date: March, 9. 

Assignment: Lab 5.
"""


def validate(card):
    """ This function can determine whether a card number is "valid" or not. (The prompt did not ask me to 
    ensure that the input string had the correct length. It will accept any length, even or odd. It is split
    into three main sections. 1) Reorder the list and multiple certain values by 2. 2) add all values in the 
    new list together. 3) Check the validity. The first part rewrites the string in reverse to better multiply
    the values 'starting at the second to last value'. The second function creates a while loop to gradually
    accumulate all the values in the new list, although the first value of the list is kept baseline and is 
    outside of the loop. The third function checks to see if the sum of all of these values is divisible by
    10. 
    It returns "True" or "False" based off of the third function."""
    cardlist = card
    tarotlist = []
    z = 0
    while z < len(cardlist):
        y = -1 * (z + 1)
        if y % 2 == 0:
            s = int(cardlist[y]) * 2
            tarotlist.append(s)
            z = z + 1
        else:
            tarotlist.append(cardlist[y])
            z = z + 1
    i = 1
    total = tarotlist[0] 
    totes = int(total)
    while i >= 0 and i < len(tarotlist):
        if int(tarotlist[i]) >= 10:
            newstr = str(tarotlist[i])
            tarotlist[i] =  int(newstr[0]) + int(newstr[1])
        totes = totes + int(tarotlist[i])
        i = i + 1
    if totes % 10 == 0:
        return True
    else:
        return False
def main():
    """This function asks for user input, and runs the function above. Python knows to search for a 
    true/false response when the function is called in an if statement. """
    carddigan = input("Please give me your credit card number (He he):")
    if validate(carddigan):
        print("Valid credit card detected. Thank you for your naivety")
    else:
        print("Bad card! Please try again.")
main()

