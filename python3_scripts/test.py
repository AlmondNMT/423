import matplotlib.pyplot as plt
import numpy as np
import random
from iris import read_iris

from util import *

"""
Iris CSV Structure
sepal length, sepal width, petal length, petal width
"""
random.seed(1)

def read_csv():
    """
    Pull csv data from data/iris.data
    """
    attributes = []
    labels = []
    with open("data/iris.data", "r") as f:
        lines = f.readlines()
        for line in lines:
            datum = line.split(",")
            label = datum[-1].rstrip()
            attrs = [float(attr) for attr in datum[:-1]]
            attributes.append(attrs)
            labels.append(label)
        return np.array(attributes), np.array(labels)

def get_train_test_split(split: float):
    """
    :param split: float between .1 and .9 which determines 
    how much training data to use 
    """
    assert 0.1 <= split <= 0.9
    attrs, labels = read_csv()
    combined = list(zip(attrs, labels))
    random.shuffle(combined)
    n = round(split * len(combined))
    attrs, labels = zip(*combined)
    train_attrs = np.array(attrs[:n])
    train_labels = np.array(labels[:n])
    test_attrs = np.array(attrs[n:])
    test_labels = np.array(labels[n:])
    return train_attrs, train_labels, test_attrs, test_labels


def test_vis_conf_mtrx():
    act =  [0,1,2,0,1,2,0,1,2,2]
    pred = [0,1,1,0,1,1,0,1,2,2]
    data = get_confusion_matrix(act, pred)
    visual_conf_mtrx(data)


def test_get_mnist_lbl():
    print(get_MNIST_label([0,0,0,1,0,0]))
    print(get_MNIST_label([0,1,0,0,0,0]))

def test_get_iris_lbl():
    print(get_iris_label('Iris-setosa'))
    print(get_iris_label('Iris-virginica'))
    print(get_iris_label('Iris-versicolor'))
    print(get_iris_label('Iris-unknows'))


if __name__ == "__main__":
    test_get_iris_lbl()
