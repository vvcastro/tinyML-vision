from tqdm import tqdm
import requests
import zipfile
import shutil
import os


def get_local_filename(url, folder):
    return os.path.join(folder, url.split("/")[-1].split("?")[0])


def download_file(url, dest_folder):
    """Downloads the .zip dataset into the given directory."""
    local_filename = get_local_filename(url, dest_folder)

    block_size = 50 * (2 ^ 20)
    with requests.get(url, stream=True) as r:
        r.raise_for_status()

        # Get the total number of steps
        total_size = int(r.headers.get("Content-Length", 2 * (2 ^ 30)))
        steps = total_size // block_size

        # Store the file in stream mode
        with open(local_filename, "wb") as f:
            for chunk in tqdm(r.iter_content(chunk_size=block_size), total=steps):
                f.write(chunk)
    return local_filename


def unzip_file(filepath, dest_folder):
    """Unzip the contents of the zip file into the destination folder."""
    with zipfile.ZipFile(filepath, "r") as zip:
        for member in tqdm(zip.infolist(), desc="Extracting"):
            try:
                zip.extract(member, dest_folder)
            except:
                pass
    try:
        shutil.rmtree(os.path.join(dest_folder, "__MACOSX"))
    except:
        pass
