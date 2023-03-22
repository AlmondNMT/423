# Less than 1 Calm
# 3 1-3 Light air
# 4 4-27 Breeze
# 5 28-47 Gale
# 6 48-63 Storm
# 7 Above 63 Hurricane
#Look Mr. Frodo: Elifs!

speed = int(input("Please enter the speed of the wind (in knots) as an integer:"))
if speed > 63:
    print("There is a hurricane in your area, please seek shelter")
elif 48 <= speed <= 63:
    print("There is a storm in your area, be careful")
elif 47 >= speed >= 28:
    print("There is a gale of wind in your area, be cautious")
elif 4 <= speed <= 27:
    print("There is a breeze in your area")
elif 1 <= speed <= 3:
    print("There is a gentle zephyr of light air in your area")
elif 0 < speed < 1:
    print("The air in your area is calm")
else:
    print("Error not a valid knot speed")
    