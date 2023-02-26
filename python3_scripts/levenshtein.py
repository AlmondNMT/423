
def dist(a, b):
    D = [[0 for i in range(len(b)+1)] for j in range(len(a)+1)]
    for i in range(1, len(a) + 1):
        D[i][0] = i
    for j in range(1, len(b) + 1):
        D[0][j] = j

    for i in range(1, len(a)+1):
        for j in range(1, len(b)+1):
            subcost = int(a[i-1] != b[j-1])
            D[i][j] = min(D[i-1][j] + 1, D[i][j-1] + 1, D[i-1][j-1] + subcost)
    return np.array(D)
a = "pqrstuvwx"
b = "qrsstnvwx"

D = dist(a, b)
print(D)
