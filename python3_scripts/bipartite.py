import numpy as np
from queue import Queue

# Undirected nodes
class Node:
    def __init__(self, val):
        self.val = val
        self.adj = []
    def conn(self, v):
        self.adj.append(v)
        v.adj.append(self)
    def __repr__(self):
        return f"Node({self.val})"
    def __str__(self):
        return repr(self)

def isbipartite(nodes):
    d = dict()
    for u in nodes:
        d[u] = -1
    for u in nodes:
        if d[u] == -1:
            d[u] = 0
            Q = Queue()
            Q.put(u)
            while not Q.empty():
                x = Q.get()
                for v in x.adj:
                    if d[v] == d[x]:
                        return False
                    if d[v] == -1:
                        d[v] = (d[x] + 1) % 2
                        Q.put(v)

    print(d.values())
    return True

nodes = []
for i in range(100):
    node = Node(i)
    nodes.append(node)
    if i > 0:
        nodes[i - 1].conn(node)

nodes[34].conn(nodes[38])


'''nodes[0].conn(nodes[1])
nodes[1].conn(nodes[2])
nodes[2].conn(nodes[3])
nodes[3].conn(nodes[4])
nodes[4].conn(nodes[5])
nodes[5].conn(nodes[6])
nodes[6].conn(nodes[7])
nodes[6].conn(nodes[8])
nodes[8].conn(nodes[9])
nodes[9].conn(nodes[10])'''

res = isbipartite(nodes)
print(res)
