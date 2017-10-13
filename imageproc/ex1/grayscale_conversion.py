from PIL import Image
from math import floor
from math import sqrt
from numpy import array





#Task 2 a)
def avg(tupp):
    sum = 0
    for el in tupp:
        sum += el
    return floor(el / 3)

def avg_lum_preserve(tupp):
    sum = 0
    rw = 0.2126
    gw = 0.7152
    bw = 0.0722
    return floor(tupp[0]*rw + tupp[1]*gw + tupp[2]*bw)


def rgb2gray(im, avg_fun):
    pixels = list(im.getdata())
    grey_data = []
    for pix in pixels:
        grey_data.append(avg_fun(pix))
    grey_im = Image.new("I", im.size)
    grey_im.putdata(grey_data)
    return grey_im

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

# Since we were not asked about keeping adding and removing padding, this function removes kernel_border_length rows/cols of pixels from all sides
def spatial_convolution(gray_im, kernel):
    data = list(gray_im.getdata())
    size_x = gray_im.size[0]
    size_y = gray_im.size[1]
    kernel_border_length = int(sqrt(len(kernel)))
    side_padding = floor(kernel_border_length/2)
    new_data = []
    index = lambda x, y: y*size_x + x
    kernel_index = lambda i, j: i*kernel_border_length + j
    for y in range(side_padding, size_y - side_padding):
        for x in range(side_padding, size_x - side_padding):
            #print(index(x,y))
            conv_sum = 0
            for i in range(0, kernel_border_length):
                for j in range(0, kernel_border_length):
                    #print(kernel_index(i, j))
                    #print(index(x + j - side_padding, y + i - side_padding))
                    conv_sum += data[index(x + j - side_padding, y + i - side_padding)] * kernel[kernel_index(i, j)]
            new_data.append(conv_sum)
    new_im = Image.new("I", (size_x - 2*side_padding, size_y - 2*side_padding))
    new_im.putdata(new_data)
    new_im.show()
    return new_im


#im = Image.open("Lenna.png")
im = Image.open("rainbow.jpg")
#im = Image.open("vibrant.jpg")
#im = Image.open("dim.jpg")


rgb_im = im.convert("RGB")
#rgb_im.show()
img = rgb2gray(im, avg_lum_preserve)
#img.show()

img_inv = intensity_transformation(im)
#img_inv.show()

im_gamma = gamma_transformation(img, 2.2, 8)

kernel = array([1, 1, 1,
                1, 1, 1,
                1, 1, 1]) / 9

smooth_img = spatial_convolution(img,kernel)



#rgb2gray(im, avg)
print()