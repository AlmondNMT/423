import numpy as np
from PIL import Image

def salt_and_pepper(pixels: np.ndarray, perc: float):
    """
    :param pixels: 
    :param perc:
    :return: a tuple of salt_and_pepper pixels and 
    :rtype: np.ndarray
    """
    assert type(pixels) is np.ndarray
    h, w = pixels.shape[:2]
    unif = 255 * np.round(np.random.uniform(-.5 - perc, .5 + perc, h * w))
    sp = np.array([[i, i, i] for i in unif]).reshape(pixels.shape)
    new_pixels = np.array(np.clip(pixels + sp, 0, 255), dtype=np.uint8) # never forget uint8
    Image.fromarray(new_pixels, mode="RGB").save("images/donuts_sp.jpg")
    return new_pixels
    

def dim(pixels, scale):
    """
    :rtype: np.ndarray
    """
    assert type(scale) in [int, float]
    assert 0 <= scale, "scale must be nonnegative"
    new_pixels = np.array(np.clip(np.dot(pixels, scale), 0, 255), dtype=np.uint8)
    Image.fromarray(new_pixels, mode="RGB").save("images/donuts_dimmed.jpg")
    return new_pixels


def black_box(pixels, width: int, height: int):
    assert type(pixels) is np.ndarray, "pixels must be ndarray"
    assert type(width) is int and type(height) is int, "width and height must be integers"
    h, w, ch = pixels.shape
    assert width < w, "width parameter must be less than width of image"
    assert height < h, "height parameter must be less than height of image"
    left, top = np.random.randint(0, w - width), np.random.randint(0, h - height)
    new_pixels = np.copy(pixels)
    new_pixels[top:top+height, left:left+width] = 0
    Image.fromarray(new_pixels, mode="RGB").save("images/donuts_bb.jpg")
    return new_pixels


def euclidean(u, v):
    return np.linalg.norm(u - v)


with Image.open("images/donuts.jpg") as img:
    pixels = np.array(img)
    h, w, ch = pixels.shape
    sp = salt_and_pepper(pixels, 0.02)
    dimmed = dim(pixels, .8)
    bb = black_box(pixels, 90, 73)
    print("Euclid(orig, s&p): {}".format(euclidean(pixels, sp)))
    print("Euclid(orig, dim): {}".format(euclidean(pixels, dimmed)))
    print("Euclid(orig, bb): {}".format(euclidean(pixels, bb)))
