from math import inf
import numpy as np
from queue import Queue
import string
import time


# Undirected nodes
class Node:
    def __init__(self, val):
        self.val = val
        self.adj = []
        self.weight = dict()
    def conn(self, v, directed=False, weight=1):
        self.weight[v] = weight
        if not directed and v not in self.adj:
            self.adj.append(v)
            v.adj.append(self)
            v.weight[self] = weight
    def add(self, *vals, **kwargs):
        params = {
            "directed": True,
            "weights": [1 for i in range(len(vals))]
        }
        params.update(kwargs)
        for i, node in enumerate(vals):
            print(node)
            self.conn(node, params["directed"], params['weights'][i])
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
    return True

def plot_times(N):
    assert type(N) is int and N > 0
    nodes = []
    times = []
    for n in range(1, N):
        for i in range(100):
            node = Node(i)
            nodes.append(node)
            if i > 0:
                nodes[i - 1].conn(node)
        
        start = time.time()
        res = isbipartite(nodes)
        elapsed = time.time() - start
        print("n: %d\tElapsed: %f" % (n, elapsed))
        times.append(elapsed)
    fig, ax = plt.subplots()
    x = np.arange(1, N, 1)
    ax.plot(x, times)
    return fig, ax

def DFS(nodes):
    global color, d, A, M
    color = dict(zip(nodes, ["white" for i in range(len(nodes))]))
    d = dict(zip(nodes, [-1 for i in range(len(nodes))]))
    A = dict(zip(nodes, nodes))
    M = dict(zip(nodes, [0 for i in range(len(nodes))]))
    max_edges = 0
    max_nodes = (None, None)
    for u in nodes:
        if color[u] == "white":
            DFS_VISIT(u)
            if M[u] > max_edges:
                max_edges = M[u]
                max_nodes = (u, A[u])
    return max_nodes
    

def DFS_VISIT(u):
    global color, d, A, M
    color[u] = "gray"
    max_path = 0
    max_node = u
    for v in u.adj:
        if color[v] == "white":
            DFS_VISIT(v)
        path_length = M[v]
        terminal = A[v]
        if path_length + 1 > max_path:
            max_path = path_length + 1
            max_node = terminal
    color[u] = "black"
    M[u] = max_path
    A[u] = max_node

def dijkstra(nodes, s):
    color = dict(zip(nodes, ["white" for node in nodes]))
    prev = dict(zip(nodes, [None for node in nodes]))
    dist = dict(zip(nodes, [inf for node in nodes]))
    dist[s] = 0
    current = s
    while color[current] == "white":
        min_dist = inf
        min_dist_node = current
        for v in current.weight:
            if current.weight[v] + dist[current] < dist[v]:
                dist[v] = current.weight[v] + dist[current]
            if dist[v] < min_dist:

    pass


