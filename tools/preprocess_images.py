import os
import sys
import subprocess

SUPPORTED_SOURCE_EXTENSIONS = [".jpg", ".png"]
INTERMEDIATE_FORMAT = ".bmp"    # format that compressonator understands as input
TARGET_FORMAT = ".dds"          # desired output format for engine import
COMPRESSONATOR_FORMAT = "DXT3"  # compressed format


def convert_to_dds(image_path):
    base_path = os.path.splitext(image_path)[0]
    intermediate_path = base_path + INTERMEDIATE_FORMAT
    final_path = base_path + TARGET_FORMAT
    subprocess.run(["convert", image_path, intermediate_path])
    subprocess.run(["compressonatorcli", "-fd", COMPRESSONATOR_FORMAT, intermediate_path, final_path])
    subprocess.run(["rm", intermediate_path])


def process_directory(root_dir_path, source_extensions, process_fn):
    for dirpath, dirnames, filenames in os.walk(root_dir_path):
        for file in filenames:
            if file.endswith(tuple(source_extensions)):
                image_abs_path = os.path.join(dirpath, file)
                process_fn(image_abs_path)


if __name__ == "__main__":
    current_relative_dir = sys.argv[1]
    current_abs_dir = os.path.abspath(current_relative_dir)
    process_directory(current_abs_dir, SUPPORTED_SOURCE_EXTENSIONS, convert_to_dds)
