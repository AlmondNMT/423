"""
A file to learn more about multi-answer dictionaries, and practice randomization and chance function.

file: markov.py

author: anonymous

date: April, 6th

assignment: lab 10
"""


import random 
def markovsequence(gorbachov, kasparov):
    """markovsequence() takes an input dictionary, and a list. The list is all of the first words of the lines of a file. The function
    defines natasha as an input string, and uses contatonation to define the first two values of the string. Then it runs a while loop
    to find the next eight values of the string. The dictionary guides the function by defining what each new value will be from its
    predecessor. Every time a word is added, the z value is increased until it either hits an end sentence, or where it reaches 100
    total words. The output should be the final accumulation of words in the string natasha. """
    badinov = random.choice(kasparov)
    z = 2
    natasha = ""
    natasha = badinov
    karpov = gorbachov[badinov]
    natasha = natasha + " " + karpov[0]
    karpov = gorbachov[karpov[0]]
    natasha = natasha + " " + karpov[0]
    while z  <= 100:
        if gorbachov[karpov[0]] == []:              #end of line word ends the program. 
            z = 101
        elif len(gorbachov[karpov[0]]) == 1:
            karpov = gorbachov[karpov[0]]
            natasha = natasha + " " + karpov[0]
        else:
            b = random.choice(gorbachov[karpov[0]])             #random item from the list.
            karpov = [b]
            natasha = natasha + " " + karpov[0]
        z = z + 1 
    print(natasha)


def main():
    """main takes an input file name and reads the contents of that file as seperate lines. It tranfers those lines and keeps track
    of the first words of each line as a list (raskolnikov), and then it splits the lines into words, and then defines dictionary 
    keys as word entries from the list. It defines the dictionary answers as the word after the dictionary key itself. It is able to
    search and see if any word has occured in the dictionary twice. If so, it defines the answer as a list and adds any additional 
    occurences of the word to the list. It outputs the first words of each line (raskolnikov) and the completed dictionary 
    (trotsky). It runs these outputs into markovsequence() to randomize the word structure of any given text file. """
    dostoevsky = input("enter a file name")
    tolstoy = []
    raskolnikov = []
    trotsky = {}
    stalin = ""
    with open(dostoevsky, "r", encoding='UTF-8') as sample_message:
        lenin = sample_message.readline()
        while lenin != "":
            stalin = lenin
            lenin = sample_message.readline()
            krushchev = str(stalin)
            krushchev = krushchev.split()
            tolstoy.append(krushchev)
    czar = 0
    while czar < len(tolstoy):                                  #lines loop. Go through every line
        rus = tolstoy[czar]
        vlad = 0
        if rus == "" or rus == []:
            chernobyl = 12000000                        #nuclear meltdown
        elif vlad == 0:
            raskolnikov.append(rus[0])
        while vlad < len(rus):                      #words loop. Go through every word
            if vlad == len(rus) - 1:
                trotsky[rus[vlad]] = []
            else:
                if rus[vlad] in trotsky:                #repeat entry
                    peter = trotsky[rus[vlad]]          #defined as a list
                    peter.append(rus[vlad + 1])
                else:
                    trotsky[rus[vlad]] = [rus[vlad + 1]]
            vlad = vlad + 1
        czar = czar + 1
    markovsequence(trotsky, raskolnikov)
            
main()
