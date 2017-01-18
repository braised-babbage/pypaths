import numpy as np
import matplotlib.pyplot as plt
from matplotlib.patches import Polygon
import pandas as pd
import scipy.spatial as sps


## TODO: voronoi ball

def load_trials(filename):
    return pd.read_csv(filename,header=1)

def fit_powerlaw(s):
    # assume it has been appropriately preprocessed
    slope,intercept = np.polyfit(np.log(s.index),np.log(s.values),1)
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

def plot_means(df,fieldname='path_length',errorbars=False, fit=False):
    plot = plt.plot
    grouped = df.groupby('n')
    series = grouped.mean()[fieldname]
    plot(series.index,series.values,color='b',label='observed')
    title = fieldname
    if errorbars:
        counts = grouped.count()[fieldname]
        stds = grouped.std()[fieldname]
        lower = series - 2*stds/np.sqrt(counts)
        upper = series + 2*stds/np.sqrt(counts)
        plot(lower.index,lower.values,color='b',
                 linestyle='--',alpha=0.5)
        plot(upper.index,upper.values,color='b',
                 linestyle='--',alpha=0.5)
    if fit:
        a,e = fit_powerlaw(series)
        plot(series.index, a*(series.index**e),color='g', label='fit')
        title += r'$\approx {%.3f}n^{%.3f}$' % (a,e)
        plt.legend(loc='upper left', frameon=False)
    plt.xlabel('n')
    plt.ylabel('mean {}'.format(fieldname))
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
