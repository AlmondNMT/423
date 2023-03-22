def rgcomd(num1, num2):
    """This function takes in two integers and determines the greatest common denominator recursively"""
    if num1 == min(num1, num2):
        z = num1
    else:
        z = num2

    if num1 == 0 or num2 == 0:
        z = max(num1, num2)
        return z
    else:
        num1 = int(num1)
        num2 = int(num2)
        print('num1: ', num1, ' num2: ', num2, ' z: ', z)

        if num1 % z == 0 and num2 % z == 0:
            print('is we here')
            print("The greatest common denominator is:", z)
            return z

        else:
            return rgcomd(max(num1, num2), ((max(num1, num2) % z)))


def main():
    num = rgcomd(16, 52)
    print("The greatest comm")

if __name__ == '__main__':
    main()