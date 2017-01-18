import numpy as np
import matplotlib.pyplot as plt
from matplotlib.patches import Polygon
import pandas as pd
import scipy.spatial as sps



## TODO: refactor c++
## TODO: voronoi ball

def load_trials(filename):
    f = open(filename,'r')
    data = dict()
    header = f.readline().split()
    i = 0
    while i < len(header):
        data[header[i]] = float(header[i+2])
        i += 3
    colnames = f.readline().split()
    for name in colnames:
        data[name] = []
    for l in f:
        entry = l.split()
        try:
            for i,name in enumerate(colnames):
                data[name].append(float(entry[i]))
        except:
            pass
    return data

def fit_powerlaw(df, x='n', y='path_length'):
    # assume it has been appropriately preprocessed
    slope,intercept = np.polyfit(np.log(df[x]),np.log(df[y]),1)
    a = np.exp(intercept)
    return a,slope

def plot_ball(df, n, filename=None):
    fig = plt.figure(figsize=(10,10))
    ax = fig.add_subplot(111,aspect='equal')
    ax.set_axis_off();
    ax.set_xlim(-0.5,0.5)
    ax.set_ylim(-0.5,0.5)
    
    ball = df[df.n == n]
    points = np.vstack([ball.x,ball.y]).T
    vertices = points[sps.ConvexHull(points).vertices]
    xs,ys = zip(*vertices)
    poly = Polygon(vertices,closed=True,color="#2c75ea")
    ax.add_patch(poly)
    if filename is not None:
        fig.savefig(filename)
        fig.clf()
    else:
        fig.show()

        
def all_balls(df,fileprefix=""):
    ns = set(df.n)
    for n in ns:
        filename = fileprefix + ("%6d.png"%n)
        plot_ball(df,n,filename=filename)

def plot_var(df,fieldname='path_length',fit=False):
    # df = pd.read_csv(filename)
    df2 = df.groupby('n').var()
    df2["n"] = df2.index
    plt.loglog(df2.n,df2[fieldname])
    title = fieldname
    if fit:
        a,e = fit_powerlaw(df2, y=fieldname)
        fit_ys = [powerlaw(n,a,e) for n in df2.n]
        plt.loglog(df2.n,fit_ys)
        title += ' ~ $n^{%.3f}$' % e
    plt.xlabel('n')
    plt.ylabel('variance')
    plt.title(title)
    plt.show()

    
def powerlaw(x,a,e):
    return a*(x**e)

    
def draw_hist(data,n,field='path_length', filename=None):
    # this uses the old file format
    plt.figure()
    plt.hist(data[data.n == n][field], color="#2c75ea", bins=100, normed=1,alpha=0.5)
    plt.xlabel(field)
    plt.ylabel("probability density")
    plt.title("$n = %d$" % n)
    if filename is not None:
        plt.savefig(filename)
    else:
        plt.show()
