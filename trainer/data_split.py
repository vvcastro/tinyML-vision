# In this script you will find the code to split the dataset
# into test and train. Also a there an additional feature for
# different variaties of fruits.
from sklearn.model_selection import train_test_split
import matplotlib.pyplot as plt
from tqdm import tqdm
import seaborn as sns
import numpy as np
import shutil
import glob
import os

BASE_DIR = os.path.join("data", "TinyDataset", "ardData")
TRAIN_DIR = os.path.join("data", "TinyDataset", "trainData")
TEST_DIR = os.path.join("data", "TinyDataset", "testData")

# Get the classes
classes = [d for d in os.listdir(BASE_DIR) if os.path.isdir(os.path.join(BASE_DIR, d))]
classes = sorted(classes)

# Get all the files and their labels
datapoints = glob.glob(os.path.join(BASE_DIR, "*", "*"))
labels = [s.split(os.sep)[-2] for s in datapoints]

# Split in a stratified way ( keeping the same class distribution )
X_train, X_test, Y_train, Y_test = train_test_split(
    datapoints,
    labels,
    test_size=0.2,
    random_state=42,
)
print("Train size:", len(X_train))
print("Test size:", len(X_test))

# Create the train/test folders
os.makedirs(TRAIN_DIR, exist_ok=True)
os.makedirs(TEST_DIR, exist_ok=True)
for c in classes:
    os.makedirs(os.path.join(TRAIN_DIR, c), exist_ok=True)
    os.makedirs(os.path.join(TEST_DIR, c), exist_ok=True)

# Move the files
for f in tqdm(X_train, "Training"):
    shutil.copy(f, f.replace(BASE_DIR, TRAIN_DIR))

for f in tqdm(X_test, "Testing"):
    shutil.copy(f, f.replace(BASE_DIR, TEST_DIR))

# Show the distribution
labels, counts = np.unique(Y_train, return_counts=True)
dist = counts / counts.sum()

x = np.arange(len(labels))
palette = sns.color_palette(None, len(labels))
plt.bar(x, dist, 0.9, color=palette, label=labels)
plt.legend(loc="best")
plt.xticks(x, labels, rotation=45)
plt.show()
