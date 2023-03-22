import turtle

z = int(input("enter a positive integer for the sides of the perfect polygons, foolish mortal:"))
if z <= 2:
    print("You have failed, mortal! No polygons availible have this many sides!")
else:
    q = int(input("enter the length of the sides:"))
    mcturtle = turtle.Turtle()
    for s in range (z):
        mcturtle.forward(q)
        mcturtle.left(360/(z))

    turtle.done()