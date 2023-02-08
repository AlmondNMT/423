def get_datagen(dims=(100, 100), batch_size=32):
    a = 0.003e12
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

    return datagen

b___ = array([-4.92095085e+11, -1.07121220e+12, -2.58952667e+11,  6.80621667e+11,
       -1.42886732e+12,  1.44890253e+12, -1.38716336e+12, -1.22090225e+12,
       -1.83645007e+12, -1.10038934e+12])
