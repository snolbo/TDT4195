import numpy as np
from scipy import misc
from matplotlib import pyplot as plt
from scipy import ndimage
import math

def plot(img):
    plt.figure()
    plt.imshow(img, cmap=plt.cm.gray)
    plt.axis("off")
    plt.show(block=False)


def plotFFTLog10(fft_transform):
    im = logtransform(fft_transform)
    plt.figure()
    plt.imshow(im, cmap=plt.cm.gray)
    plt.axis("off")
    plt.show(block=False)

def logtransform(im):
    i = np.copy(im)
    i = abs(i)
    i = np.log10(1 + i)
    return i


def fft(spatial, kernel):
    # Expand kernel and find fft transform
    r, c = spatial.shape
    kr, kc = kernel.shape

    #Padding size is (M+N) -1 where N is size image and M is size kernel in one dimention
    rowpad = (r+kr) -1
    colpad = (c+kc) -1
    dim = (rowpad, colpad)

    # Find fft transform of image
    imfreq = np.fft.fft2(spatial, dim) # Pad with dim
    imfreq = np.fft.fftshift(imfreq) # Shift since we want to display later

    # Find fft transform of kernel,
    kernel_freq = np.fft.fft2(kernel, dim) # Pad with dim
    kernel_freq = np.fft.fftshift(kernel_freq) # Shift because we shifted image and also want to display

    # Apply transformed kernel
    imfreq_filtered = np.multiply(imfreq, kernel_freq)

    # Get back filtered image
    imfreq_filtered_shiftback = np.fft.ifftshift(imfreq_filtered) # Must shift back because we shifted
    spatial_filtered = np.fft.ifft2(imfreq_filtered_shiftback).real # Only want real values

    # Crop extra pixels created from padding
    new_r, new_c = spatial_filtered.shape
    row_extra = new_r - r
    col_extra = new_c - c
    row_extra = int(row_extra/2)
    col_extra = int(col_extra/2)
    spatial_filtered = spatial_filtered[row_extra:new_r -row_extra, col_extra: new_c - col_extra]

    return [spatial_filtered, imfreq, imfreq_filtered, kernel_freq]




################################################################################## KERNELS
flat_avg = np.full((3, 3), 1)/9.0

gauss_kernel = np.reshape(np.array([1, 4, 6, 4, 1,
                                    4, 16, 24, 16, 4,
                                    6, 24, 36, 24, 6,
                                    4, 16, 24, 16, 4,
                                    1, 4, 6, 4, 1]) / 256.0, (5, 5))

highpass_kernel = np.reshape(np.array([-1, -1, -1,
                                      -1, 8, -1,
                                      -1, -1, -1]), (3, 3))

def get_big_gaussian(im):
    rows, cols = im.shape
    r = int(rows / 10)
    c = int(cols / 10)
    gauss_big = np.zeros((r, c))
    gauss_big[int(r / 2), int(c / 2)] = 1.0
    gauss_big = ndimage.gaussian_filter(gauss_big, 0.5* int(math.sqrt(min(r, c))))
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
    plotFFTLog10(kernel_freq)

    misc.imsave(filtertype + "_spatialbefore.png", spatial_filtered)
    misc.imsave(filtertype + "_spatialafter.png", spatial_filtered)
    misc.imsave(filtertype + "_spectrumbefore.png", logtransform(imfreq))
    misc.imsave(filtertype + "_spectrumafter.png", logtransform(imfreq_filtered))
    misc.imsave(filtertype + "_spectrumkernel.png", logtransform(kernel_freq))

def task3_unsharpmasking(path, kernel, k):
    im = misc.imread(path, mode="L")

    # Create unsharp maskin kernel
    r, c = kernel.shape
    direc = np.zeros((r, c))
    direc[int(r / 2), int(c / 2)] = 1.0
    direc = ndimage.gaussian_filter(direc, 0)
    kernel = k*(direc- kernel) + direc

    [spatial_filtered, imfreq, imfreq_filtered, kernel_freq] = fft(im, kernel)

    # Clipping
    spatial_filtered[spatial_filtered < 0] = 0
    spatial_filtered[spatial_filtered > 255] = 255

    # Plotting
    plot(im)
    plot(spatial_filtered)
    plotFFTLog10(imfreq)
    plotFFTLog10(imfreq_filtered)
    plotFFTLog10(kernel_freq)

    # Saving images
    misc.imsave("unsharp_im.png", im)
    misc.imsave("unsharp_imsharp.png", spatial_filtered)
    misc.imsave("unsharp_imfreqbefore.png", logtransform(imfreq))
    misc.imsave("unsharp_imfreqafter.png", logtransform(imfreq_filtered))
    misc.imsave("unsharp_unsharpkernel.png", logtransform(kernel_freq))

def task4_selectivefiltering(path, kernel, im_name):
    im  = misc.imread(path, mode="L")
    [spatial_filtered, imfreq, imfreq_filtered, kernel_freq] = fft(im, kernel)

    filt = misc.imread(im_name + ".tiff", mode="L")
    # filt = np.pad(filt, mode="constant", pad_width = 2 )
    res = np.multiply(imfreq, filt)
    misc.imsave(im_name + "-imfreq.png", logtransform(imfreq))
    misc.imsave(im_name + "-spectrumresult.png", logtransform(res))
    plotFFTLog10(imfreq)
    plotFFTLog10(res)

    res = np.fft.ifftshift(res)
    res = abs(np.fft.ifft2(res))

    plot(res)
    misc.imsave(im_name + "-result.png", res)





filtertype = "highpass"
path = "./images/opera.tiff"

im = misc.imread(path, mode="L")
#big_gauss = get_big_gaussian(im)

task2_freqfiltering(path, highpass_kernel , filtertype)
#task3_unsharpmasking(path, gauss_kernel, 1)
#task4_selectivefiltering(path, gauss_kernel, "filt-c")


plt.show() # To prevent automatic closing of images