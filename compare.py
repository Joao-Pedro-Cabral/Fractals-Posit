from skimage.metrics import structural_similarity as ssim
from PIL import Image
import numpy as np
import os
import re
import csv

def compare_images(image1_path, image2_path):
    # Load and convert to grayscale
    img1 = Image.open(image1_path)
    img2 = Image.open(image2_path)

    # Resize to the same size if needed
    if img1.size != img2.size:
        print("Warning: resizing images to match")
        img2 = img2.resize(img1.size)

    arr1 = np.array(img1)
    arr2 = np.array(img2)

    # SSIM computation
    ssim_index, _ = ssim(arr1, arr2, channel_axis=-1, full=True)

    # Count different pixels
    diff_pixels = np.sum(arr1 != arr2)

    return ssim_index, diff_pixels

# Regex pattern
pattern = re.compile(r'^mandelbrot_([+-]?\d+\.\d{6})_([+-]?\d+\.\d{6})_([0-9.eE+-]+)_(float|double|posit32_2)\.png$')

groups = {}

# Group images by parameters
for filename in os.listdir('.'):
    match = pattern.match(filename)
    if match:
        key = f"{match.group(1)}_{match.group(2)}_{match.group(3)}"
        dtype = match.group(4)
        groups.setdefault(key, {})[dtype] = filename

# Write CSV
with open("comparison_results.csv", "w", newline="") as csvfile:
    writer = csv.writer(csvfile)
    writer.writerow([
        "parameters",
        "float_vs_double_ssim", "float_vs_double_diff_pixels",
        "posit32_2_vs_double_ssim", "posit32_2_vs_double_diff_pixels"
    ])

    for key, files in groups.items():
        double_img = files.get("double")
        if not double_img:
            continue

        float_ssim = float_diff = None
        posit_ssim = posit_diff = None

        if "float" in files:
            float_ssim, float_diff = compare_images(files["float"], double_img)

        if "posit32_2" in files:
            posit_ssim, posit_diff = compare_images(files["posit32_2"], double_img)

        writer.writerow([
            key,
            f"{float_ssim:.4f}" if float_ssim is not None else "",
            float_diff if float_diff is not None else "",
            f"{posit_ssim:.4f}" if posit_ssim is not None else "",
            posit_diff if posit_diff is not None else ""
        ])
