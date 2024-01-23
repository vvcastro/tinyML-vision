# Fruit Recognition with TinyML

This repository is organised as follows:

- `fruit_detection`: The main app for the arduino, with the model an the output tensor. It captures and run the inference when a `command` string is sent to the board.

- `monitor`: The code to extract new images with the same representation as for the model input.

- `trainer`: All the notebooks and files to train the models. All except the `00_base_trainer.ipynb` were executed locally.

## Configurations ⚙️

To run the `fruit_recognition` and the `monitor` apps, a change on the following libraries are needed:

- On the HardvardTinyML library, we deleted the `TensorflowLite` related files. This used to caused a collision on the `tf` library, as the Harvard library has a outdated version is better to delete it.
