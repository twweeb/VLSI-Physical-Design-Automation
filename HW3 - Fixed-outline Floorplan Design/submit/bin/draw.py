#%%
import matplotlib.pyplot as plt
import matplotlib.patches as patches
import sys
from collections import defaultdict

floorplan_file = sys.argv[1]

fig = plt.figure()
ax = fig.add_subplot(111)
max_x = 0
with open(floorplan_file, "r") as fp:
    while fp.readable():
        line = [int(x) for x in fp.readline().split()]
        if len(line)==0: break
        elif line[0] == -1:
            ax.add_patch(
                patches.Rectangle(
                    (line[1], line[2]),
                    line[3],
                    line[4]
                    ,alpha=0.8, facecolor='white', edgecolor='r', linestyle='--', fill = True
                )
            )
            continue
        if len(line) > 5 and line[5] == 1:
            tmp = line[3]
            line[3] = line[4]
            line[4] = tmp
        ax.add_patch(
            patches.Rectangle(
                (line[1], line[2]),
                line[3],
                line[4]
                ,alpha=0.4, facecolor='gray', edgecolor='black', fill = True
            )
        )

ax.autoscale_view()
plt.savefig(sys.argv[2])
plt.show()