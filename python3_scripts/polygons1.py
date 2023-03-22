import turtle

z = input("enter a positive integer for the length of perfect polygons, foolish mortal:")
mcturtle = turtle.Turtle()
for s in range (5):
    for q in range (int(s+3)):
        mcturtle.forward(1*int(z))
        mcturtle.left(360/(s+3))
        
    mcturtle.forward(3*int(z))


turtle.done()