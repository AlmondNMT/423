"""
file: word_count.py

author: anonymous

date: April, 4th

assignment: Lab 9
"""


def wordmatcher(chord, fret, b, c, k):
    """word count takes in two lists of words and three manipulative variables as input parameters. 
    fret is our user input, and chord is every character in a file, split into a list. A while 
    loop on value i is run to see if the characters in one phrase are identical to another. If they match,
    the j value is increased. If the whole phrase matches, a print statement is shown, describing where the 
    match is. B keeps track of the character position in the line. Once the loop is finished, the fuction
    uses recursion to call itself starting on the second character and moving the character up.
    C keeps track of the line. Every time a \ n character is encountered, b is reset to zero, and c is moved to
    1. k is the negative result tracker. If a positive match is found, k is moved up to the positives every 
    time the function is recalled. If no matches are found, a print statement will be issued stating that
    the searched string is not found in the file. 
    """
    i = 0
    j = 0
    while i < len(chord):
        if fret[i] == chord[i]:
            i = i + 1
            j = j + 1
            if j == len(fret):              #every character matches.
                print("There it is, the phrase ends at line", c, "character", b)
                k = k + 1
                i = len(chord)
        else:
            if chord[i] == "\n":                #new line moves c up and resets b
                b = 0
                c = c + 1
            i = len(chord)
    if chord == [] and k == 0:                  #negative answer.
        print("This file does not have the phrase you are looking for.")
    elif chord == []:                           #positive answer. end of while loop. 
        return
    else:
        chord.remove(chord[0])
        b = b + 1
        wordmatcher(chord, fret, b, c, k)       #recursive function. 

def main():
    """main can take a file name searched for by the user (it needs .txt) and searches for that file under the 
    directory. If it finds it, it takes the file out and turns it and an additional user input and runs them
    through wordmatcher with a few additional tracking parameters."""
    wind = input("Enter a string:")
    answer = input("open a file to search:")
    dylan = ""
    with open(answer, "r") as sample_message:    
        bob = sample_message.readline()
        while bob !=  "":
            dylan = dylan + bob       #message from file 1 transfered.
            bob = sample_message.readline()
    marv = list(dylan)
    pappy = list(wind)
    wordmatcher(marv, pappy, 0, 1, 0)



main()
