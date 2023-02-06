from math import ceil, floor
import numpy as np
import random

def gen_coins(n: int):
    assert type(n) is int
    assert n > 1
    a = [1 for i in range(n)]
    index = random.randint(0, n - 1)
    a[index] += random.normalvariate(0, 0.05)
    return a

def find_counterfeit(coins):
    n = len(coins)
    assert n > 1
    left = coins[0 : floor(n / 2)]
    right = coins[ceil(n / 2) : n]
    lw = sum(left)
    rw = sum(right)
    if lw == rw:
        avg = np.mean(left)
        center = coins[n // 2]

coins = gen_coins(3)
print(coins)
