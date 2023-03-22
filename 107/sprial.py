import turtle
z = input("enter an angle:")
mcturtle = turtle.Turtle()
for s in range (128):
        mcturtle.forward(float((s+1)*5))
        mcturtle.right(float(z))
    