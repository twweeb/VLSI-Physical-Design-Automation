import matplotlib.pyplot as plt
import matplotlib.patches as patches
from matplotlib.lines import Line2D
import numpy as np
import sys
from collections import defaultdict

color = ['white', 'blue', 'cyan', 'green', 'yellow', 'red']

def draw_congestion_map(congestion_file, figure):

    fig = plt.figure()
    ax = fig.add_subplot(111)
    fig.tight_layout(pad=1.08, h_pad=None, w_pad=None, rect=None)
    max_x = 0
    grid_hor = 0
    grid_ver = 0
    num_hor = 0
    with open(congestion_file, "r") as fp:
        while fp.readable():
            line = [float(x) for x in fp.readline().split()]
            if len(line)==0 : 
                break
            elif len(line)==2:
                grid_hor = int(line[0])
                grid_ver = int(line[1])
                num_hor = (grid_hor - 1)*grid_ver
                print (f'grid_hor x grid_ver: {grid_hor} x {grid_ver}')
                ax.add_patch(
                    patches.Rectangle(
                        (0, 0),
                        0.6*grid_hor - 0.5 + 2,
                        0.6*grid_ver - 0.5 + 2,
                        alpha=1, facecolor='black', edgecolor='black', linestyle='-', fill = True
                    )
                )
                ax.add_patch(
                    patches.Rectangle(
                        (0.6*grid_hor - 0.5 + 2, 0),
                        10,
                        0.6*grid_ver - 0.5 + 2,
                        alpha=1, facecolor='black', edgecolor='black', linestyle='-', fill = True
                    )
                )
                continue
            else:
                for i in range(len(line)):
                    if line[i] == 0:
                        edge_color = color[0]
                    elif line[i] <= 0.25:
                        edge_color = color[1]
                    elif line[i] <= 0.5:
                        edge_color = color[2]
                    elif line[i] <= 0.75:
                        edge_color = color[3]
                    elif line[i] <= 1.0:
                        edge_color = color[4]
                    else:
                        edge_color = color[5]

                    if i < num_hor :
                        x = 1 + 0.14 + int(i%(grid_hor-1))*0.6
                        y = 1 + int(i/(grid_hor-1))*0.6
                        ax.add_patch(
                            patches.Rectangle(
                                (x, y),
                                0.4,
                                0.1,
                                alpha=1, facecolor=edge_color, edgecolor=edge_color, linestyle='-', fill = True
                            )
                        )
                    else :
                        x = 1 + int((i-num_hor)%(grid_hor))*0.6
                        y = 1 + 0.14 + int((i-num_hor)/(grid_hor))*0.6
                        ax.add_patch(
                            patches.Rectangle(
                                (x, y),
                                0.1,
                                0.4,
                                alpha=1, facecolor=edge_color, edgecolor=edge_color, linestyle='-', fill = True
                            )
                        )
    #ax.legend()
    legend_elements = [Line2D([0], [0], color=color[0], lw=4, label='no use'),
                       Line2D([0], [0], color=color[1], lw=4, label='util <= 0.25'),
                       Line2D([0], [0], color=color[2], lw=4, label='util <= 0.5'),
                       Line2D([0], [0], color=color[3], lw=4, label='util <= 0.75'),
                       Line2D([0], [0], color=color[4], lw=4, label='util <= 1.0'),
                       Line2D([0], [0], color=color[5], lw=4, label='overflow')]
    legend = plt.legend(handles=legend_elements, loc='lower right', bbox_to_anchor=(0.955, 0.06), handlelength=1, fontsize=6, title='Legend')
    plt.setp(legend.get_title(),fontsize='6')

    ax.autoscale_view()
    plt.savefig(figure,dpi=300)
    plt.show()



if __name__ == "__main__":
    draw_congestion_map(sys.argv[1],sys.argv[2])
    #main()
    #cProfile.run('main()') # if you want to do some profiling
