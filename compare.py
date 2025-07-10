from skimage.metrics import structural_similarity as ssim
from PIL import Image
import numpy as np
import os
import re
import csv

import cv2

def compare_ssim(image1_path, image2_path):
    # Load and convert to grayscale
    img1 = Image.open(image1_path)
    img2 = Image.open(image2_path)

    # Resize to the same size if needed
    if img1.size != img2.size:
        return -1

    arr1 = np.array(img1)
    arr2 = np.array(img2)

    # SSIM computation
    ssim_index, _ = ssim(arr1, arr2, channel_axis=-1, full=True)

    return ssim_index

def compare_hist(image1_path, image2_path):
    # Load images
    image1 = cv2.imread(image1_path)
    image2 = cv2.imread(image2_path)
    hist_img1 = cv2.calcHist([image1], [0, 1, 2], None, [256, 256, 256], [0, 256, 0, 256, 0, 256])
    cv2.normalize(hist_img1, hist_img1, alpha=0, beta=1, norm_type=cv2.NORM_MINMAX)
    hist_img2 = cv2.calcHist([image2], [0, 1, 2], None, [256, 256, 256], [0, 256, 0, 256, 0, 256])
    cv2.normalize(hist_img2, hist_img2, alpha=0, beta=1, norm_type=cv2.NORM_MINMAX)
    return cv2.compareHist(hist_img1, hist_img2, cv2.HISTCMP_CORREL)

def compare_sift(image1_path, image2_path):
    # Load RGB images
    img1_color = cv2.imread(image1_path)
    img2_color = cv2.imread(image2_path)

    # Convert to grayscale
    img1 = cv2.cvtColor(img1_color, cv2.COLOR_BGR2GRAY)
    img2 = cv2.cvtColor(img2_color, cv2.COLOR_BGR2GRAY)

    sift = cv2.SIFT_create()
    kp1, des1 = sift.detectAndCompute(img1, None)
    kp2, des2 = sift.detectAndCompute(img2, None)

    if des1 is None or des2 is None:
        return 0.0

    # FLANN matcher
    index_params = dict(algorithm=1, trees=5)
    search_params = dict(checks=50)
    flann = cv2.FlannBasedMatcher(index_params, search_params)
    matches = flann.knnMatch(des1, des2, k=2)

    # Apply Lowe's ratio test
    good_matches = [m for m, n in matches if m.distance < 0.75 * n.distance]

    similarity = len(good_matches) / max(len(kp1), len(kp2))
    return similarity


# Regex pattern
mandelbrot_pattern = re.compile(r'^mandelbrot_([+-]?\d+\.\d{6})_([+-]?\d+\.\d{6})_([0-9.eE+-]+)_(float|half|double|posit32_2)\.png$')
julia_pattern = re.compile(r'^julia_set_([+-]?\d+\.\d{6})_([+-]?\d+\.\d{6})_([0-9.eE+-]+)_([+-]?\d+\.\d{6})_([+-]?\d+\.\d{6})_(float|half|double|posit32_2)\.png$')

groups = {}

# Group images by parameters
for filename in os.listdir('.'):
    match = mandelbrot_pattern.match(filename)
    if match:
        key = f"{match.group(1)}_{match.group(2)}_{match.group(3)}"
        dtype = match.group(4)
        groups.setdefault(key, {})[dtype] = filename
        continue

    match = julia_pattern.match(filename)
    if match:
        key = f"{match.group(1)}_{match.group(2)}_{match.group(3)}_{match.group(4)}_{match.group(5)}"
        dtype = match.group(6)
        groups.setdefault(key, {})[dtype] = filename

# Write CSV
with open("comparison_results.csv", "w", newline="") as csvfile:
    writer = csv.writer(csvfile)
    writer.writerow([
        "parameters",
        "float_ssim", "float_hist", "float_sift",
        "half_ssim", "half_hist", "half_sift",
        "posit32_2_ssim", "posit32_2_hist", "posit32_2_sift",
    ])

    for key, files in groups.items():
        double_img = files.get("double")
        if not double_img:
            continue

        float_ssim = float_hist = float_sift = None
        half_ssim = half_hist = half_sift = None
        posit_ssim = posit_hist = posit_sift = None

        if "float" in files:
            float_ssim = compare_ssim(files["float"], double_img)
            float_hist = compare_sift(files["float"], double_img)
            float_sift = compare_sift(files["float"], double_img)

        if "half" in files:
            half_ssim = compare_ssim(files["half"], double_img)
            half_hist = compare_sift(files["half"], double_img)
            half_sift = compare_sift(files["half"], double_img)

        if "posit32_2" in files:
            posit_ssim = compare_ssim(files["posit32_2"], double_img)
            posit_hist = compare_sift(files["posit32_2"], double_img)
            posit_sift = compare_sift(files["posit32_2"], double_img)

        writer.writerow([
            key,
            f"{float_ssim:.4f}" if float_ssim is not None else "",
            f"{float_hist:.4f}" if float_hist is not None else "",
            f"{float_sift:.4f}" if float_sift is not None else "",
            f"{half_ssim:.4f}" if half_ssim is not None else "",
            f"{half_hist:.4f}" if half_hist is not None else "",
            f"{half_sift:.4f}" if half_sift is not None else "",
            f"{posit_ssim:.4f}" if posit_ssim is not None else "",
            f"{posit_hist:.4f}" if posit_hist is not None else "",
            f"{posit_sift:.4f}" if posit_sift is not None else "",
        ])
