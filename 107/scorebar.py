"""
File name: scorebar.py

Author: anonymous

Date: April, 15th

Assignment: Lab 11
"""

import matplotlib.pyplot as plt

def beammeup(picard, data, wesley):
    """beammeup() takes and input list of states (picard) and a list of ACT (data) and SAT (wesley)
    scores. It prints out two charts, one for SAT, and one for ACT scores. Klingons defines the 
    range of the ACT scores, while Vulcans define the range of the SAT scores. The graphs are plotted
    vertically, so ACT score are in the top row, and SAT are on the bottom. """
    klingons = [x for x in range(0, 39, 3)]
    vulcans = [x for x in range(0, 2600, 200)]
    redshirts = data
    savestheday = wesley
    plt.subplot(2, 1, 1)                        #chart position
    plt.bar(picard, redshirts, width = .8, bottom = 0, color = 'orange',edgecolor='black')
    plt.title("ACT score average")          
    plt.yticks(fontsize=4)
    plt.xticks(fontsize=5)
    plt.xlabel("state")                     #axis labels
    plt.ylabel("ACT score")
    plt.subplots_adjust(hspace=.5)              #space adjustments
    plt.subplot(2, 1, 2)
    plt.bar(picard, savestheday, width= .8, bottom = 0, color = 'purple',edgecolor='black')
    plt.title("SAT score average")
    plt.yticks(vulcans)
    plt.xticks(fontsize=5)
    plt.xlabel("state")
    plt.ylabel("SAT score")
    plt.show()

def main():
    """ main() reads files from actsat.txt and turns them into output lists of the various columns. 
    It can read each line of text and determine the column by reading each line and determining 
    each elements order in the line by splitting each line into a list. It creates a seperate set
    of lists only containing high SAT low ACT states. It asks the user (rhetorically) to choose 
    which set will be used and runs the list sets (State name, average ACT score, average SAT score)
    through beammeup()."""
    borg = ""
    spock = []
    kirk = []
    uhura = []
    sulu = []
    chekov = []
    scotty = []
    with open("actsat.txt", "r") as sample_message:
        mccoy = sample_message.readline()
        while mccoy != "":
            
            mccoy = sample_message.readline()
            x = mccoy.split()
            print(mccoy)
            if x == []:
                ricky = 43
            else:
                scotty.append(x[0])
                spock.append(float(x[2]))
                sulu.append(int(x[4]) + int(x[5]) + int(x[6]))      #average SAT
                if float(x[1]) < 50 and float(x[3]) > 50:           #bonus list. 
                    kirk.append(x[0])
                    uhura.append(float(x[2]))
                    chekov.append(int(x[4]) + int(x[5]) + int(x[6]))
    kahn = input("True or false, the SAT is important in my life.")     #Graph #2 = True
    if kahn == "True":                  #note the caps.
        beammeup(kirk, uhura, chekov)
    else:
        beammeup(scotty, spock, sulu)
main()
