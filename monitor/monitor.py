from config import *
import serial

from PIL import Image, ImageTk
import tkinter as tk
import threading
import struct
import queue
import time

# Configuration
bytesPerPixel = 2
rawWidth, rawHeight = (176, 144)
rawBytesPerFrame = rawWidth * rawHeight * bytesPerPixel

rgbWidth, rgbHeight = (112, 112)
rgbBytesPerFrame = rgbWidth * rgbHeight * 3

# Stablish connection with Arduino
port = find_arduino()
arduino = serial.Serial(port, baudrate=9600)

# Global queue to store frames
raw_queue = queue.Queue()
crop_queue = queue.Queue()

raw_image = []
crop_image = []


def process_frame(buffer, width, height):
    image = Image.new("RGB", (width, height))
    pixels = image.load()

    index = 0
    for i in range(height):
        for j in range(width):
            p = struct.unpack(">H", buffer[index : index + 2])[0]

            # # Convert RGB565 to RGB 24-bit
            # r = ((p >> 11) & 0x1F) << 3
            # g = ((p >> 5) & 0x3F) << 2
            # b = ((p >> 0) & 0x1F) << 3
            # print(r, g, b)

            # Convert RGB565 to RGB 24-bit
            base_red = (p & 0xF800) >> 11
            base_green = (p & 0x07E0) >> 5
            base_blue = p & 0x001F

            r = (base_red << 3) | (base_red >> 2)
            g = (base_green << 2) | (base_green >> 4)
            b = (base_blue << 3) | (base_blue >> 2)
            if (i > 70) and (i <= 75):
                if (j > 80) and (j <= 95):
                    print(r, g, b)

            # Set pixel color
            pixels[j, i] = (r, g, b)
            index += 2
    return image


def process_rgb_frame():
    print("Reading frame...")
    image = Image.new("RGB", (cropWidth, cropHeight))
    pixels = image.load()

    for row in range(cropHeight):
        for col in range(cropWidth):
            pixel_values = arduino.readline().decode().strip()
            pixels[col, row] = tuple([int(float(v) * 255) for v in pixel_values.split(",")])
    return image


def process_rgb_bytes(buffer, width, height):
    image = Image.new("RGB", (width, height))
    pixels = image.load()

    for row in range(height):
        for col in range(width):
            pos = (row * width + col) * 3
            p = struct.unpack(">I", buffer[pos : pos + 4])[0]

            # _ = (p & 0x)
            red = (p & 0xFF000000) >> 24
            green = (p & 0x00FF0000) >> 16
            blue = (p & 0x0000FF00) >> 8

            pixels[col, row] = (red, green, blue)
    return image


def read_frames_from_serial():
    while True:
        stringBuffer = arduino.readline()
        # print(stringBuffer)

        if stringBuffer == b" - Camera: Capturing frame...\r\n":
            # rawImageBuffer = arduino.read(size=rawBytesPerFrame)
            # rawimage = process_frame(rawImageBuffer, rawWidth, rawHeight)
            # raw_queue.put(rawimage)
            continue

        elif stringBuffer == b" - Camera: Cropping frame...\r\n":
            # cropImageBuffer = arduino.read(size=cropBytesPerFrame)
            # cropimage = process_frame(cropImageBuffer, cropWidth, cropHeight)
            # crop_queue.put(cropimage)
            continue

        elif stringBuffer == b"PIXELS\r\n":
            imageBuffer = arduino.read(size=rgbBytesPerFrame + 1)
            cropimage = process_rgb_bytes(imageBuffer, rgbWidth, rgbHeight)
            crop_queue.put(cropimage)


# Function to display frames from the queue in order
def display_frames(frameQueue):
    root = tk.Tk()
    root.title("Live Video Stream")

    label = tk.Label(root)
    label.pack()

    while True:
        try:
            frame_image = frameQueue.get(timeout=1)  # Get frame from the queue
            tk_image = ImageTk.PhotoImage(frame_image)
            label.config(image=tk_image)
            label.image = tk_image
            root.update_idletasks()
            root.update()
        except queue.Empty:
            time.sleep(1)


if __name__ == "__main__":
    read_thread = threading.Thread(
        target=read_frames_from_serial,
        daemon=True,
    )
    read_thread.start()
    display_frames(crop_queue)
