"""
A program to demonstrate import and ucntions

file: longest_collatz.py

author: anonymous

date: Feb 16

assignment: lab 3
"""

import collatz
def main():
    m = 1
    longest_q = 1
    while m <= 1000000:
        collatz.collatz_len(m)  
        m= m + 1
        if ((collatz.q-longest_q)>=0):
            longest_q = collatz.q 
            l = (m - 1)
    print ("Longest chain is produced by {} with a sequence length of {}".format(l, longest_q))

if __name__ == '__main__':
    main()
    


    


    
    
        
