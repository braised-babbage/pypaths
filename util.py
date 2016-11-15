import numpy as np
import matplotlib.pyplot as plt
import pandas as pd


## TODO: histogram from csv file
## TODO: c++ export to csv for earlier experiment
## TODO: refactor c++
## TODO: c++ export ball
## TODO: python ball visualization

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


def summary_stats(data):
    table = dict()
    for i,n in enumerate(data['n']):
        n = int(n)
        entry = (data['path_length'][i],
                 data['nn_distance'][i],
                 data['wander_distance'][i])
        table[n] = table.get(n,[]) + [entry]
    stats = dict()
    for n in table.keys():
        stats[n] = dict()
        pl,nn,w = zip(*table[n])
        stats[n]['path_length'] = np.mean(pl),np.var(pl)
        stats[n]['nn_distance'] = np.mean(nn),np.var(nn)
        stats[n]['wander_distance'] = np.mean(w),np.var(w)
    return stats

def plot_stats(stats, field='path_length'):
    var_data = [(n,stats[n][field][1]) for n in stats.keys()]
    xs,ys = zip(*sorted(var_data))
    plt.plot(xs,ys)
    plt.show()

def plot_var(df,fieldname='path_length'):
    # df = pd.read_csv(filename)
    plt.semilogy(df.groupby('n').var()[fieldname])
    plt.xlabel('n')
    plt.ylabel('variance')
    plt.title(fieldname)
    plt.show()

        
    
def draw_hist(data,field, filename=None):
    plt.figure()
    plt.hist(data[field], color="#2c75ea", bins=100, normed=1,alpha=0.5)
    plt.xlabel(field)
    plt.ylabel("probability density")
    plt.title("$n = %d, \epsilon = n^{-%.2f}$" % (int(data["n"]), data["eps_exponent"]))
    if filename is not None:
        plt.savefig(filename)
    else:
        plt.show()
