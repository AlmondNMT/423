#! /usr/bin/python3

from func import *

# Returns the average distance between data points
def avg_diff(values):
    total = 0
    for i in range(len(values)-1):
        total += np.abs(values[i+1] - values[i])
    return total  / (len(values)-1)

# Class to build linear regression plot on matplotlib based on a set of x and y data points
class LR:
    def __init__(self, x, y, title="y vs x",xlabel="x",ylabel="y",fontsize=20,linecolor="#ff6f00",marker="o"):
        # Set the title, horizontal, and vertical axes names
        self.title = plt.title(title,fontsize=fontsize)
        self.xlabel = plt.xlabel(xlabel,fontsize=fontsize-6)
        self.ylabel = plt.ylabel(ylabel,fontsize=fontsize-6)

        # Calculate the slope and y-intercept of the best-fit line
        self.slope,self.y_intercept = st.minr(x,y)

        # Enable the grid
        plt.grid(True)

        # Get the x and y axes for the grid
        self.x_axis = plt.axhline(ls='dashed',color="#000000")
        self.y_axis = plt.axvline(ls='dashed',color="#000000")

        # Constrain the graph to a domain and range based on the data
        x_distance = avg_diff(x)
        y_distance = avg_diff(y)
        left = np.min(x) - x_distance
        right = np.max(x) + x_distance
        top = np.max(y) + y_distance
        bottom = np.min(y) - y_distance
        plt.xlim(left,right)
        plt.ylim(bottom,top)

        domain = np.arange(left, right,.001)

        # Set the graph subplots
        padding = .05
        plt.subplots_adjust(left = padding, right = 1 - padding, top = 1 - padding, bottom = padding)

        # Plot the x and y values in a scatter plot and the best-fit line using a domain
        self.scatter = plt.scatter(x,y,color=linecolor,s=40,marker=marker)
        self.line, = plt.plot(domain,self.slope*domain + self.y_intercept,color=linecolor)

    def show(self):
        plt.show()

    def set_line_color(self,color):
        self.line.set_color(color)

    def save(self):
        plt.savefig(self.title.get_text().replace(" ","_") + ".png")
