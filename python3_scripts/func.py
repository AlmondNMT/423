#! /usr/bin/python3.5

mpl.use("TkAgg")
import os
import random
#import stats as st
import sys
import time

## x is a global numpy array useful for plotting graphs of functions
x = np.arange(-10,10,.001)

# Plot the slope field and solution curves of a differential equation
# diff is the derivative in terms of x and y
# f is a solution curve in terms of x and the constant of integration C
def solution_curves(diff, f, linewidth = 1, resolution = 41, dx = .2, interval = (-10, 10)):
    domain = np.arange(*interval, .001)
    fig, ax = plt.subplots()
    fig.subplots_adjust(.1, .1, .9, .9)
    lines = slope_field(ax, diff, color = "#000f1f", linewidth = linewidth, resolution = 41)
    sols = []
    C = interval[0]
    while C < interval[1]:
        sols.append(ax.plot(domain, f(domain, C), color = "r", linewidth = 2)[0])
        C += dx
    ax.set_ylim(interval)
    ax.set_xlim(interval)
    return lines, sols


# Plot a 3D graph of an implicit function F = 0
def plot_implicit(ax, F, domains, resolution = 200, slices = 30):
    ax.clear()
    Domains = []
    B = []
    for domain in domains:
        Domains.append(getMesh(domain, resolution = resolution))
        B.append(np.linspace(*domain, slices))
    for z in B[2]:
        X, Y = Domains[0]
        Z = F(X, Y, z)
        ax.contour(X, Y, Z + z, [z], zdir = 'z')
    for y in B[1]:
        X, Z = D1, D2
        Y = F(X, y, Z)
        ax.contour(X, Y + y, Z, [y], zdir = 'y')
    for x in B[0]:
        Y, Z = D1, D2
        X = F(x, Y, Z)
        ax.contour(X + x, Y, Z, [x], zdir = 'x')
    ax.set_box_aspect([1, 1, 1])
    ax.set_xlim3d(*domain)
    ax.set_ylim3d(*domain)
    ax.set_zlim3d(*domain)
    ax.set_xlabel("x")
    ax.set_ylabel("y")
    ax.set_zlabel("z")

def create_3D(aspect = ""):
    plt.ion()
    fig = plt.figure(figsize = (50,50))
    if aspect == "equal":
        ax = fig.add_subplot(111, aspect = "equal", projection = "3d")
    else:
        ax = fig.add_subplot(111, projection = "3d")
    ax.xaxis.label.set_text("x")
    ax.yaxis.label.set_text("y")
    ax.zaxis.label.set_text("z")
    plt.subplots_adjust(top = 1, right = 1, bottom = 0, left = 0)
    return ax

## Return 2D meshgrid
def grid(radius = 10, resolution = 200):
    return np.meshgrid(np.linspace(-radius, radius, resolution), np.linspace(-radius, radius, resolution))

### Return a sphere
def get_spherical_coord(radius, res = 200):
    theta, phi = np.meshgrid(np.linspace(0, 2 * pi, 200), np.linspace(0, pi, 200))
    x = radius * np.cos(theta) * np.sin(phi)
    y = radius * np.sin(theta) * np.sin(phi)
    z = radius * np.cos(phi)
    return x, y, z

## Clearing the terminal
def cls():
    if sys.platform == "win32":
        cmd("cls")
    elif sys.platform == "linux":
        cmd("clear")

## ls function to list the contents of the current directory. Not necessary in ipython
'''def ls(withOptions = False):
    if withOptions:
        cmd("ls -l")
    else:
        cmd("ls")'''

#### Vectors ####     
class V:
    # The first element of m_a list will be taken as the magnitude and the rest as angles
    def __init__(self, c_t = None, m_a = None):
        if c_t != None and m_a != None:
            ValueError("Can only accept either a component tuple or a magnitude/angles tuple.")
        if c_t == None and m_a == None:
            ValueError("Must pass either components or magnitude/angle list.")
        if c_t != None:
            self.components = np.array(c_t)
            self._calc_mag_angles()
        if m_a != None:
            self.magnitude = m_a[0]
            self.angles = m_a[1:len(m_a)]
            self._calc_components()

    def __getitem__(self,index):
        return self.components[index]

    def __len__(self):
        return len(self.components)

    def __add__(self, v):
        if not isinstance(v, V):
            ValueError("Invalid vector addition.")
        if len(self) != len(v):
            ValueError("Must add vectors of same dimensions.")
        components = (self.components + v.components)
        return V(list(components))
    def __repr__(self):
        return "Vector" + str(self.components)

    def _calc_mag_angles(self):
        self.magnitude = np.sqrt(np.sum(self.components**2))

        ## TODO (Probably just going to hope that vectors don't exceed 3 dimensions and calculate the angles for spherical coordinates)
        self.angles = None
    def _calc_components(self):
        theta = self.angles[0]
        if len(self.angles) == 2:
            r = self.magnitude
            phi = self.angles[1]
            self.components = np.array([r*np.sin(phi)*np.cos(theta),r*np.sin(phi)*np.sin(theta),r*np.cos(phi)])
        if len(self.angles) == 1:
            r = self.magnitude
            self.components = np.array([r*np.cos(theta),r*np.sin(theta)])

def cartesian(A, B):
    A = list(A)
    B = list(B)
    C = []
    for a in A:
        for b in B:
            C.append((a, b))
    return C


## Returns the dot product of two vectors, but a (probably) more optimized version of this 
# function exists in numpy.dot
def dot(v, w):
    componentCount = len(v)
    currentSum = 0
    for i in range(componentCount):
        currentSum += v[i] * w[i]
    return currentSum

def angle(u, v):
    return acos(dot(u, v) / (magnitude(u) * magnitude(v)))

## Squares each element in the vector then returns the square root of the sum to find 
# the magnitude of a the vector
def magnitude(v, string = False):
    total = np.array(np.array(v) ** 2).sum()
    if string:
        return str(reduce_radical(total)) + " or " + str(np.sqrt(total))
    else:
        return np.sqrt(total)

def distance(p1, p2):
    return np.sqrt(sum((p1[i] - p2[i]) ** 2 for i in range(len(p1))))

def unit_vector(v):
    return np.array(v) / magnitude(v)

# Returns the direction angles of each component of a vector
def direction_angles(v, string = False):
    mag = magnitude(v)
    angles=[]
    for component in v:
        angles.append(acos(component / mag))
    return np.array(angles)

# Returns a tuple representing the cross product of two 3D vectors
def cross(v,u,string=False):
    a = v[1] * u[2] - u[1] * v[2]
    b=v[0] * u[2] - u[0] * v[2]
    c=v[0] * u[1] - u[0] * v[1]
    if string:
        return "(" + str(a)+ ")i - (" + str(b) + ")j + (" + str(c) + ")k"
    else:
        return (a, -b, c)

def decompose(v, w):
    numer = dot(v, w)
    scalar = numer / magnitude(w) ** 2
    v1 = scalar * np.array(w)
    v2 = np.array(v) - v1
    return "v1: " + str(v1) + "\nv2: " + str(v2)

### Simplification and Exact Values ###

## Simply returns a string representation of a reduced fraction given the numerator and 
# denominator of an unsimplified fraction
def simplify_fraction(numer, denom):
    if numer > denom:
        for i in range(int(abs(denom)), 0, -1):
            if numer % i == 0 and denom % i == 0:
                denom /= i
                numer /= i
                if denom == 1:
                    return numer
    elif numer < denom:
        for i in range(int(abs(numer)), 0, -1):
            if numer % i == 0 and denom % i == 0:
                denom /= i
                numer /= i
                if denom == 1:
                    return numer
    else:
        return 1
    return str(int(numer)) + "/" + str(int(denom))

## Reduce the value of the radical 
def reduce_radical(radical):
    factors = []
    for i in range(radical // 2, 1, -1):
        if radical % i == 0 and sqrt(i) % 1 == 0:
            factors.append(sqrt(i))
            radical /= i
    reduced_integer = 1
    for i in range(len(factors)):
        if factors[i] is not 0:
            reduced_integer *= factors[i]
    return str(reduced_integer) + "*sqrt(" + str(radical) + ")"

## First-Order Differential Equation Slope Field Generator.
# This accepts a function of two variables: diff(x,y), representing the derivative as a 
# function of both x and y, plotting and display a slope field of arbitrary resolution.
def slope_field(ax, diff, color = "#abc888", interval = (-10, 10), resolution = 20, linewidth = 2):
    lineLength = .75 * (interval[1] - interval[0]) / resolution
    lines = []
    x, y = np.linspace(*interval, resolution), np.linspace(*interval, resolution)
    for i in x:
        for j in y:
            slope = diff(i,j)
            domain_radius = lineLength * np.cos(np.arctan(slope)) / 2
            #print("Line Length: " + str(np.sqrt((domain_radius * 2) ** 2 + (lineLength * np.sin(np.arctan(slope))) ** 2)))
            domain = np.linspace(i - domain_radius, i + domain_radius, 2)
            lines.append(ax.plot(domain, func(i, j), color = color, linewidth = linewidth, solid_capstyle = "projecting", solid_joinstyle = "bevel")[0])
    #ax.subplots_adjust(right=.999,top=.999,left=-.0001,bottom=.0001)
    return lines

## Create a vector field in two or three dimensions after passing a vector-valued function
def vector_field(F, dim, ax, length = .1, width = .003,color = "b", resolution = 12, normalize = True, domain = (-10, 10)):
    if dim == 3:
        norm = Normalize()
        x, y, z = getMesh3D(x = domain, y = domain, z = domain, resolution = resolution)
        #ax = create_3D()
        ax.quiver(x, y, z, *F(x, y, z), length = length, normalize = normalize)
    else:
        plt.ion()
        #fig = plt.figure(figsize=(50,50))
        #ax = fig.add_subplot(111)
        ax.set_facecolor("#000000")
        plt.subplots_adjust(0, 0, 1, 1)
        x, y = getMesh(x = domain, y = domain, resolution = resolution)
        ax.quiver(x, y, *F(x, y), width = width)
    return ax

## Approximates a numerical solution to first-order differential equations
def euler_approximation(diff, x0, y0, h=.01, linecolor="r"):
    lines = []
    n = int(10 / h)
    for i in range(n):
        #print("x: %f\ny: %f\n" % (x0,y0))
        slope = diff(x0, y0)
        #domain_radius = h*np.cos(np.arctan(slope))/2
        domain = np.linspace(x0, x0 + h, 2)
        lines.append(plt.plot(domain, y0 + slope * (domain - x0), color = linecolor)[0])
        x0 += h
        y0 += diff(x0, y0) * h
    return lines

## Given an implicit function as a solution to a first order differential equation, this can plot curves as a function of the arbitrary constant, using the global meshgrid x,y and F and G(C)
def plot_contour(C):
    return plt.contour(x, y, (F - G(C)))

""" def solution_curves(func,interval=(-1,1),color="ar",linewidth=2,resolution=.1):
    domain  = np.arange(interval[0],interval[1],.001)
    C = interval[0]
    lines = []
    while C <= interval[1]:
        lines.append(plt.plot(domain,func(domain),linewidth=linewidth,color=color)[0])
        C += resolution
    return lines
def vector_field(diff,linecolor="#bbbfff",interval=(-10,10),resolution=20):
    dash()
    plt.ion()
    arrows = []
    x,y = np.linspace(*interval,resolution),np.linspace(*interval,resolution)
    lineLength = .75

    for i in x:
        for j in y:
            slope = diff(i,j)
            theta = np.arctan(slope)
            dx,dy = lineLength*np.cos(theta),lineLength*np.sin(theta)
            arrows.append(plt.arrow(i,j,dx,dy,color=linecolor))
    plt.subplots_adjust(right=.999,top=.999,left=-.0001,bottom=.0001)
    plt.show()
    return arrows """
    
### Testing the parity of a function ###
def parity(func):

    table = {}
    
    err = "Error at x = "

    if -func(100) == func(-100):
        return "Odd"
    elif func(-100) == func(100):
        return "Even"
    else:
        return "Neither"
        
def sec(x):
    return 1 / np.cos(x)
def csc(x):
    return 1 / np.sin(x)
def cot(x):
    return np.cos(x) / np.sin(x)
    
def DMS(val, toMin = True, minut = None, sec = None): # Takes the entire value in degrees and converts it to minutes and seconds
    if minut is not None or sec is not None:
        toMin = False
        return round(val + minut/60 + sec/3600,2)
    if toMin:    
        curr_val = float("." + str(val).split(".")[1])
        minutes, seconds = str(curr_val * 60).split(".")
        seconds = "." + seconds
        print(seconds)
        seconds = str(round(float(seconds) * 60))
        
        return str(int(val)) + "00b0" + minutes + "'" + seconds + '"'
        
#### Plotting and graphing
def getMesh(x = (-10, 10), y = (-10, 10), resolution = 200):
    return np.meshgrid(np.linspace(x[0], x[1], resolution), np.linspace(y[0], y[1], resolution))
def getMesh3D(x = (-10, 10), y = (-10, 10), z = (-10, 10), resolution = 5):
    return np.meshgrid(np.linspace(x[0], x[1], resolution), np.linspace(y[0], y[1], resolution), np.linspace(z[0], z[1], resolution))

## Sets the background 
def stylize(xlabel = "x", ylabel = "y", ylimit = 10):
    style.use("dark_background")
    hline = plt.axhline(ls = 'solid')
    vline = plt.axvline(ls = 'solid')
    plt.xlabel(xlabel)
    plt.ylabel(ylabel)
    plt.grid(visible = True)
    plt.ylim(-ylimit, ylimit)
    plt.subplots_adjust(right = 1, top = 1, left = 0, bottom = 0)
    return hline, vline
    
## Summation of a sequence (function) from k to n
def summation(func, k, n):
    total = 0
    for i in range(k, n + 1, 1):
        total += func(i)
    return total

### Product of n terms
def product(func, k, n):
    total = 1
    for i in range(k, n + 1, 1):
        total *= func(i)
    return total

## Returns a series of tangent lines on the interval of a to b of a function with the number of tangent 
# lines determined by the resolution.
def tangentize(f, a = -10, b = 10, resolution = 100):
    inc= a
    while inc < 5:
        plt.plot(x, deriv(f, inc) * (x - inc) + f(inc))
        inc += 1 / resolution
    return 

# f is a function and x is the value to take the derivative at. 
# This currently needs work as accuracy varies greatly for different types of functions
# and suffers even greater losses in accuracy when attempting to return derivatives of any 
# order higher than one.

def deriv(f, x, order=1): 
    x = x
    h=.00001
    if order is 1:
        return (f(x + h) - f(x)) / h
    else:
        return deriv()

## Return the divergence of a vector-valued function at a specified point
def divergence(f, *r):
    return np.diag(partial(f, r[0])).sum()

## TODO Return the curl of a vector-valued function in 3 dimensions
#def curl(f, *r):
#    operator = np.array(

## Partial v2: pass a function f with regular parameterization. Returns list of the partial derivatives at the specified point
def partial(f, args):
    partials = []
    h = .0001
    for i in range(len(args[0])):
        h_params = [args[0]]
        h_params[i] += h
        partials.append((f(*h_params) - f(args[0])) / h)
    return partials

# Replaced by a better version above
'''def partial(f,args):
    partials = []
    def resetArgs(args):
            newAr = []
            for i, val in enumerate(args[0]):
                newAr.append(val)
            return newAr
    for i, x in enumerate(args[0]):
        newArgs = resetArgs(args)
        newArgs[i] += .000001
        partials.append((f(newArgs)-f(args[0]))/.000001)
    return partials'''


def cylinder(f, a, b):    # finding the volume with cylindrical shells
    return integrate(newfunc, a, b, 2000)

def washer(f, a, b):
    return integrate(newfunc, a, b, 2000)

def Newton_Raphson(f, c_1, max_iterations = 10):
    c = []
    c.append(c_1)
    for i in range(max_iterations):
        c.append(c[i] - f(c[i]) / deriv(f, c[i]))
    return c[-1]

# Sum midpoint rectangles of arbitrary size to approximate area under the curve of a function f
def integrate(f, a, b, n, side = "mid"):
    delta_x = (b - a) / n
    total = 0
    if side == "left":
        current_x = a
    elif side=="right":
        current_x = a + delta_x
    elif side=="mid":
        current_x = (a + a + delta_x)/2
    for i in range(n):
        total += delta_x * f(current_x)
        current_x += delta_x
    return total

def trapezoid(f, a, b, n):
    delta_x = (b - a) / n
    return (1 / 2) * (integrate(f, a, b, n, side = 'left') + integrate(f, a, b, n, side = 'right'))

def simpson(f, a, b, n):
    if not n % 2 == 0:
        ValueError("n must be an even number.")
    current_x = a
    delta_x = (b - a) / n
    total = f(a)
    current_x += delta_x
    for i in range(1, n, 1):
        if i % 2 == 1:
            total += 4 * f(current_x)
        else:
            total += 2 * f(current_x)
        current_x += delta_x
    total += f(current_x)
    total *= (delta_x / 3)
    return total

def favg(f, a, b):
    return (1 / (b - a)) * integrate(f, a, b, 2000)

def binomial_coefficients(n, j):
    return factorial(n) / (factorial(j) * factorial(n - j))

def expansion(x, a, deg):
    outputStr = ""
    for i in range(deg + 1):
        if deg - i == 0:
            outputStr += str(bino(deg,i) * a ** i * x ** (deg - i))
        else:
            outputStr += str(bino(deg, i) * a ** i * x ** (deg - i)) + "x^" + str(deg-i) + "+ "
    return outputStr
    
def plotter(arr, func):
    newar = []
    for i in range(len(arr)):
        newar.append(func(arr[i]))
    return np.array(newar)
    
def divis(val):
    for i in range(1, val // 2):
        if val % i == 0:
            print (str(i) + "\t:\t" + str(val / i))
            
#### Derive a function based on the data points ((Incomplete)) ####
def getfunc():
    val = []
    a = b = c = None
    funcstr = input("Enter function: ")
    for i in range(len(funcstr)):
        print()

## This function accepts a function argument (single variables for now) and parses through the string representation of the function's definition and locates key values
def analyze(f):
        fstring = ins.getsource(f)
        if fstring.find("def") == 0:
                islambda=False
                fname=fstring.split("(")[0].split(" ")[1].strip()
                varname=fstring.split("(")[1].split(")")[0].strip()
                algorithm=fstring.split("return")[1].strip()
        if fstring.split("=")[1].split(" ")[0] == "lambda":
                islambda=True
                fname=fstring.split("=")[0].strip(" ")
                varname=fstring.split("lambda")[1].split(":")[0].strip()
                algorithm=fstring.split(":")[1].strip()
        
def midpoint(a,b):
    a,b=np.array(a),np.array(b)
    return (a + b) / 2

def fibbonaci(length):
    fib = [0,1]
    for i in range(1, length):
        fib.append(fib[i] + fib[i - 1])
    return fib

## Heron's Formula: Find's the area of a triangle using it's three side lengths
def heron(a, b, c):
    s=(a + b + c) / 2
    return sqrt(s * (s - a) * (s - b) * (s - c))

## Chaos Theory
def chaos(y0, k, n):
    y0 = Decimal(y0)
    k = Decimal(k)
    a = [Decimal(y0)]
    for i in range(len(n) - 1):
        a.append(k * a[i] - k * a[i] ** 2)
    return a

# Actually useful for slowing output but the time module itself has a function for doing this
def timer(val, func = None, fVal = None, compute = None):
    tm_val = time.time() + val
    while tm_val - time.time() > .00001:
        if compute is None:
            print()
        else:
            compute()
    if func is not None and fVal is not None:
        return func(fVal)
    elif func is not None and fVal is None:
        return func()
        
##################################################################################################
        
# Binary to decimal 
def binary_to_decimal(val):
    val = str(val)
    total = 0
    for i, x in enumerate(val):
        total += int(x) * 2 ** (len(val) - (i + 1))
    return total
    

# Physics
g = 9.8             # m/s²
elementary_charge = 1.602e-19    # elementary magnitude of the charge of a proton or electron
permittivity_constant = 8.85418782e-12        # C²/N·m²; permittivity constant
G = 6.67e-11             # N∙m²/kg²
permeability_constant = 1.25663706e-6     # N∙s/C
planck = 6.62607004e-34 # J∙s

class kinematic: ### Assumes constant acceleration
    def __init__(self, delta_x = None,v0 = None,vf = None,t = None,a = None):
        self.delta_x = delta_x
        self.v0 = v0
        self.vf = vf
        self.t = t
        self.a = a
    def __check(self):
        if self.delta_x is None:
            print()
    def displacement(self):
            return self.v0*self.t + .5 * self.a * self.t ** 2
    def final_v(self):
        return 

def find_g(L,h,t): # Calculate g for the given values of L (length of the incline), h (vertical distance from one photogate to the other), t (elapsed time)
    return 2 * L / (t ** 2 * h)
