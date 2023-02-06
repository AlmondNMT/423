import re

### General utility functions ###

## Locates a string in an enumerable object which at least slightly matches the given pattern
# and returns an array of potentially matching strings
def find(pattern, arr, caseInsensitive=True):
    if caseInsensitive:
        pattern = pattern.lower()
    matchArr = []
    currentIndex = 0
    prevIndex = 0
    for arrIndex, string in enumerate(arr):
        originalString=string
        if caseInsensitive:
            string = string.lower()
        for charIndex, char in enumerate(string):
            if char == pattern[currentIndex]:
                currentIndex += 1
                prevIndex = charIndex
            if charIndex-prevIndex > 1:
                #print(str(charIndex-prevIndex) + ":\tWord: " + string)
                currentIndex = 0
            if currentIndex == len(pattern):
                matchArr.append(originalString)
                currentIndex = 0
                break
    return matchArr

## TODO Add function that traverses the method tree of an object until it 
#   reaches either a primitive type or basic object methods. Returns a list of
#   methods and primitive types. Pass a string representation of the object for
#   better documentation
def expand(obj, obj_repr, depth = 5):
    attr_list = []
    string = obj_repr
    if depth > 0:
        for item in dir(obj):
            if item == "__abstractmethods__":
                continue
            new_string = string + "." + item
            new_obj = getattr(obj, item)
            attr_list.append(new_string)
            if dir(obj) != dir(getattr(obj, item)):
                new_list = expand(new_obj, new_string, depth - 1)
                attr_list.extend(new_list)
    return attr_list

## Use expand and find functions to locate a specified pattern in an array
def locate(search, obj, obj_string, depth = 2):
    return find(search, expand(obj, obj_string, depth = depth))
