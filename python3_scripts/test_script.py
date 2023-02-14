import numpy as np
def get_datagen(dims=(100, 100), batch_size=32):

    datagen = ImageDataGenerator(
        rescale=1. / 255,
        shear_range=0.2,
        zoom_range=0.2,
        rotation_range=90,
        horizontal_flip=True,
        vertical_flip=True,
        featurewise_center=False,
        samplewise_center=False,
      zca_whitening=False,
    )
    a = 0.003e12
    return datagen

b___ = np.array([-4.92095085e+11, -1.07121220e+12, -2.58952667e+11,  6.80621667e+11,
       -1.42886732e+12,  1.44890253e+12, -1.38716336e+12, -1.22090225e+12,
       -1.83645007e+12, -1.10038934e+12])

#### Bracket nesting with arbitrary indents
a = {"asdf": [[[1, 2, 3], [2, 3], 
        [3, 4,5], [0,1],[],
            [],[],[]]], "fida": 
                {"goofy": "asdf"}}

#### Str testing
""" asdf """
((((((

    ((((

        (((
