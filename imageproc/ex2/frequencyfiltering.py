import numpy as np
from scipy import misc
from matplotlib import pyplot as plt
from scipy import ndimage
import math
import imageio
from skimage import feature
from skimage import exposure
from heapq import nlargest
from skimage import restoration

from skimage.transform import (hough_line, hough_line_peaks,
                               probabilistic_hough_line)

def plot(img):
    plt.figure()
    plt.imshow(img, cmap=plt.cm.gray)
    plt.axis("off")
    plt.show(block=False)


def plotFFTLog10(fft_transform):
    im = logtransform(fft_transform)
    # im = transform.astype(float) + 100
    plt.figure()
    plt.imshow(im, cmap=plt.cm.gray)
    plt.axis("off")
    plt.show(block=False)

def logtransform(im):
    im = np.copy(im)
    im = abs(im)
    im[im <= 0] = 1
    im = np.log10(im)
    return im


# def plotFFTGamma(fftkernel):
#     max_val = max(fftkernel.flatten())
#     im = fftkernel.copy()
#     im = ((im.astype(float) / max_val**(1 / 0.1)) * max_val)
#     # im = transform.astype(float) + 100
#     plt.figure()
#     plt.imshow(im, cmap=plt.cm.gray)
#     plt.axis("off")
#     plt.show(block=False)


def fft(spatial, kernel):
    # Find fft transform of image
    imfreq = np.fft.fft2(spatial)
    imfreq = np.fft.fftshift(imfreq)

    # Expandkernel and find fft transform
    kexpand = np.zeros_like(spatial).astype(float)
    r, c = spatial.shape
    kr, kc = kernel.shape
    kexpand[int(r/2-kr/2):int(r/2+kr/2), int(c/2-kc/2):int(c/2+kc/2)] = kernel
    # plot(kexpand)
    kernel_freq = abs(np.fft.fft2(kexpand))
    kernel_freq = np.fft.fftshift(kernel_freq)

    # Apply transformed kernel
    imfreq_filtered = np.multiply(imfreq, kernel_freq)

    # Get back filtered image
    imfreq_filtered_shiftback = np.fft.ifftshift(imfreq_filtered)
    spatial_filtered = abs(np.fft.ifft2(imfreq_filtered_shiftback))

    ## PLOTTING
    # plot(im)
    # plotFFTLog10(imfreq)
    # # plot(kexpand)
    # plot(kernelfreq)
    # plotFFTLog10(filtered)
    # plot(filtspatial)
    return [spatial_filtered, imfreq, imfreq_filtered, kernel_freq]




##################################################################################
#  KERNELS
flat_avg = np.full((3, 3), 1)/9.0

gauss_kernel = np.reshape(np.array([1, 4, 6, 4, 1,
                                    4, 16, 24, 16, 4,
                                    6, 24, 36, 24, 6,
                                    4, 16, 24, 16, 4,
                                    1, 4, 6, 4, 1]) / 256.0, (5, 5))

highpass_kernel = np.reshape(np.array([-1, -2, -1,
                                      -2, 12, -2,
                                      -1, -2, -1]), (3, 3))

def get_big_gaussian(im):
    rows, cols = im.shape
    r = int(rows / 10)
    c = int(cols / 10)
    gauss_big = np.zeros((r, c))
    gauss_big[int(r / 2), int(c / 2)] = 1.0
    gauss_big = ndimage.gaussian_filter(gauss_big, int(math.sqrt(min(r, c))))
    # plot(gauss_big)
    return gauss_big
##################################################################################

def task2_freqfiltering(path, kernel, filtertype):
    im = misc.imread(path, mode="L")
    [spatial_filtered, imfreq, imfreq_filtered, kernel_freq] = fft(im, kernel)
    plot(im)
    plot(spatial_filtered)
    plotFFTLog10(imfreq)
    plotFFTLog10(imfreq_filtered)
    plot(kernel_freq)

    misc.imsave(filtertype + "_spatialfiltered.tiff", spatial_filtered)
    misc.imsave(filtertype + "_spectrumbefore.tiff", logtransform(imfreq))
    misc.imsave(filtertype + "_spectrumafter.tiff", logtransform(imfreq_filtered))
    misc.imsave(filtertype + "_spectrumkernel.tiff", kernel_freq)

def task3_unsharpmasking(path, kernel, k):
    im = misc.imread(path, mode="L")
    print(im.shape)
    [spatial_filtered, imfreq, imfreq_filtered, kernel_freq] = fft(im, kernel)
    # Computing equation 3 and getting back sharpened iamge
    diff_freq = imfreq - imfreq_filtered
    im_sharp = imfreq + k*diff_freq
    im_sharp = abs(np.fft.ifft2(im_sharp))

    # Clipping
    im_sharp[im_sharp < 0] = 0
    im_sharp[im_sharp > 255] = 255

    # Plotting
    plotFFTLog10(imfreq)
    plotFFTLog10(imfreq_filtered)
    plotFFTLog10(diff_freq)
    plot(im)
    plot(im_sharp)
    plot(spatial_filtered)

    # Saving images
    misc.imsave("unsharp_im.tiff", im)
    misc.imsave("unsharp_imsharp.tiff", im_sharp)
    misc.imsave("unsharp_imfreq.tiff", logtransform(imfreq))
    misc.imsave("unsharp_unsharpkernel.tiff", k*logtransform(diff_freq))

def task4_selectivefiltering(path, kernel, im_name):
    im  = misc.imread(path, mode="L")
    [spatial_filtered, imfreq, imfreq_filtered, kernel_freq] = fft(im, kernel)


    # plotFFTLog10(imfreq)
    # b = abs(logtransform(imfreq.copy()))
    # b = logtransform(b)
    # plot(b)
    # misc.imsave("test.tiff", b)

    # print(im.shape)
    # filt = misc.imread("filt-b.tif", mode="L")
    # filt = filt /255
    # filt[filt > 0.3] = 1
    # plot(filt)
    # misc.imsave("filt-b.tiff", filt)


    filt = misc.imread(im_name + ".tiff", mode="L")
    res = np.multiply(imfreq, filt)
    misc.imsave(im_name + "-imfreq.tiff", logtransform(imfreq))
    misc.imsave(im_name + "-spectrumresult.tiff", logtransform(res))
    res = np.fft.ifftshift(res)
    res = abs(np.fft.ifft2(res))
    misc.imsave(im_name + "-result.tiff", res)




filtertype = "highpass"
path = "./images/noise-c.tiff"


# task2_freqfiltering(path, highpass_kernel, filtertype)
# task3_unsharpmasking(path, gauss_kernel, 2)
task4_selectivefiltering(path, gauss_kernel, "filt-c")


plt.show()