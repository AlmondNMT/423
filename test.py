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
    '''train_dataset = tf.keras.utils.image_dataset_from_directory(
        image_dir_multi,
        labels="inferred",
        label_mode="categorical",
        color_mode="rgb",
        batch_size=batch_size,
        image_size=dims,
        shuffle=True,
        subset="training",
        seed=1,
        validation_split=0.2,
        interpolation="bilinear"
    )
    val_dataset = tf.keras.utils.image_dataset_from_directory(
        image_dir_multi,
        labels="inferred",
        label_mode="categorical",
        color_mode="rgb",
        batch_size=batch_size,
        image_size=dims,
        shuffle=True,
        subset="validation",
        seed=1,
        validation_split=0.2,
        interpolation="bilinear"
    )'''

    return datagen
