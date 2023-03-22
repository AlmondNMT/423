"""
A program to allow turtles to achieve a new plane of existence. They have achieved new consciousness as a species and can now 
reproduce and populate the earth. They do not need you to ask them for things directly, when they can follow your 
psychic commands from above. 

file: navigate3.py

author: anonymous

date: April 6th

assignment: Lab 10
"""
def main():
    """main() is new and improved! It can now read commands from a file. It turns the contents of that file into a string, franklin
    and runs those commands through listening. It then sorts the string entires into a list of commands and runs them through 
    log()."""
    #Program of inputs. Creates the list. Arms the TURTLE.
    sample_input = open("sample_input.txt", "r+")
    franklin = ""
    with open("sample_input.txt", "r") as sample_input:    
        bert = sample_input.readline()
        while bert !=  "":
            franklin = franklin + bert     #message from file 1 transfered.
            bert = sample_input.readline()
    splinter = listening(franklin)
    nav = []
    x = 0
    while x < len(splinter):
        z = splinter[x]
        if z == "stop":
            nav.append("stop")
            x = x + 1
        elif z == "left":
            nav.append("left")
            x = x + 1
            k = splinter[x]
            k = float(k)
            nav.append(k)
            x = x + 1
        elif z == "right":
            nav.append("right")
            x = x + 1
            d = splinter[x]
            d = float(d)
            nav.append(d)
            x = x + 1
        elif z == "forward":
            nav.append("forward")
            x = x + 1
            r = splinter[x]
            r = float(r)
            nav.append(r)
            x = x + 1
        elif z == "split":
            nav.append("split")
            x = x + 1
            i = splinter[x]
            i = float(i)
            nav.append(i)
            x = x + 1
        else:
            print("Error. Invalid command. Please type forward, left, right, or stop.")     #Error filter.
            x = x + 1
    log(nav, 1)

def listening(halfshell):
    """listening() takes string and seperates that string into lines. It then splits those lines by the spaces to separate the 
    elements of the vector (direction + magnitude). It returns a set of instructions as a list (raphael). """
    leonardo = halfshell.split("\n")
    t = 0
    raphael = []
    while t < len(leonardo):
        michelangelo = leonardo[t]
        donatello = michelangelo.split(" ")
        c = 0
        while c < len(donatello):
            if donatello[c] == " ":
                c = c + 1
            else:
                raphael.append(donatello[c])
                c = c + 1
        t = t + 1
    return raphael
def log(nav, x):
    """log() first creates the turtle army, and starts running the turtle program as mcturtle. It is able to read the commands
    and use them to move the turtle, as before, but this time it moves the whole turtle army! Every time the "split" command occurs
    a new koopa troopa is added to the army, but koopa will be rotated slightly right. Eventually, they will conquer the land,
    and kidnap Princess Peach."""
    turtlearmy = []
    import turtle
    mcturtle = turtle.Turtle()
    turtlearmy.append(mcturtle)
    print(turtlearmy)
    b = 0
    while b < len(nav):
        if nav[b] == "stop":
            b = b + 1
        elif nav[b] == "left":
            b = b + 1
            q = 0
            while q < len(turtlearmy):
                lonelygeorge = turtlearmy[q]
                lonelygeorge.left(float(nav[b]))
                q = q + 1
            b = b + 1
        elif nav[b] == "right":
            b = b + 1
            g = 0
            while g < len(turtlearmy):
                squirt = turtlearmy[g]
                squirt.right(float(nav[b]))
                g = g + 1
            b = b + 1
        elif nav[b] == "forward":
            b = b + 1
            j = 0
            while j < len(turtlearmy):
                crush = turtlearmy[j]
                crush.speed(300)
                crush.forward(float(nav[b]))
                j = j + 1

            b = b + 1
        elif nav[b] == "split":
            b = b + 1
            a = 0
            e = len(turtlearmy)
            while a < e:
                troopa = turtlearmy[a]
                koopa = troopa.clone()
                koopa.right(float(nav[b]))
                turtlearmy.append(koopa)
                a = a + 1
            b = b + 1
if __name__ == '__main__':
        main()



            
