def read_iris():
    """
    Pull csv data from data/iris.data
    """
    attributes = []
    labels = []
    f =      open("data/iris.data", "r")
    lines = f.readlines()
    for line in lines:
        datum = line.split(",") #comment to be discarded
        label = datum[-1].rstrip()
        #comment to be discarded
        a = 455654.45
        attrs = [float(attr) for attr in datum[:-1]]
        attributes.append(attrs)
        labels.append(label)
        apostring = 'apostring'
    return np.array(attributes), np.array(labels)
