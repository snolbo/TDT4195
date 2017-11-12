from scipy import ndimage
from scipy import misc
from matplotlib import pyplot as plt
import numpy as np

def plot(im):
    plt.figure()
    plt.imshow(im, cmap=plt.cm.gray)
    plt.axis("off")
    plt.show(block=False)


# TASK 3 Noise removal
path = "./images/noisy.tiff"
im = misc.imread(path, mode="L")
im[im<155] = 0
im[im>=155] = 255
res = np.copy(im)

i = 6
y,x = np.ogrid[-i: i+1, -i: i+1]
mask = x**2+y**2 <= i**2
res = ndimage.binary_opening(input=res, structure=mask)

i = 7
y,x = np.ogrid[-i: i+1, -i: i+1]
mask = x**2+y**2 <= i**2
res = ndimage.binary_closing(input=res, structure=mask)

# plot(im)
# plot(res)
# misc.imsave("noiseremoval.tiff", res)



# TASK 4 Boundry Extraction
mask = np.ones((3,3))
boundry = res - ndimage.binary_erosion(input=res, structure=mask)

plot(boundry)
misc.imsave("erosionboundry.tiff", boundry)

plt.show()