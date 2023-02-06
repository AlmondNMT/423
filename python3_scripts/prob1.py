import matplotlib.pyplot as plt
import numpy as np

plt.ion()
f = lambda n: np.log2(n) / np.log10(n)

n = np.arange(2, 100000, 1)

fig, ax = plt.subplots()
ax.plot(n, f(n))
