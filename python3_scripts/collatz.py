"""
A program to demonstrate import

file: collatz:py

author: anonymous

date: Feb 16

assignment: Lab 3
"""

def collatz_len(n):
    global q
    x = 0
    q = 0
    while x <= 0:
        if n == 1:
            q = q + 1
            x = x + 1  
        elif n%2 == 0:
            q = q + 1
            n = n / 2
        else:
            q= q + 1
            n =(3 * n) + 1
def main():
    n = input("Enter a starting number:")
    n = int(n)
    collatz_len(n)
    print ("Length of Collatz sequence: {}".format(q))

if __name__ == '__main__':
    main()
