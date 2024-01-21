from config import *
import serial

from PIL import Image, ImageTk
import tkinter as tk
import threading
import struct
import queue
import time
import os

# Configuration
width, height = (112, 112)
bytesPerFrame = width * height * 3

# Stablish connection with Arduino
port = find_arduino()
arduino = serial.Serial(port, baudrate=9600)

# Global queue to store frames
image_queue = queue.Queue()


def process_rgb_bytes(buffer):
    image = Image.new("RGB", (width, height))
    pixels = image.load()

    for row in range(height):
        for col in range(width):
            pos = (row * width + col) * 3
            p = struct.unpack(">I", buffer[pos : pos + 4])[0]

            # Mask the bits to obtain only the first three
            red = (p & 0xFF000000) >> 24
            green = (p & 0x00FF0000) >> 16
            blue = (p & 0x0000FF00) >> 8

            # Create image
            pixels[col, row] = (red, green, blue)
    return image


def read_frames_from_serial():
    while True:
        stringBuffer = arduino.readline()
        if stringBuffer == b"Frame:\r\n":
            imageBuffer = arduino.read(size=bytesPerFrame + 1)
            image = process_rgb_bytes(imageBuffer)
            image_queue.put(image)
        else:
            if stringBuffer.decode().strip():
                print(stringBuffer.decode().strip())


def ask_frame_serial():
    while True:
        _ = input("> Enter to take picture")
        arduino.write(b"capture\r")


def write_image(image, dir="outputs"):
    timestamp = time.time()
    output_file = os.path.join(dir, str(timestamp) + ".png")
    image.save(output_file, format="PNG")
    return True


# Function to display frames from the queue in order
def display_frames(frameQueue):
    # root = tk.Tk()
    # root.title("Live Video Stream")

    # label = tk.Label(root)
    # label.pack()

    while True:
        try:
            # Get frame from the queue
            frame_image = frameQueue.get(timeout=1)

            # Display the image with Tk
            # tk_image = ImageTk.PhotoImage(frame_image)
            # label.config(image=tk_image)
            # label.image = tk_image
            # root.update_idletasks()
            # root.update()

            # Store image in disk
            write_image(frame_image)

        except queue.Empty:
            time.sleep(1)


if __name__ == "__main__":
    read_thread = threading.Thread(
        target=read_frames_from_serial,
        daemon=True,
    )
    write_thread = threading.Thread(
        target=ask_frame_serial,
        daemon=True,
    )
    read_thread.start()
    write_thread.start()

    display_frames(image_queue)
