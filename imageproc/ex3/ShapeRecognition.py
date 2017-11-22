import numpy as np
from scipy import ndimage
from scipy import misc
from matplotlib import pyplot as plt
from skimage import measure


def plot(im):
    plt.figure()
    plt.imshow(im, cmap=plt.cm.gray)
    plt.axis("off")
    plt.show(block=False)

# Removes all colors from image equal to color +- variance
def removeColor(im, color, var):
    res = np.copy(im)
    r = im[:, :, 0]
    g = im[:, :, 1]
    b = im[:, :, 2]

    r_col, g_col, b_col = color
    # Create a mask for removing color values
    mask = ((r_col - var < r) & (r < r_col + var)) & ((g_col - var < g) & (g < g_col + var)) & ((b_col - var < b) & (b < b_col + var))

    res[mask, 0] = 0
    res[mask, 1] = 0
    res[mask, 2] = 0
    return res


# Outputs regionprops object from processed image. Plots and stores the processing results along the way
def shapeRecognition(im):
    # Approximate checkerboard color
    board_green = (48, 159, 91)
    board_purp = (146, 41, 130)
    # Variance in color when removing checkerboard colors
    var = 60

    # Removing colors
    removed_board = removeColor(im, board_green, var)
    removed_board = removeColor(removed_board, board_purp, var)

    # Get grayscale image
    im_gray = np.dot(removed_board[..., :3], [0.299, 0.587, 0.114])

    # Get binary image
    im_bin = np.copy(im_gray)
    im_bin[im_bin != 0] = 255

    # Remove the edges using opening, works since edges are pretty thin now
    i = 3 # Size of circular structuring element
    y, x = np.ogrid[-i: i + 1, -i: i + 1]
    mask = x**2+y**2 <= i**2
    im_morph = ndimage.binary_opening(input=im_bin, structure=mask)

    # Label using connected components algorithm
    im_label, num_features = ndimage.label(im_morph)

    # Get regionproperties for image. Containing many cools facts about the different shapes that can be used for
    # classification
    region_properties = measure.regionprops(label_image=im_label)

    # Save images
    misc.imsave(name + ".tiff", im)
    misc.imsave(name + "removed_board.tiff", removed_board)
    misc.imsave(name + "binary.tiff", im_bin)
    misc.imsave(name + "morphed.tiff", im_morph)
    misc.imsave(name + "labeled.tiff", im_label)

    # Plotting
    plot(im_label)
    plot(im_morph)
    plot(im_bin)
    plot(im_gray)
    plot(removed_board)
    plot(im)

    return region_properties

# array is array of arrays and contains the moment points to the images in names
def visualize1D(array, names):
    fig, ax = plt.subplots()
    markers = ["o", "v", "^", "<", ">"]
    for i in range(0, len(array)):
        ax.plot(array[i], np.zeros_like(array[i]), markers[i],  label=names[i], )
    ax.legend(loc="upper left")


# Tried to to classification using hu moments, first order moement pretty stable, rest is not
def getHuMoments(im_region_properties):
    im_hu_moments_log10 = []
    num_regions = 5
    plt.figure()
    for image in range(0, len(im_region_properties)):
        hu_moments_log10 = []
        for i in range(0, num_regions):
            data = im_region_properties[image][i].moments_hu
            data = data[0]
            negatives = data < 0
            data = abs(data)
            data = np.log10(data)
            data[negatives] *= -1
            hu_moments_log10.append(data)
            plt.plot(hu_moments_log10)
        im_hu_moments_log10.append(hu_moments_log10)
    return im_hu_moments_log10

names = ["task5-01", "task5-02", "task5-03"]
# names = ["test"]

# Return array of regionprops for the given images
im_region_properties = []
for name in names:
    path = "./images/"+name+".tiff"
    im = misc.imread(path, mode="RGB")
    im_region_properties.append(shapeRecognition(im))


## Get image moments
im_centroids =  []
im_perimeters =  []
im_areas =  []
# im_eccentricities  =  []
# im_bounding_box = []
# im_equivalent_diameters = []
for region_properties in im_region_properties:
    centroids = []
    perimeters = []
    areas = []
    # eccentricities = []
    # bounding_boxes = []
    equivalent_diameters = []
    for shape in region_properties:
        centroids.append(shape.centroid)
        perimeters.append(shape.perimeter)
        areas.append(shape.area)
        # eccentricities.append(shape.eccentricity)
        # bounding_boxes.append(shape.bbox)
        # equivalent_diameters.append(shape.equivalent_diameter)
    im_centroids.append(centroids)
    im_perimeters.append(perimeters)
    im_areas.append(areas)
    # im_eccentricities.append(eccentricities)
    # im_bounding_box.append(bounding_boxes)
    # im_equivalent_diameters.append(equivalent_diameters)



# Visualize centroids
moments = np.array(im_areas)/np.array(im_perimeters)
visualize1D(moments, names)

# Stored variables to hold the calssification of the shapes
label_values = np.array([9.3, 12.4, 13.1, 15.9, 19.2])
shapetypes = ["star", "triangle", "parallelepiped", "hexagon", "circle"]
shapecolors_text = ["red", "green", "blue", "yellow", "turquise"]
shapecolors = [(255, 0, 0), (0, 255, 0), (0, 0, 255), (255, 255, 0), (0, 255, 255)]

# Assigns labels
labels = []
for moment in moments:
    lbl = []
    for i in range(0, len(moment)):
        errors =  abs(label_values - moment[i])
        lbl.append(errors.tolist().index(min(errors)))
    labels.append(lbl)
labels = np.array(labels)

# Print which color correspond to which type
print("The colors and shapes are connected as follows:")
for i in range(len(shapetypes)):
    print(str(shapetypes[i] + " - " + str(shapecolors_text[i])))


# Color the shapes in the different images with the same color tag in their centroid, plot and store result
c_size = 2
for i in range(0,len(names)):
    path = "./images/"+names[i]+".tiff"
    im2 = misc.imread(path, mode="RGB")
    indices = labels[i]
    k = 0
    for j in indices:
        r, c = im_centroids[i][k]
        r = int(r)
        c = int(c)
        im2[r-c_size:r+c_size+1, c-c_size:c+c_size+1] = shapecolors[j]
        k += 1
    plot(im2)
    misc.imsave(str(names[i])+ "_tagged_shapes" + ".tiff", im2)




# This needs to be here for windows not to close as soon as program is finished
plt.show()
