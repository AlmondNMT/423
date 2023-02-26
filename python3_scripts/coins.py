<<<<<<< HEAD


def gen_coins(n):

=======
import random

def gen_coins(n):
>>>>>>> f0fc3a18355e94ee65ea979f1da6cf7a068ce105
    a = [1 for i in range(n)]
    index = random.randint(0, n - 1)
    a[index] += random.normalvariate(0, 0.05)
    return a

def find_counterfeit(coins):
    n = len(coins)
<<<<<<< HEAD
   
=======
>>>>>>> f0fc3a18355e94ee65ea979f1da6cf7a068ce105
    left = coins[0 : floor(n / 2)]
    right = coins[ceil(n / 2) : n]
    lw = sum(left)
    rw = sum(right)
    if lw == rw:
        avg = np.mean(left)
        center = coins[n // 2]

coins = gen_coins(3)
print(coins)
