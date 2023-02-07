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

    return datagen
