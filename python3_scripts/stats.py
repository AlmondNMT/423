from decimal import Decimal
import matplotlib.pyplot as plt
import numpy as np
import os

### Set of useful statistics functions
def pearson(x, y, population = True):
    return covariance(x, y, population) / (stddev(x, population) * stddev(y, population))

def covariance(x, y, population = True):
    if population:
        return sum((x - np.mean(x)) * (y - np.mean(y))) / len(x)
    else:
        return sum((x - np.mean(x)) * (y - np.mean(y))) / (len(x) - 1)

def variance(x):
    return np.sqrt((sum(x ** 2) - sum(x) ** 2 / len(x)) / (len(x) - 1))

def residuals(f,vals):
    total = 0
    avg = np.mean(vals)
    for i, val in enumerate(vals):
        total += (avg - f(val)) ** 2
    return total

def squares(vals):
    total = 0
    avg = np.mean(vals)
    for i, val in enumerate(vals):
        total += (val-avg) ** 2
    return total

## Not currently a very useful function, as it uses the python input function to get values
# and would be better implemented with a gui
def getDataPoints():
    inputErrorMsg = "Invalid input"
    arr = []
    while True:
        try:
            numberOfMeasurements = int(input("Enter the number of measurements to be taken: "))
        except ValueError:
            print(inputErrorMsg)
        else:
            break
    for i in range(numberOfMeasurements):
        while True:
            try:
                arr.append(float(input("Data entry " + str(i + 1) + ": ")))
            except ValueError:
                print(inputErrorMsg)
            else:
                break
    return arr

## This function already exists via np.mean
def average(vals = None):
    if vals == None:
        vals = getDataPoints()
    else:
        arr = vals
    return sum(vals)/len(vals)

# Returns the standard deviation of a set of data using the population or sample formula
def stddev(vals = None, population = True):
    #if vals == None:
    #    vals = getDataPoints()
    average_x = np.mean(vals)
    print("Average:\t" + str(average_x))
    cumulative_total = 0
    for i, x in enumerate(vals):
        cumulative_total += (average_x - x) ** 2
    if population:
        return np.sqrt(cumulative_total / len(vals))
    else:
        return np.sqrt(cumulative_total / (len(vals) - 1))

## This function is used to find the least squared residuals of a data set and 
# returns the slope and y-intercept of the best-fit line. Because there was not 
# an explicit method for solving for the least squared residuals that I could 
# find on Wikipedia, I had to use the equation f(m,b) = ∑[(y - mx - b)²], which
# I then set each partial derivative to zero and solved for the respective m and b
# values to yield the minimum value of f(m,b).
'''
    Equation for finding the linear regression using the least square residuals method:
    m=(∑y∑x−n∑yx)/((∑x)²−n∑x²)
    b=(∑y∑x²−∑x∑yx)/(n∑x²−(∑x)²)

'''

# f_dist: Create a set data points based on some function, then return the 
#       points 
def f_dist(func, domain = (-1, 1), error = 1, precision = .01):
    x = np.arange(*domain, precision)
    y = []
    for e in x:
        y.append(func(e) + 2 * error * np.random.random() - error)
    y = np.array(y)
    return x, y

# Get the coefficients associated with a polynomial of degree deg
def get_weights(x, y, deg):
    x = np.array(x)
    y = np.array(y)
    M = np.matrix([[sum(x ** (j - i)) for j in range(deg * 2, deg - 1, -1)] for i in range(deg + 1)])
    s = np.matrix([sum(x ** j * y) for j in range(deg, -1, -1)]).T
    #sp.Matrix(M)*sp.Matrix(s)
    return M.I * s

# polynomial approximation function
def poly_f(x, w):
    x = np.array(x)
    w = np.array(w)
    return sum(x ** j * w[-j - 1] for j in range(len(w) - 1, -1, -1))

# Generate nth degree polynomial distributions
def generate(deg = 1, domain = (-1, 1), error = 1, precision = 0.01, radius = 10):
    coefficients = [2 * radius * np.random.random() - radius for i in range(deg + 1)]
    x = np.arange(domain[0], domain[1], precision)
    y = []
    coefficients = np.array(coefficients)
    def f(x):
        terms = np.array([x ** j for j in range(deg, -1, -1)])
        return terms.dot(coefficients) + error * np.random.random() - error / 2
    for element in x:
        y.append(f(element))
    y = np.array(y)
    return x, y, coefficients


# Return the normal distribution function given the mean and stddev
def normal(mean, stddev):
    def f(x):
        return 1 / (stddev * sqrt(2 * np.pi)) * np.exp(-.5 * (x - mean) ** 2 / stddev ** 2)
    return f

# Find the least-squared residuals (derived a more efficient algorithm 2019-11-3 ~ 0900)
## Returns the the slope (m) and y-intercept (b) in the format: [m, b]
def minr(x,y):
    n = len(x)
    x = np.array(x)
    y = np.array(y)
    m = (sum(y) * sum(x) - n * sum(x * y)) / (sum(x) ** 2 - n * sum(x ** 2))
    b = (sum(y) * sum(x ** 2) - sum(x) * sum(x * y)) / (n * sum(x ** 2) - sum(x) ** 2)
    return np.array([m, b])

def RMSE(x, y):    # Method for finding the RMSE (Root-Mean-Square error)
    m, b = minr(x, y)
    return np.sqrt(np.sum((y - m * x - b) ** 2))

## This function was used to test the least square residuals approach to linear regression by creating a set of random values along a domain
# and then plotting them using matplotlib.pyplot.
def testData(total = 100, max_residual = 10):
    fs = 10
    x = np.arange(0, total, .001)
    domain = np.arange(1, total + 1,1)
    fRange = []
    for i in range(total):
        fRange.append(domain[i] + (2 * max_residual * np.random.random() - max_residual + 1000))
    fRange = np.array(fRange)
    plt.scatter(domain, fRange) 
    
    # use least squares method to find best fit line for data
    m,b = minr(domain, fRange)
    plt.plot(x, m * x + b)
    print(m)
    print(b)
    plt.show()
