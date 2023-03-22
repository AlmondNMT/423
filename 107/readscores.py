"""
file: readscores.py

author: anonymous

date: April, 4th, 2021

assignment: Lab 9
"""

def readscore(wanda):
    """readscore takes an input list, and turns the elements in the list into a dictionary. It is able to get rid 
    of excess characters in the list before turning it into the dictionary. After the dictionary keys for a state
    is completed, it removes the items in the list and adds the dictionary entry to the dictionary. It returns a list
    of each dictionar entry."""
    x = 0
    chester = []
    while len(wanda) >= 8:
        if wanda[0] == "":
            del wanda[0]
        if wanda[0][0] == "\n":
            wanda[0] = wanda[0][1:]
        timmy = {"state": wanda[0], "act_percent_taking": wanda[1], "act_average_score": wanda[2], "sat_percent_taking": wanda[3], "sat_average_math": wanda[4], "sat_average_reading": wanda[5], "sat_average_writing": wanda[6]}
        chester.append(timmy)
        del wanda[0]
        del wanda[0]
        del wanda[0]
        del wanda[0]
        del wanda[0]
        del wanda[0]
        del wanda[0]
        del wanda[0]
    return chester     

def main():
    """ main is able to read a given io text file and transfer the elements of that file into a list. It then runs
    the list through readscore and prints the returned result."""
    profbot = ""
    with open("actsat.txt", "r") as sample_message:
        cosmo = sample_message.readline()
        while cosmo != "":
            profbot = profbot + cosmo
            cosmo = sample_message.readline()
    jon = str(profbot)
    jon = jon.split("\t")
    print(readscore(jon))
main()
