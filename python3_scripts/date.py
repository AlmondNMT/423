"""
A program to demonstrate how to have dictionaries interact across programs.

File: date.py

Author: anonymous

Date: March, 16

Assignment: Lab 7
"""
def yeartab(month, day, year, leaper):
    """Year tab is going to sum all of the first three parameters, (day, month,
    and year), and sum them all into a collective whole. (The big num). Year tab
    also is where we define the dictionary leaper, which keeps track of whether
    there is a leap year or not. Leaper is defined in another function, but 
    its properties are altered here."""
    year = int(year) 
    day = int(day)
    Feb = 28
    Apr = 30
    thebignum = 0
    if (year % 4 == 0 and year % 100 != 0) or year % 400 == 0: #leap year as if
        Feb = Feb + 1                                         #29 days in Feb
        leaper[year] = "yes"                  
    else:                                 #Leaper only has 2 replies, yes or no.
        leaper[year] = "no"
    if day > 31 or day < 0:  #It was April the 31st and the clocks had struck 13:00
        print("Error, not a valid date.")
    elif month == str("January"):
        thebignum = thebignum + day         
    elif month == str("Febuary"):
        if day > Feb:           #Feb is the only value which is not constant
            print("Error, not a valid date")
        else:
            thebignum = thebignum + 31 + day
    elif month == str("March"):
        thebignum = thebignum + 31 + Feb + day
    elif month == str("April"):
        if day > Apr:                  #No April 31sts. 
            print("Error, not a valid date")
        else:
            thebignum = thebignum + 62 + Feb + day
    elif month == str("May"):           
        thebignum = thebignum + 92 + Feb + day
    elif month == str("June"):
        if day > Apr:
            print("Error, not a valid date")
        else: 
            thebignum = 123 + Feb + day
    elif month == str("July"):
        thebignum = 153 + Feb + day
    elif month == str("August"):
        thebignum = 184 + Feb + day
    elif month == str("September"):
        if day > Apr:
            print("Error, not a valid date")
        else:
            thebignum = 215 + Feb + day
    elif month == str("October"):
        thebignum = 245 + Feb + day
    elif month == str("November"):
        if day > Apr:
            print("Error, not a valid date")
        else:
            thebignum = 276 + Feb + day
    elif month == str("December"):
        thebignum = 306 + Feb + day
    else:                           
        print("Error, try captitazlizing the month, using a commas and a space.")
    return thebignum
def main(stra):  
    """ Main will split an input string ("stra") into a list with three entries.
    This is an effective way of turning one parameter into three. Main runs
    yeartab on the input and returns an integer "flummox". Main also defines 
    leaper, and combines the two into a list so it can pass them as
    a package to the other programs."""
    tab = stra.split(", ")    #Very fickle on what input it can split
    month = tab[0]          #fit into new parameters
    day = tab[1]
    year = tab[2]
    leaper = {}             #Leaper is empty, (for now)
    flummox = yeartab(month, day, year, leaper)
    lol = [flummox, leaper]         #package for delivery
    flummox = int(flummox)
    return lol                    #package delivered  
if __name__ == '__main__':
    """ We need horoscope to run smoothly, despite wanting slightly different 
    values than date.py when date.py is running normally. if__name__='__main__'
    clear defines what we want for the date.py assignment. It asks for an input
    and runs the function. It asks for value zero of the returned items, and 
    prints the final result."""
    thesoph = input("Type, stop, or enter a month, day, and year:")
    value = main(thesoph)
    decibel = value[0]      #value is a list now. We only want value[0] (thebignum)
    print("Day of the year:", decibel)
