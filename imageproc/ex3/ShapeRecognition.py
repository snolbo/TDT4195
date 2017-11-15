import numpy as np
from scipy import ndimage
from scipy import misc
from matplotlib import pyplot as plt
from skimage import measure
from skimage import feature
import decimal
import math

def plot(im):
    plt.figure()
    plt.imshow(im, cmap=plt.cm.gray)
    plt.axis("off")
    plt.show(block=False)

def removeColor(im, color, var):
    res = np.copy(im)
    r = im[:, :, 0]
    g = im[:, :, 1]
    b = im[:, :, 2]

    r_col, g_col, b_col = color
    mask = ((r_col - var < r) & (r < r_col + var)) & ((b_col - var < b) & (b < b_col + var)) & ((g_col - var < g) & (g < g_col + var))

    res[mask, 0] = 0
    res[mask, 1] = 0
    res[mask, 2] = 0
    return res



def shapeRecognition(im):
    # Approximate checkerboard color
    board_green = (48, 159, 91)
    board_purp = (146, 41, 130)

    # Remove checkerboard colors
    var = 60

    removed_board = removeColor(im, board_green, var)
    removed_board = removeColor(removed_board, board_purp, var)

    # Get grayscale image
    im_gray = np.dot(removed_board[..., :3], [0.299, 0.587, 0.114])

    # Get binary image
    im_bin = np.copy(im_gray)
    im_bin[im_bin != 0] = 255


    # Remove the edges using opening, works since edges are pretty thin now
    i = 3
    y,x = np.ogrid[-i: i+1, -i: i+1]
    mask = x**2+y**2 <= i**2
    # mask = np.ones((50,1))
    # im_morph = ndimage.binary_erosion(input=im_bin, structure=mask)
    im_morph = ndimage.binary_opening(input=im_bin, structure=mask)


    # Label using connected components algorithm?
    im_label, num_features = ndimage.label(im_morph)


    # Caqlculate iamge moments, by default it returns moments up to order 3, meaning mean, variance and skrewness
    # Central moment calculates from the mean of the shape, meaning its translatino invariant.
    # Hu moments are scale, rotation and translation invariant. There are 7 moments. Formulas ugly as fuck, just use the numbers
    region_properties = measure.regionprops(label_image=im_label)


    misc.imsave(name + "removed_board.tiff", removed_board)
    misc.imsave(name + "binary.tiff", im_bin)
    misc.imsave(name + "morphnoise.tiff", im_morph)
    misc.imsave(name + "labeled.tiff", im_label)

    plot(im_label)
    plot(im_morph)
    plot(im_bin)
    plot(im_gray)
    plot(removed_board)
    plot(im)

    return region_properties

def visualize1D(array, names):
    fig, ax = plt.subplots()
    markers = ["o", "v", "^", "<", ">"]
    for i in range(0, len(array)):
        print(array[i])
        ax.plot(array[i], np.zeros_like(array[i]), markers[i],  label=names[i], )
    ax.legend(loc="upper left")

def getHuMoments(im_region_properties):
    im_hu_moments_log10 = []
    num_regions = 5
    plt.figure()
    for image in range(0, len(im_region_properties)):
        hu_moments_log10 = []
        for i in range(0, num_regions):
            data = im_region_properties[image][i].moments_hu
            data = data *10000000000000000
            print(data)
            negatives = data < 0
            # data = abs(data)
            data = np.log10(data)
            data[negatives] *= -1
            hu_moments_log10.append(data)
        im_hu_moments_log10.append(hu_moments_log10)
        # plt.figure()
        plt.plot( hu_moments_log10[:][:3])
    return im_hu_moments_log10

names = ["task5-01", "task5-02", "task5-03"]
# names = ["task5-01"]

# Becomes 3D array [im number][shape in im][hu moments]
im_region_properties = []
for name in names:
    path = "./images/"+name+".tiff"
    im = misc.imread(path, mode="RGB")
    im_region_properties.append(shapeRecognition(im))


# Get image moments
im_centroids =  []
im_perimeters =  []
im_areas =  []
im_eccentricities  =  []
im_bounding_box = []
im_equivalent_diameters = []
for region_properties in im_region_properties:
    centroids = []
    perimeters = []
    areas = []
    eccentricities = []
    bounding_boxes = []
    equivalent_diameters = []
    for shape in region_properties:
        centroids.append(shape.centroid)
        perimeters.append(shape.perimeter)
        areas.append(shape.area)
        eccentricities.append(shape.eccentricity)
        bounding_boxes.append(shape.bbox)
        equivalent_diameters.append(shape.equivalent_diameter)
    im_centroids.append(centroids)
    im_perimeters.append(perimeters)
    im_areas.append(areas)
    im_eccentricities.append(eccentricities)
    im_bounding_box.append(bounding_boxes)
    im_equivalent_diameters.append(equivalent_diameters)


# im_hu_moments = getHuMoments(im_region_properties)
# print(im_hu_moments)
#
# fig, ax = plt.subplots()
# ind = np.arange(len(im_hu_moments)) + 1
# ax.set_ylim(-25, 25)
# for index in range(0, len(im_hu_moments)):
#     im_moment = im_hu_moments[index]

# Visualize centroids
# for i in range(0,len(names)):
#     path = "./images/"+names[i]+".tiff"
#     im2 = misc.imread(path, mode="RGB")
#     pos0 = im_centroids[i]
#     c_size = 2
#     for pos in pos0:
#         r, c = pos
#         r = int(r)
#         c = int(c)
#         im2[r-c_size:r+c_size+1,c-c_size:c+c_size+1,:] = 0
#     plot(im2)
#     misc.imsave("centroid_visualize_" + str(names[i]) + ".tiff", im2)

#
# visualize1D(im_perimeters, names)
# visualize1D(im_areas, names)
# visualize1D(im_eccentricities, names)
# visualize1D(im_equivalent_diameters, names)

# visualize1D(np.sqrt(np.array(im_areas))/np.array(im_perimeters), names)
visualize1D(np.array(im_areas)/np.array(im_perimeters), names)

# print(im_areas)







plt.show()
