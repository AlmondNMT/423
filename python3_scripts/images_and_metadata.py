import glob
import os
import time

root_dir = "../data/all_images"
aug_dir = "../data/augmented"
df = pd.read_csv("../data/csvs/HAM10000_metadata.csv")
img_resize = (100, 100)

def get_images(n):
    X = []
    start = time.time()
    for i, row in enumerate(np.array(df)):
        if i >= n:
            break
        img_path = os.path.join(root_dir, row[1] + ".jpg")
        X.append(np.array(Image.open(img_path).resize(img_resize, Image.Resampling.BICUBIC)))
    print(time.time() - start)
    return X

