"""
File name: cesaro.py

Author: anonymous

Date: March 26th

Assignment: Lab 8
"""
import turtle
turtle.speed('fastest')
mcturtle = turtle.Turtle()
def cesaro(ina):
    """cesaro() takes an input int and uses it to determine the number of times it needs to repeat
    itself. It uses a for loop to call every iteration before the target pattern, and then the target
    pattern calls a loop of continually decreasing iterations after each turtle.forward command."""
    for x in range(int(ina) + 1):               #for loop counts up 1, 2, 3, 4...
        cicero = x
        hadrian = x
        plutarch = x
        mcturtle.forward(15)         #I'm not trying to win, I am trying to weave a beautiful pattern
        mcturtle.right(85)
        cicero = int(cicero) - 1            #count down the function 4, 3, 2, 1...
        if cicero > 0:
            cesaro(cicero)                  #call the function x amount of times
        mcturtle.forward(15)
        mcturtle.left(170)
        hadrian = int(hadrian) - 1          #call the function after every command
        if hadrian > 0:
            cesaro(hadrian)
        mcturtle.forward(15)
        mcturtle.right(85)
        plutarch = int(plutarch) - 1
        if plutarch > 0:
            cesaro(plutarch)

def main():
    """main accepts an integer as an input and runs it through cesaro."""
    das = input("Enter the fractal depth:")
    cesaro(das)
main()
