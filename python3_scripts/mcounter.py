import sys
n = 10
if len(sys.argv) == 2:
    n = int(sys.argv[1])

def mcount(n):
    counter = 0
    for l in range(1, n + 1):
        for i in range(1, n - l + 2):
            j = i + l - 1
            for k in range(i, j):
                counter += 1
    return counter

for i in range(1, n + 1, 1):
    print("%d: %d" % (i, mcount(i)))

#print(counter == (n ** 3 - n) / 6)
