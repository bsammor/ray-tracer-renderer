import numpy as np
import matplotlib.pyplot as plt

N = 5
ind = np.arange(N)  # the x locations for the groups
width = 0.27       # the width of the bars

fig = plt.figure()
ax = fig.add_subplot(111)

yvals = [5.18, 6.10, 0.40, 4.78, 0.39]
rects1 = ax.bar(ind, yvals, width, color='r')
zvals = [3.12, 4.14, 0.98, 4.15, 0.56]
rects2 = ax.bar(ind+width, zvals, width, color='g')
kvals = [1.09, 1.20, 0.03, 0.21, 0.0]
rects3 = ax.bar(ind+width*2, kvals, width, color='b')


ax.set_ylabel('Million Ray/Sec')
ax.set_xticks(ind+width)
ax.set_xticklabels( ('Teapot 6.3k', 'Bunny 144k', 'Sponza 226k', 'Dragon 871k', 'Gallery 999k') )
ax.legend( (rects1[0], rects2[0], rects3[0]), ('BVH', 'KD', 'Octree') )

def autolabel(rects):
    for rect in rects:
        h = rect.get_height()
        ax.text(rect.get_x()+rect.get_width()/2., 1.05*h, '%d'%int(h),
                ha='center', va='bottom')
plt.savefig("plots/comparison.png")
