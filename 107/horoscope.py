"""A program bestowed with awesome power. It can read the future. It can read your mind.

File: horoscope.py

Author: anonymous

Date: March, 16

Assignment: Lab 7"""
import date
def Theprophecy(all):
    """The prophecy takes an input number and gives you your prophecy. :)"""
    if 1 <= all < 20 or 356 <= all <= 366:
        print("Capricorn: Your heart will explode.")
    elif 20 <= all <= 49:
        print("Aquarius: Your friends will abandon you.")
    elif 50 <= all <= 79:
        print("Pisces: You are already dead.")
    elif 80 <= all <= 109:
        print("Aries: They are coming for you.")
    elif 110 <= all <= 139:
        print("Taurus: It was your fault.")
    elif 140 <= all <= 171:
        print("Gemini: Let the hunger take you. Feed on their blood.")
    elif 172 <= all <= 203:
        print("Cancer: It is standing right behind you. Do not move. Do not breathe.")
    elif 204 <= all <= 234:
        print("Leo: All should bow before you. Make them!")
    elif 235 <= all <= 264:
        print("Virgo: There is no escape. Not in this life, not in the next.")
    elif 266 <= all <= 295:
        print("Libra: Have you had the dream again? A black goat with seven eyes that watches from outside")
    elif 296 <= all <= 325:
        print("Scorpio: Your crimes are terrible, numberless, glorious.")
    elif 326 <= all <= 355:
        print("Saggitarius: Ph'nglui mglw'nafh R'lyeh flg'htn")
def main():
    """Main here is going to ask the user for an input. It will run that input
    through date.py. It then has two values, thebignum, and leaper. Now it has
    to sort out the tricky business of the horoscope. March 20th should return 
    Pisces no matter what. Main splits the returned dictionary into a lists
    and searches for the answer. If it is a leap year, it will readjust to 
    fix the erroneous value. Finally it runs thebignum into the prophecy."""
    peric = input("Enter the date: Know your destiny!")
    val = date.main(peric)
    d = val[1]                      #value1 is the dictionary leaper{year, "y/n"}
    mykey = list(d.keys())
    myvalue = d[mykey[0]]           #Ironically, the input year is one of the "keys"
    #and the 0 value is the "yes/no" reply. 
    if myvalue == "yes":
        if val[0] > 60:         #No error adjustment for Jan 1 - Feb 29.
            barbar = val[0] - 1         #The days of the year - Feb 29.
        else:
            barbar = val[0]         #val[0] is return input "thebignum"
    else:
        barbar = val[0]
    print(Theprophecy(barbar))
main()
