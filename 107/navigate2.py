"""
A program to allow turtles to independently follow a list of instructions.
Turtles don't need to ask for help!

file: navigate2.py

author: anonymous

date: Feb 28

assignment: Lab 4
"""
def main():
    #Program of inputs. Creates the list. Arms the TURTLE.
    nav = []
    x = 1
    while x == 1:
        z = input("Do you want to move forward, left, right, or stop? Enter a command:")
        z = str(z)
        if z == "stop":
            nav.append("stop")
            x = x+1
        elif z == "left":
            nav.append("left")
            k = input("How many degrees?")
            k = float(k)
            nav.append(k)
        elif z == "right":
            nav.append("right")
            d = input("How many degrees?")
            d = float(d)
            nav.append(d)
        
        elif z == "forward":
            nav.append("forward")
        else:
            print("Error. Invalid command. Please type forward, left, right, or stop.")
    log(nav, x)

def log(nav, x):
    import turtle
    mcturtle = turtle.Turtle()
    turtle.speed(0.5)
    b = 0
    while x == 2:
        if nav[b] == "stop":
            x = x + 1
        if nav[b] == "left":
            b = b + 1
            mcturtle.left(float(nav[b]))
            b = b + 1
        if nav[b] == "right":
            b = b + 1
            mcturtle.right(float(nav[b]))
            b = b + 1
        if nav[b] == "forward":
            mcturtle.forward(100)
            b = b + 1

if __name__ == '__main__':
        main()



            
