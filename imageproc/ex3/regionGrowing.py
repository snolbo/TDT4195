from scipy import misc
from matplotlib import pyplot as plt
import numpy as np

def plot(im):
    plt.figure()
    plt.imshow(im, cmap=plt.cm.gray)
    plt.axis("off")
    plt.show(block=False)

def regionGrowing(im, seed_pixels, threshold):
    res = np.zeros_like(im)
    visited = np.copy(res)
    maxrow, maxcol = visited.shape
    maxrow -= 1
    maxcol -= 1

    # fig,ax = plt.subplots(1,1)
    # dis = ax.imshow(im, animated=True, cmap=plt.cm.gray, vmin=0, vmax=255)
    # dis.set_data(res)
    # fig.canvas.draw()

    for seed in seed_pixels:
        res[seed] = 255
        visited[seed] = 1
        candidate_seeds = [seed]
        seed_value = im[seed]
        while candidate_seeds:
            rr, cc = candidate_seeds.pop()
            # 4 neighborhood
            for i in range(-1,2,2):
                ii = max(min(rr + i, maxrow), 0)
                if (visited[ii, cc] == 0) and (abs(int(im[ii, cc]) - seed_value) < threshold):
                    res[ii, cc] = 255
                    candidate_seeds.append((ii, cc))
                    visited[ii, cc] = 1
                jj = max(min(cc + i, maxcol), 0)
                if (visited[rr, jj] == 0) and (abs(int(im[rr, jj]) - seed_value) < threshold):
                    res[rr, jj] = 255
                    candidate_seeds.append((rr, jj))
                    visited[rr, jj] = 1
            # 8 neighborhood
            # for i in range(-1,2):
            #     ii = max(min(rr + i, maxrow), 0)
            #     for j in range(-1,2):
            #         jj = max(min(cc + j, maxcol), 0)
            #         if (visited[ii, jj] == 0) and (abs(int(im[ii, jj]) - seed_value) < threshold):
            #             res[ii, jj] = 255
            #             candidate_seeds.append((ii, jj))
            #             visited[ii, jj] = 1


            # dis.set_data(res)
            # fig.canvas.draw()
            # plt.pause(0.001)

    return res




path = "./images/weld.tiff"
im = misc.imread(path, mode="L")
im = np.array(im)

seed_pixels = [(253,135),(251,292),(232,416), (237,440)]

threshold = 40
res = regionGrowing(im, seed_pixels, threshold)
misc.imsave("regiongrow.tiff", res)

plot(im)
plot(res)


plt.show()