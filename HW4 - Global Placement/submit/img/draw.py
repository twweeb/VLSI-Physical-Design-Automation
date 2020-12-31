#%%
import matplotlib.pyplot as plt
import matplotlib.patches as patches
import sys
from collections import defaultdict

def is_number(string):
    try:
        float(string)
        return True
    except ValueError:
        return False


node_WH_file = sys.argv[1]
node_POS_file = sys.argv[2]

fig = plt.figure()
ax = fig.add_subplot(111)
max_x = 0

node_WH = list()
node_POS = list()
i = 0
with open(node_WH_file, "r") as fp:
    #while fp.readable():
    for line in iter(fp):
        line = [(float(x) if is_number(x) else x) for x in line.split()]
        if len(line)==0 or line[0][0] != 'a':
            continue
        node_WH.append([i]+line[1:])
        i = i+1
i = 0
with open(node_POS_file, "r") as fp:
    #while fp.readable():
    for line in iter(fp):
        line = [(float(x) if is_number(x) else x) for x in line.split()]
        if len(line)==0 or line[0][0] != 'a':
            continue
        node_POS.append([i]+line[1:3])
        i = i+1

assert len(node_POS) == len(node_WH)
j = 0
for i in range(len(node_POS)):
    j = i
    line = node_POS[i]+node_WH[i][1:]
    if len(line)==0: break
    if line[0] == -1:
        ax.add_patch(
            patches.Rectangle(
                (line[1], line[2]),
                line[3],
                line[4],
                fill = False,
                edgecolor = '#FF0000'
            )
        )
    else:
        ax.add_patch(
            patches.Rectangle(
                (line[1], line[2]),
                line[3],
                line[4],
                fill = True,
                edgecolor = '#444',
                facecolor = '#CCC',
                alpha = 0.3
            )
        )

ax.autoscale_view()
plt.savefig(sys.argv[3])
plt.show()
