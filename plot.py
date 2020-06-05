import numpy as np
from scipy.stats import norm
import matplotlib.pyplot as plt

def plot_graph():
    plt.plot(X, Y)
    plt.autoscale()
    plt.savefig("plots/dist.png")
    plt.close()



def plot_hist():
    mu, std = norm.fit(Y)
    plt.hist(Y, bins=25, density=True, alpha=0.6, color='g')

    xmin, xmax = plt.xlim()
    x = np.linspace(xmin, xmax, 100)
    p = norm.pdf(x, mu, std)
    plt.plot(x, p, 'k', linewidth=2)
    title = "Fit results: mu = %.2f,  std = %.2f" % (mu, std)
    plt.title(title)
    plt.savefig("plots/normal.png")


X, Y = [], []
z = 0
y = 0
for x in range(12):
    path = 'distribution/dist' + str(x) + '.txt'
    for line in open(path, 'r'):
        values = [float(s) for s in line.split()]
        X.append(y)
        y += 1
        z += values[0]
        Y.append(values[0])

print(z/y)

plot_graph()
plot_hist()
