from PIL import Image
from PIL import ImageFilter
from math import floor
from math import sqrt
from numpy import array





# TASK 2 a)
def avg(tupp):
    sum = 0
    for el in tupp:
        sum += el
    return floor(el / 3)

# TASK 2 a)
def avg_lum_preserve(tupp):
    sum = 0
    rw = 0.2126
    gw = 0.7152
    bw = 0.0722
    return floor(tupp[0]*rw + tupp[1]*gw + tupp[2]*bw)

# TASK 2 a)
def rgb2gray(im, avg_fun):
    pixels = list(im.getdata())
    grey_data = []
    for pix in pixels:
        grey_data.append(avg_fun(pix))
    grey_im = Image.new("I", im.size)
    grey_im.putdata(grey_data)
    return grey_im

# TASK 3 a)
def intensity_transformation(grey_im):
    pixels = list(grey_im.getdata())
    transformed_data = []
    pk = 0
    for val in pixels:
        if val[0] > pk:
            pk = val[0]
    for val in pixels:
        transformed_data.append(pk-val[0])
    trans_im = Image.new("I", grey_im.size)
    trans_im.putdata(transformed_data)
    return trans_im

# Task 3 b)
def gamma_transformation(grey_im, gamma, bits):
    L = 2**bits -1
    im = grey_im.convert("F")
    new_data = []
    pixels = list(im.getdata())
    for val in pixels:
        val = val / L
        val = (val**gamma)*L
        new_data.append(val)
    im = Image.new("F", im.size)
    im.putdata(new_data)
    return im


# Task 4 a)
def spatial_convolution(gray_im, kernel):
    data = list(gray_im.getdata())
    size_x = gray_im.size[0]
    size_y = gray_im.size[1]
    kernel_border_length = int(sqrt(len(kernel)))
    side_padding = floor(kernel_border_length/2)
    new_data = []
    index = lambda x, y: y*size_x + x
    kernel_index = lambda i, j: i*kernel_border_length + j
    for y in range(0, size_y):
        for x in range(0, size_x):

            if y < side_padding or y >= size_y - side_padding or x < side_padding or x >= size_x - side_padding:
                new_data.append(255)
                continue
            #print(index(x,y))
            conv_sum = 0
            for i in range(0, kernel_border_length):
                for j in range(0, kernel_border_length):
                    #print(kernel_index(i, j))
                    #print(index(x + j - side_padding, y + i - side_padding))
                    conv_sum += data[index(x + j - side_padding, y + i - side_padding)] * kernel[kernel_index(i, j)]
            new_data.append(conv_sum)
    new_im = Image.new("I", gray_im.size)
    new_im.putdata(new_data)
    return new_im

# Task 4 b)
def spatial_convolution_rgb(rgb_im, kernel):
    data_red = []
    data_green = []
    data_blue = []
    color_data = rgb_im.getdata()
    for values in color_data:
        data_red.append(values[0])
        data_green.append(values[1])
        data_blue.append(values[2])

    ## LOL PLEASE DONT JUDGE ME
    red_im = Image.new("I", rgb_im.size)
    red_im.putdata(data_red)
    green_im = Image.new("I", rgb_im.size)
    green_im.putdata(data_green)
    blue_im = Image.new("I", rgb_im.size)
    blue_im.putdata(data_blue)

    red_smooth = spatial_convolution(red_im, kernel3)
    green_smooth = spatial_convolution(green_im, kernel3)
    blue_smooth = spatial_convolution(blue_im, kernel3)

    red_smooth_data = red_smooth.getdata()
    green_smooth_data = green_smooth.getdata()
    blue_smooth_data = blue_smooth.getdata()
    rgb_data = []
    for i in range(0, len(red_smooth_data)):
        val = (red_smooth_data[i], green_smooth_data[i], blue_smooth_data[i])
        rgb_data.append(val)

    smooth_rbg_image = Image.new("RGB", rgb_im.size)
    smooth_rbg_image.putdata(rgb_data)
    return smooth_rbg_image


def gradient_mag(Ix, Iy):
    x_data = Ix.getdata()
    y_data = Iy.getdata()
    magnitude = []
    for i in range(0, len(x_data)):
        mag = sqrt(x_data[i]**2 + y_data[i]**2)
        magnitude.append(mag)
    new_image = Image.new("F", Ix.size)
    new_image.putdata(magnitude)
    return new_image


# Get image
im = Image.open("Lenna.png")
#im = Image.open("rainbow.jpg")
#im = Image.open("vibrant.jpg")
#im = Image.open("dim.jpg")


# Convert image to RGB format
rgb_im = im.convert("RGB")
rgb_im.show()

# Task 2 b)
avg_fun1 = avg
avg_fun2 = avg_lum_preserve
img = rgb2gray(im, avg_fun2)
img.show()

# Task 3 a)
img_inv = intensity_transformation(im)
img_inv.show()

#Tasi 3 b)
im_gamma = gamma_transformation(img, 2.2, 8)



# Task 4
kernel3 = array([1, 1, 1,
                1, 1, 1,
                1, 1, 1]) / 9

kernel5 = array([1, 4, 6, 4,
                 4, 16, 24, 16, 4,
                 6, 24, 36, 24, 6,
                 4, 16, 24, 16, 4,
                 1, 4, 6, 4]) / 256

# Task 4 a)
smooth_img = spatial_convolution(img, kernel3)
smooth_img.show()

smooth_img2 = spatial_convolution(img, kernel5)
smooth_img2.show()

# Task 4 b)
smooth_rbg_image = spatial_convolution_rgb(rgb_im, kernel3)
smooth_rbg_image.show()

smooth_rbg_image2 = spatial_convolution_rgb(rgb_im, kernel5)
smooth_rbg_image2.show()

grad_x = [-1, 0, 1, -2, 0, 2, -1, 0, 1]
grad_y = [-1, -2, -1, 0, 0, 0, 1, 2, 1]
# fucntion i made only work with nxn kernels, so no optimization with separating kernels here
Ix = spatial_convolution(img, grad_x)
Iy = spatial_convolution(img, grad_y)
Ix.show()
Iy.show()

# TASK 4  c)
grad_mag = gradient_mag(Ix, Iy)
grad_mag.show()




