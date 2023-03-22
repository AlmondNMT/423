"""
A program to demonstrate turtle power

file: navigate.py

author: anonymous

date: Feb 16

assignment: Lab 3
"""

import turtle
navlog = []
def main():
        q = input("Do you want to move forward, turn, or stop? Enter a command:")

def nav():
        
        s = 0
        mcturtle = turtle.Turtle()
        while s!=1:
                q = input("Do you want to move forward, turn, or stop? Enter a command:")
                q = str(q)
                if q == "stop":
                        s = 1
                        turtle.done()
                elif q == "forward":
                        mcturtle.forward(100)
                elif q == "left":
                        d = input("How many degrees?")
                        d = float(d)
                mcturtle.left(d)
                elif q == "right":
                        z = input("How many degrees?")
                        z = float(z)
                        mcturtle.right(z)
                else:
                        print("Error. Invalid command. Please type forward, left, right, or stop.")

        turtle.done()
        del(turtle)

if __name__ == '__main__':
        main()
