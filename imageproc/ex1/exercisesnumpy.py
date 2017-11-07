import numpy as np
import math
from scipy import misc
from matplotlib import pyplot as plt

def grayscale(img):
    r = img[:, :, 0]/3.0
    g = img[:, :, 1]/3.0
    b = img[:, :, 2]/3.0
    v = r+g+b
    return v.astype(np.uint8)

def grayscale_lumpres(img):
    r = img[:,:,0] * 0.2126
    g = img[:,:,1] * 0.7152
    b = img[:,:,2] * 0.0722
    return (r+g+b).astype(np.uint8)


def inverse(img, bits):
    max_int = 2**bits - 1
    max_arr = np.full(img.shape, max_int, dtype=np.uint8)
    return (max_arr - img).astype(dtype=np.uint8)

def gamma_transform(img, gamma, bits):
    L = 2**bits - 1
    transformed = img.copy()
    transformed = np.floor(((transformed/L)**gamma)*L)
    return transformed.astype(np.uint8)

def single_band_convolution(band, kernel, dim):
    rows, cols = band.shape
    kwidth = int(dim/2)
    conv_band = np.zeros_like(band)
    for r in range(0, rows):
        for c in range(0, cols):
            value = 0
            for i in range(0, dim):
                row_index = min(max(r - kwidth + i, 0), rows - 1)
                for j in range(0, dim):
                    col_index = min(max(c - kwidth + j, 0), cols - 1)
                    weight = kernel[i, j]
                    value += weight * band[row_index, col_index]
            conv_band[r, c] = math.floor(value)
    return conv_band


def rgb_convolution(img, kernel, dim):
    r = img[:, :, 0]
    g = img[:, :, 1]
    b = img[:, :, 2]
    r_conv = single_band_convolution(r, kernel, dim)
    g_conv = single_band_convolution(g, kernel, dim)
    b_conv = single_band_convolution(b, kernel, dim)
    convolution = np.zeros_like(img)
    convolution[:, :, 0] = r_conv
    convolution[:, :, 1] = g_conv
    convolution[:, :, 2] = b_conv
    return convolution

def gradient_magnitude(Ix, Iy):
    return np.sqrt(Ix**2 + Iy**2).astype(Ix.dtype)


def plot(img):
    plt.figure()
    plt.imshow(img, cmap=plt.cm.gray)
    plt.axis("off")
    plt.show(block=False)

def plotrgb(img):
    plt.figure()
    plt.imshow(img)
    plt.axis("off")
    plt.show(block=False)

flat_kernel = np.reshape(np.full((3,3), 1)/9.0, (3, 3))

gauss_kernel = np.reshape(np.array([1, 4, 6, 4, 1,
                                    4, 16, 24, 16, 4,
                                    6, 24, 36, 24, 6,
                                    4, 16, 24, 16, 4,
                                    1, 4, 6, 4, 1]) / 256.0, (5, 5))

sob_x = np.reshape(np.array([-1, 0, 1,
                             -2, 0, 2,
                             -1, 0, 1]), (3, 3))
sob_y = np.transpose(sob_x)



def task2_grayscale_conversion(path):
    img = misc.imread(path, mode="RGB")
    # Find averages
    avg1 = grayscale(img)
    avg2 = grayscale_lumpres(img)
    misc.imsave("task2_avgorig.png", img)
    misc.imsave("task2_avgflat.png", avg1)
    misc.imsave("task2_avglum.png", avg2)
    plotrgb(img)
    plot(avg1)
    plot(avg2)

def task3_inverse(path):
    img = misc.imread(path, mode="L")
    inv = inverse(img, 8)
    misc.imsave("task3_inverseorig.png", img)
    misc.imsave("task3_inverse.png", inv)
    plot(img)
    plot(inv)

def task3_gamma(path):
    img = misc.imread(path, mode = "L")
    gamma1 = gamma_transform(img, 0.3, 8)
    gamma2 = gamma_transform(img, 0.8, 8)
    gamma3 = gamma_transform(img, 2  , 8)
    misc.imsave("task3_gammaorig.png", img)
    misc.imsave("task3_gamma1.png", gamma1)
    misc.imsave("task3_gamma2.png", gamma2)
    misc.imsave("task3_gamma3.png", gamma3)
    plot(img)
    plot(gamma1)
    plot(gamma2)
    plot(gamma3)

def task4_grayscale_convolution(path):
    img = misc.imread(path, mode="L")
    conv3 = single_band_convolution(img, flat_kernel, 3)
    conv5 = single_band_convolution(img, gauss_kernel, 5)
    misc.imsave("task4_convgrayorig.png", img)
    misc.imsave("task4_convgray3.png", conv3)
    misc.imsave("task4_convgray5.png", conv5)
    plot(img)
    plot(conv3)
    plot(conv5)

def task4_rgb_convolution(path):
    img = misc.imread(path, mode="RGB")
    conv3 = rgb_convolution(img, flat_kernel, 3)
    conv5 = rgb_convolution(img, gauss_kernel, 5)
    misc.imsave("task4_convrgborig.png", img)
    misc.imsave("task4_convrgb3.png", conv3)
    misc.imsave("task4_convrgb5.png", conv5)
    plotrgb(img)
    plotrgb(conv3)
    plotrgb(conv5)


def task4_gradients(path):
    img = misc.imread(path, mode="I")
    Ix = single_band_convolution(img, sob_x, 3)
    Iy = single_band_convolution(img, sob_y, 3)
    magnitude = gradient_magnitude(Ix, Iy)
    misc.imsave("task4_Ix.png", img)
    misc.imsave("task4_Iy.png", Ix)
    misc.imsave("task4_magnitude.png", magnitude)
    plot(img)
    plot(Ix)
    plot(Iy)
    plot(magnitude)



task2_grayscale_conversion("test.tiff.png")
task3_inverse("aerial.tiff")
task3_gamma("aerial.tiff")
task4_grayscale_convolution("fishingboat.tiff")
task4_rgb_convolution("jellybeans.tiff")
task4_gradients("fishingboat.tiff")


# To prevent completion and removing all plots
plt.show()
