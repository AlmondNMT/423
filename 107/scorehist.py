"""
File name: scorehist.py

Author: anonymous

Date: April 15th

Assignment: Lab 11
"""

import matplotlib.pyplot as plt

def theonering(faramir):
    """Theoneright takes a list of data, and turns it into a neat little chart."""
    plt.hist(faramir)
    plt.title("Histogram of ACT scores")
    plt.ylabel("Score of ACT")
    plt.show()


def main():
    """main() reads information out of actsat.txt and takes a specific column of text (the column 
    indicating the average ACT score for the state). It takes that infromation round the data to 
    the nearest whole number, and runs it through the one ring."""
    frodo = ""
    saruman = []
    with open("actsat.txt", "r") as sample_message:
        gandalf = sample_message.readline()
        while gandalf != "":
            
            gandalf = sample_message.readline()
            x = gandalf.split()
            if x == []:                     #weird oddity with the split() func.
                gimli = 43
            else:
                saruman.append(x[2])           #column 2 is the right column.
    l = 0
    rhadagast = []
    while l < len(saruman):
        denethor = int(round(float(saruman[l]), 1))     #round to whole numbers. 
        rhadagast.append(denethor)
        l = l + 1
    #boromir = {}
    theonering(rhadagast)
    
main()
