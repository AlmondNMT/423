Emp = int(input("Enter the type of employee:"))

if Emp == 1:
    pay = float(input("Enter weekly salary:"))
    print("The manager's pay is", pay)
elif Emp == 2:
    Min = float(input("Enter the hourly salary:"))
    Hour = float(input("Enter the total hours worked:"))
    Over = Hour - 40
    Super = (Over*Min*1.5)+40*Min
    if Hour <= 0:
        print("Having health issues on the job is the same as stealing from the company.")
    elif 0 < Hour <= 40:
        print("The worker's pay is:", Min*Hour)
    else:
        print("The worker's pay is:", Super)    
elif Emp == 3:
    Sal = float(input("Enter gross weekly sales:"))
    Mon = ((Sal*5.7)/100)+250
    print("The commision worker's pay is:", Mon)
elif Emp == 4 :
    Pie = float(input("Enter number of pieces made:"))
    Pri = float(input("Enter the wage per piece:"))
    Cas = Pie * Pri
    print("The piece worker's pay is:", Cas)
else:
    print ("Error, the given employee code is not valid.")
    print("Valid coes are:")
    print("1: Manager; 2: Hourly worker; 3: Commission Worker; 4: Pieceworker")
