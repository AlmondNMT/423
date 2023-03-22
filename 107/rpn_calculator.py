""" 
File name: rpn_calculator

Author: anonymous

Date: March, 17th

Assignment: Lab 7th
"""
def stepsintopoland(battleontheice):
    """stepsintopoland takes in an entry string. It then turns that string into
    a list, seperated whenever the user puts in a space. (Krakow).
    It then appends digits into a new list, dombrowsky. If it encounters an
    operator symbol, it will combine the two previous numbers in the list
    into a new singular value (Chopin), which is the sum, difference, product,
    or remainder, depending on the values used. It then removes the two entries
    from the list, and replaces them with the value. By the end, only one 
    value will be left standing, which is the final total. This value is returned.
    """
    Krakow = battleontheice
    Krakow = Krakow.split()     #String is split at spaces. All entries not given
    #enough 'space' (Hah!), are combined into a new singular entry.
    p = 0
    dombrowsky = []
    while p < len(Krakow):
        if Krakow[p].isdigit():    #appends the digits to the list. 
            dombrowsky.append(Krakow[p])
        elif Krakow[p] == str("+"):
            if len(dombrowsky) < 2:
                print("Error: Too many operators.")
                return []
            Chopin = float(dombrowsky[-2]) + float(dombrowsky[-1])
            dombrowsky.pop()            #removes dombrowsky[-1]
            dombrowsky.pop()            #removes dombrosky[-2]
            dombrowsky.append(Chopin)   #Chopin replaces dombrowsky[-1] and [-2]
        elif Krakow[p] == str("-"):
            if len(dombrowsky) < 2:
                print("Error: Too many operators.")
                return []
            Chopin = float(dombrowsky[-2]) - float(dombrowsky[-1])
            dombrowsky.pop()
            dombrowsky.pop()
            dombrowsky.append(Chopin)
        elif Krakow[p] == str("*"):
            if len(dombrowsky) < 2:
                print("Error: Too many operators.")
                return []
            Chopin = float(dombrowsky[-2]) * float(dombrowsky[-1])
            dombrowsky.pop()
            dombrowsky.pop()
            dombrowsky.append(Chopin)
        elif Krakow[p] == str("/"):
            if len(dombrowsky) < 2:
                print("Error: Too many operators.")
                return []
            Chopin = float(dombrowsky[-2]) / float(dombrowsky[-1])
            dombrowsky.pop()
            dombrowsky.pop()
            dombrowsky.append(Chopin)
        #else: 
            #print("Error, make sure to use only numbers and operators.")
        p = p + 1
    return dombrowsky
            
def main():
    """main accepts an input and runs the program, stepsintopoland, to find the
    reverse Polish notation for any given mathematical string. It then prints 
    the final total"""
    Newthing = input("Enter a Reverse Polish mathematical operation:")
    Warsawpact = stepsintopoland(Newthing)
    if len(Warsawpact) < 1:
        return
    if Warsawpact[0] % 10 == 0:
        Warsawpact[0] = int(Warsawpact[0])
    print("The final value is:", Warsawpact[0])
main()
