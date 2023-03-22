"""
File name: nested.py

Author: anonymous

Date: March 25

Assignment: Lab 8
"""

def element_of (earth, theelements):
    """element_of() takes an input string and an input nested list and searches for the item in
    the list. It goes through every item in the list, and sorts them by type. If the item is in
    int, it returns true if it finds the item, and returns false if that is the last item in the 
    list. If the item is a list, it calls the function again to search through every item in that
    list. These return values can be transfered through each layer of element_of that needs to be
    run"""
    i = 0
    while i < len(theelements):
        if type(theelements[i]) == int:
            if theelements[i] == int(earth):
                return True
            else:
                if i == len(theelements):
                    return False
                else:
                    i = i + 1
        elif type(theelements[i]) == list:
            a = element_of(earth, theelements[i])
            if element_of(earth, theelements[i]):  #if true is returned, the final answer is here.
                return True
            else:                           #if false is returned, we move on
                i = i + 1
        elif type(theelements[i]) == str:       #strings are people too.
            if theelements[i] == earth:
                return True
            else:
                i = i + 1
        else:
            if i == len(theelements):
                return False
            else:
                i = i + 1
    
def filter_by_depth(water, theelements):
    """filter_by_depth takes in an integer and a nested list and uses them to remove layers of 
    lists inside the nested list. filter_by_depth first sorts the items of the list. Water is 
    the key parameter. If water hits zero, all the items in that list are removed. Every time
    that a item in theelements is a list type, the program is called again for this list, but 
    with water being one value lower than it was, since we are in a new layer. 
    It returns a new list, "air", with whatever values are left standing."""
    j = 0
    air = theelements
    while j < len(theelements):
        if type(theelements[j]) == int or type(theelements[j]) == str:
            if water < 1:           
                del air[:]      #air only deletes values in its "layer"
                j = j + 1
            else:
                j = j + 1
        elif type(theelements[j]) == list:
            if water < 1:
                del air[:]
                j = j + 1
            else:
                filter_by_depth(water-1, theelements[j])    #calls the function
                if theelements[j] == []:                    #removes extraneous brackets
                    theelements.remove(theelements[j])
                j = j + 1
    return air
def main():
    """main sorts between the two functions, element_of, and filter_by_depth. It asks the user for 
    input value and runs the function with a sample nested lest. I originally wanted the nested
    list to also be an input value, but struggled to write a program to translate an input string
    into a nested list. It will print the returned values for the selected function."""
    x = input("Do you want ELEMENT OF or FILTER BY DEPTH?")
    if x == str("ELEMENT OF") or x == str("element of"):
        fire = input("Enter the key character:")
        elementium = [5, [1, 2, 3, 4, 5, 6, 7]]             #sample nested list
        element_of(fire, elementium) 
        if element_of(fire, elementium):                    #if function returns true
            print("Elements abound! The item is in the list!") 
        else:
            print("Alas, the character isn't here!")
    if x == str("FILTER BY DEPTH") or x == str("filter by depth"):
        wind = int(input("Enter the fractal depth:"))
        thunder = [2, [1, 2, [3, 4, [5, 6]], 7]]        #sample nested list
        print(filter_by_depth(wind, thunder))
main()

