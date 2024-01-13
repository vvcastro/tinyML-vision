import tensorflow as tf
import keras

import _utils as utils
from _config import *

# - Training config
BATCH_SIZE = 4
N_EPOCHS = 4

# Dataset dir
DATASET_FOLDER = f"{DATA_FOLDER}/fruits"
create_dirname(DATASET_FOLDER)

# Model dir (from the .zip)
MODEL_FOLDER = f"{MODELS_FOLDER}/base_model"

# -----------------------------------------
# Download the dataset if it doesn't exist
# -----------------------------------------
classes_exist = len(os.listdir(DATASET_FOLDER)) >= 5

zip_filename = utils.get_local_filename(GENERAL_DATA_URL, DATA_FOLDER)
zip_file_exist = os.path.isfile(zip_filename)

if not classes_exist:
    if not zip_file_exist:
        utils.download_file(GENERAL_DATA_URL, dest_folder=DATA_FOLDER)
    utils.unzip_file(zip_filename, DATA_FOLDER)

# -----------------------------------------
# Download last available model checkpoint
# -----------------------------------------
if not (os.path.isdir(MODEL_FOLDER)):
    model_filename = utils.download_file(BASE_MODEL_URL, dest_folder=MODELS_FOLDER)
    utils.unzip_file(model_filename, MODELS_FOLDER)

# ------------------------------------------
# Load the tensorflow model with keras
# ------------------------------------------
model = keras.models.load_model(MODEL_FOLDER)
model.compile(
    loss=keras.losses.CategoricalCrossentropy(),
    optimizer=keras.optimizers.Adam(learning_rate=1e-3),
    metrics=[keras.metrics.CategoricalAccuracy()],
)
model.summary()

# ------------------------------------------
# Loads the dataset
# ------------------------------------------
datagen = keras.preprocessing.image.ImageDataGenerator(
    shear_range=0.2,
    zoom_range=0.2,
    horizontal_flip=True,
    validation_split=0.2,
)

train_generator = datagen.flow_from_directory(
    DATASET_FOLDER,
    target_size=(112, 112),
    batch_size=BATCH_SIZE,
    class_mode="categorical",
    subset="training",
)

valid_generator = datagen.flow_from_directory(
    DATASET_FOLDER,
    target_size=(112, 112),
    batch_size=BATCH_SIZE,
    class_mode="categorical",
    subset="validation",
)

model.fit(
    train_generator,
    steps_per_epoch=train_generator.samples // BATCH_SIZE,
    validation_data=valid_generator,
    validation_steps=valid_generator.samples // BATCH_SIZE,
    epochs=N_EPOCHS,
)
