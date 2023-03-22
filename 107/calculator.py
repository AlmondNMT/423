import math
num = float(input("Enter a number to use:"))
op = str(input("Which operation? sqrt(s), arcsin(a), arccos (c), arctan(t):"))
if op == "s":
    sr = math.sqrt(num)
    print("The square root of the input is:", sr)
elif op == "a":
    asi = math.asin(num)
    print("The arcsine of the input is:")
    print(asi)
elif op == "c":
    acos = math.acos(num)
    print("The arccosine of the input is:")
    print(acos)
elif op == "t":
    atan = math.atan(num)
    print("The arctangent of the input is:")
    print(atan)
else:
    print("Error. This is an invalid operation for this calculator.")