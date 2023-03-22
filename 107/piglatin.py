"""
A program to juggle lines, words, and letters from function to function. But the prize wasn't in the juggling. It was in the files
we made a long the way.

file: piglatin.py

author: anonymous

date: April, 6th

assignment: Lab 10
"""

def porky(snowball):
    """porky is a processing file that can translate any line of text into a piglatin line. It inputs a whole list of lines, and 
    splits those lines into words. It then runs the list of words through redundantfunction() to turn them into piglatin words.
    It creates a new string, baconator, which adds spaces between the returned words and combines those words into lines.
    It returns baconator as an output. """
    p = 0
    baconator = ""
    while p < len(snowball):
        babe = snowball[p]
        razorfen = babe.split(" ")
        quilboar = redundantfunction(razorfen)
        q = 0
        while q < len(quilboar):
            baconator = baconator + quilboar[q] + " "
            q = q + 1
        baconator = baconator + "\n"
        p = p + 1
    return(baconator)

def redundantfunction(oink):
    """redundantfunction takes in an input list of words and translates those words into piglatin. It splits a word into each
    individual letter. It then tests the contents of that letter, checking whether it is a vowel, consonant, punctuation mark, or
    if it is nothing at all. It then procedes accordingly. For vowels, it simply uses string concatonation to add "way" to the word
    For consonants, it removes the first letter and adds it to the end of the word. Then it adds the suffix "ay" to complete the
    translation. It also can keep punctuation marks to help with sentence structure (yay!). It returns a list of fully translated
    words as a output. """
    somepig = []
    i = 0
    serpiggy = oink
    while i < len(serpiggy):
        porker = serpiggy[i]
        if porker == "":
            i = i + 1
        else:
            cured = porker[0]
            if cured == "a" or cured == "e" or cured == "i" or cured == "o" or cured == "u" or cured == "A" or cured == "E" or cured == "I" or cured == "O" or cured == "U":
                porker = porker + "way"
                somepig.append(porker)
                i = i + 1
            else:
                spiderham = list(porker)
                spiderham.remove(spiderham[0])
                blanket = ""
                sty = ""
                g = 0
                while g < len(spiderham):
                    if spiderham[g] == "." or spiderham[g] == "," or spiderham[g] == "?" or spiderham[g] == "!":
                        sty = spiderham[g]
                        g = g + 1
                    else:
                        blanket = blanket + spiderham[g]
                        g = g + 1
                blanket = blanket + cured + "ay" + sty
                somepig.append(blanket)
                i = i + 1
    return somepig
def main():
    """main() takes a user input file and tranfers that file and splits that file into lines of text. It runs those files into porky()
    and then expects a returned full string (all the contents of the file, separated into their proper lines) after they've been
    translated. It takes the new translation and creates a new file "boaring.txt" and writes the translation into that file."""
    napoleon = input("enter a file name:")
    bacon = ""
    with open(napoleon, "r") as sample_message:
        ham = sample_message.readline()
        while ham != "":
            bacon = bacon + ham
            ham = sample_message.readline()
        ziggy = str(bacon)
        ziggy = ziggy.split("\n")
        hogger = porky(ziggy)
        boaring = open("boaring.txt", "w")
        boaring.write(hogger)
        boaring.close
main()
