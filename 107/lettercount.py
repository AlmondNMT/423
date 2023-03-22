"""
File: lettercount.py

Author: anonymous

Date: March, 16

Assignment: Lab 7
"""

def lettercount(strink):
    """Lettercount takes an input string and sorts all of the entires into a 
    dictionary. Lettercount checks to see if a value in the string is 
    already in the dictionary. If not, it makes a new entry starting at 1.
    If it is, it raises that value above one. It then prints the keys in order
    (The for loop with i serves to count them), and returns the number value
    associate with each key/entry."""
    ency = {}
    h = 0
    while h < len(strink):
        if strink[h] in ency:               #If the letter is a key.
            ency[strink[h]] = ency[strink[h]] + 1 #The corresponding value goes up 1.
        else: 
            ency[strink[h]] = 1             #New key starting at 1
        
        h = h + 1
    for i in sorted (ency.keys()):          #Sorted alphabetically arranges these.
        print(i, ency[i])                   #the i value being the first key
                                            #ency[i] is the number of iterations.
def main():
    """Main asks for an input and runs letter count with that input."""
    sillystringhasstrung = input("Say something:")
    lettercount(sillystringhasstrung)
main()
