f = input("Enter a dollar amount: ")
j = int(f)/20 
print (int(j))
rem = int(f) % 20
print (rem/20)
twen = j - (rem/20)
print ("$20 bills:", round(twen))
h = rem/10
rem2 = rem % 10
ten = h - (rem2/10)
print ("$10 bills:", round(ten))
l = rem2/5
rem3 = rem2 % 5
five = l - (rem3/10)
print ("$5 bills:", int(five))
one = rem3
print ("$1 bills:", one)
