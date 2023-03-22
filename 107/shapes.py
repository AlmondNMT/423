#circumference = 2 * 3.1415 * R
#A = π * r ** 2
#2 × (length + width)

shape = input("Please enter a shape in lower case letters: ")

if shape == "circle":
    rad = float(input("Please enter the radius of the circle: "))
    Circ = 2*3.1415*rad
    Area = (3.1415*rad)**2
    print ("The circumference of the circle is: ", Circ)
    print ("The area of the circle is: ", Area)
    
elif shape == 'rectangle':
    width = input("Enter width")
    length = input("Enter length")
    width1 = float(width)
    length1 = float(length)
    Area_Rectangle = (length1 * width1) 
    Rectangle_Perim = (length1 * 2) + (width1 * 2)
    print("The area of the rectangle is:", Area_Rectangle)
    print("The perimiter of the rectangle is:", Rectangle_Perim)

elif shape == "square":
    side = float(input("Enter side length:"))
    area_square = side ** 2
    perimiter_square = side * 4
    print("The area of the square is:", area_square)
    print("The perimiter of the circle is:", perimiter_square)
else:
    print("Sorry, I cannot help you. Valid shapes are: circle, rectangle, square.")    
