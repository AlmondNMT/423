import numpy as np
import os
import tkinter as tk
import time

# Avagadro's Number
avagadro = 6.02214076 * 10**23

# Ideal Gas Law Constant R
R = .08206 # atm·L/mol·K

# Planck's Constant
h = 6.626e-34 # J·s

# Speed of Light (m/s)
c = 299792458 # m/s

## Element class; primarily useful for intro chemistry class and performing basic arithmetic
# on elemental masses
class Element:
    def __init__(self,symbol,name,atomic_number,average_mass,period,group):
        self.__sym = symbol
        self.__name = name
        self.__number = atomic_number
        self.__mass = average_mass
        self.__period = period
        self.__group = group
    def __add__(self, e2):
        if type(e2) == type(self):
            return self.__mass + e2.__mass
        else: return self.__mass + e2
    def __truediv__(self, e2):
        if type(e2) == type(self):
            return self.__mass / e2.__mass
        else: return self.__mass / e2
    def __mul__(self,e2):
        if type(e2) == type(self):
            return self.__mass * e2.__mass
        else: return self.__mass * e2
    def __radd__(self,e):
        return e + self.__mass
    def __rmul__(self,e):
        return e * self.__mass
    def __rtruediv__(self,e):
        return e/self.__mass
    def __sub__(self,e2):
        if type(e2) == type(self):
            return self.__mass - e2.__mass
        else: return self.__mass - e2
    def __repr__(self):
        return str((self.__sym, self.__name,"Atomic Number: " + str(self.__number),"Mass: " + str(self.__mass),"Period: " + str(self.__period), "Group: " + str(self.__group)))
    def mass():
        return this.__mass

# This does not contain all the elements but just the ones I needed for my class.
Ag = Element("Ag","Silver", 47,107.87,5,11,)
Al = Element("Al","Aluminum",13,26.98,3,13)
Ar = Element("Ar","Argon",18,39.95,3,18)
B = Element("B", "Boron", 5, 10.81, 2, 13)
Ba = Element("Ba","Barium",56,137.33,6,2)
Be = Element("Be","Beryllium",4,9.01,2,2)
Br = Element("Br","Bromine",35,79.90,4,17)
C = Element("C","Carbon",6,12.01,2,14)
Ca = Element("Ca","Calcium",20,40.08,4,2)
Cl = Element("Cl","Chlorine",17,35.45,3,17)
Co = Element("Co","Cobalt",27,58.93,4,9)
Cr = Element("Cr","Chromium",24,51.9961,4,6)
Cs = Element("Cs","Cesium",55,132.9055,6,1)
Cu = Element("Cu","Copper",29,63.55,4,11)
F = Element("F", "Fluorine", 9, 19.00, 2, 17)
Fe = Element("Fe","Iron", 26, 55.85,4,8)
Fr = Element("Fr","Francium",87,223.0197,7,1)
Ge = Element("Ge","Germanium",32,72.64,4,14)
H = Element("H","Hydrogen", 1, 1.01,1,1)
He = Element("He","Helium",2,4.00,1,18)
Hg = Element("Hg","Mercury", 80, 200.59, 6,12)
I = Element("I", "Iodine", 53,126.90,5,17)
K = Element("K","Potassium",19,39.10,4,1)
Kr = Element("Kr","Krypton",36,83.80,4,28)
Li = Element("Li","Lithium",3,6.94,2,1)
Mg = Element("Mg","Magnesium",12,24.31,3,2)
Mn = Element("Mn","Manganese",25,54.9380,4,7)
N = Element("N","Nitrogen",7, 14.01,2,15)
Na = Element("Na","Sodium",11,22.99,3,1)
Ne = Element("Ne","Neon",10,20.18,2,18)
Ni = Element("Ni","Nickel",28,58.69,4,10)
O = Element("O","Oxygen",8,16.00, 2,16)
P = Element("P","Phosphorus",15,30.97,3,15)
Pb = Element("Pb","Lead",82,207.20,6,14)
Ra = Element("Ra","Radium",88,226.0254,7,2)
Rb = Element("Rb","Rubidium",37,85.4678,5,1)
S = Element("S","Sulfur", 16,32.07,3,16)
Sc = Element("Sc","Scandium",21,44.9559,4,3)
Sb = Element("Sb","Antimony",51,121.76,5,15)
Se = Element("Se","Selenium",34,78.96,4,16)
Si = Element("Si","Silicon",14,28.09,3,14)
Sn = Element("Sn","Tin",50,118.71,5,14)
Sr = Element("Sr","Strontium",38,87.62,5,2)
Ti = Element("Ti","Titanium",22,47.867,4,4)
V = Element("V","Vanadium",23,50.9415,4,5)
Xe = Element("Xe","Xenon",54,131.293,5,18)
Zn = Element("Zn","Zinc", 30,65.41,4,12)

def empirical(*args): # Provide percent first then element for each element
    molelist = []
    if len(args) % 2 != 0:
        raise ValueError("Uneven number of arguments.")
    for i in range(0,len(args),2):
        molelist.append(args[i]/args[i+1])
    for i in range(len(molelist)):
        molelist[i] = molelist[i] / min(molelist)
    return np.array(molelist)
