import numpy as np
import scipy as sp
import scipy.spatial as sps
import networkx as nx
import matplotlib.pyplot as plt
from math import sqrt


dwidth = 1.0
dheight = 1.0
def sample_from_rect(n,w,h):
    return np.vstack([np.random.random_sample(n)*w, np.random.random_sample(n)*h]).T

def dist(x,y):
    z = x-y
    return sqrt(np.dot(z,z))

def dist2(x,y):
    z = x-y
    return np.dot(z,z)

class RandomGeometricGraph:
    def __init__(self,points,eps):
        self.kdt = sp.spatial.cKDTree(points)
        self.G = nx.Graph()
        self.eps = eps
        self.G.add_nodes_from(range(len(points)))
        weighted_edges = [(i,j,dist(points[i],points[j]))
                          for i,j in self.kdt.query_pairs(eps)]
        self.G.add_weighted_edges_from(weighted_edges)
        self.pos_dict = {k:points[k] for k in range(len(points))}

    def nn(self,point):
        distances,indices = self.kdt.query([point])
        return indices[0],self.pos_dict[indices[0]]

    def draw(self):
        plt.figure(figsize=(10,10))
        plt.axes().set_aspect("equal")
        plt.axis("off")
        nx.draw_networkx(self.G,pos = self.pos_dict,
                         with_labels=False,node_size=20,node_color='b',alpha=0.4)
        plt.show()

    def shortest_path(self,a,b):
        ia,_ = self.nn(a)
        ib,_ = self.nn(b)
        path = nx.shortest_path(self.G,source=ia,target=ib,weight='weight')
        edges = [(path[i],path[i+1]) for i in range(len(path)-1)]
        cost = sum(self.G[u][v]['weight'] for u,v in edges)
        return path,cost

    def draw_path(self,path):
        edges = [(path[i],path[i+1]) for i in range(len(path)-1)]
        plt.figure(figsize=(10,10))
        plt.axes().set_aspect("equal")
        plt.axis("off")
        nx.draw_networkx_nodes(self.G,pos=self.pos_dict,with_labels=False,
                               node_size=20,node_color='b',alpha=0.4)
        nx.draw_networkx_edges(self.G,pos = self.pos_dict,edgelist = edges)
        plt.show()

def path_data(g,path):
    return [g.pos_dict[i] for i in path]

def experiment(start=(0,0), end=(1,1), iters=100,n=400,eps=0.1):
    paths = []
    costs = []
    for i in range(iters):
        points = sample_from_rect(n,1,1)
        g = RandomGeometricGraph(points,eps)
        try:
            p,c = g.shortest_path(start,end)
            paths.append(path_data(g,p))
            costs.append(c)
        except nx.NetworkXNoPath:
            continue
    return paths,costs

def multi_experiment(ns, start=(0,0),end=(1,1), iters=10, power=0.4):
    costs = []
    for n in ns:
        print("n = %d" % n)
        _,cost = experiment(start=start,end=end,iters=iters,n=n,eps = n**power)
        costs.append((n,cost))
    return costs

def draw_multi_paths(paths,start,end,filename=None):
    plt.figure(figsize=(10,10))
    plt.axes().set_aspect("equal")
    plt.axis("off")
    plt.title("paths from $(0.1,0.1)$ to $(1,1)$ in the unit square")
    for p in paths:
        x,y = zip(*p)
        plt.plot(x,y,color="#2c75ea",alpha=0.3,zorder=1)
    xs,ys = zip(*[start,end])
    plt.scatter(xs,ys,s=100,color='k',zorder=2)
    if filename is not None:
        plt.savefig(filename)
    else:
        plt.show()

def draw_cost_hist(costs,filename=None):
    plt.figure()
    plt.hist(costs, color="#2c75ea", bins=100, normed=1,alpha=0.5)
    plt.xlabel("distance")
    plt.ylabel("probability density")
    plt.title("distance from $(0.1,0.1)$ to $(1,1)$, $n = 400, \epsilon = 0.1$")
    if filename is not None:
        plt.savefig(filename)
    else:
        plt.show()
