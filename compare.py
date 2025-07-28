from skimage.metrics import structural_similarity as ssim
from PIL import Image
import numpy as np
import os
import re
import csv
import cv2
import sys
from scipy.fft import fft2, fftshift

def compare_fft_rmse(image1_path, image2_path):
    img1 = Image.open(image1_path).convert("L")
    img2 = Image.open(image2_path).convert("L")

    if img1.size != img2.size:
        return -1

    arr1 = np.array(img1, dtype=np.float32)
    arr2 = np.array(img2, dtype=np.float32)

    fft1 = fftshift(fft2(arr1))
    fft2_ = fftshift(fft2(arr2))

    mag1 = np.log1p(np.abs(fft1))
    mag2 = np.log1p(np.abs(fft2_))

    mag1 /= np.max(mag1) if np.max(mag1) > 0 else 1
    mag2 /= np.max(mag2) if np.max(mag2) > 0 else 1

    rmse = np.sqrt(np.mean(np.abs((mag1 - mag2))))
    return rmse

def compare_ssim(image1_path, image2_path):
    img1 = Image.open(image1_path)
    img2 = Image.open(image2_path)

    if img1.size != img2.size:
        return -1

    arr1 = np.array(img1)
    arr2 = np.array(img2)
    ssim_index, _ = ssim(arr1, arr2, channel_axis=-1, full=True)
    return ssim_index

def compare_hist(image1_path, image2_path):
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
mandelbrot_pattern = re.compile(r'^mandelbrot_([+-]?\d+\.\d{6})_([+-]?\d+\.\d{6})_([0-9.eE+-]+)_(float|half|double|posit32_2|posit16_2|bfloat16|cfloat36_8|cfloat17_5)\.png$')
julia_pattern = re.compile(r'^julia_set_([+-]?\d+\.\d{6})_([+-]?\d+\.\d{6})_([0-9.eE+-]+)_([+-]?\d+\.\d{6})_([+-]?\d+\.\d{6})_(float|half|double|posit32_2|posit16_2|bfloat16|cfloat36_8|cfloat17_5)\.png$')

groups = {}

build_dir = "build"

# Group images by parameters
for filename in os.listdir(build_dir):
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

# Write CSV to build directory
csv_path = os.path.join(build_dir, "comparison_results.csv")
with open(csv_path, "w", newline="") as csvfile:
    writer = csv.writer(csvfile)
    writer.writerow([
        "parameters",
        "float_ssim", "half_ssim", "posit32_2_ssim", "posit16_2_ssim", "bfloat16_ssim", "cfloat36_8_ssim", "cfloat17_5_ssim",
        "float_hist", "half_hist", "posit32_2_hist", "posit16_2_hist", "bfloat16_hist", "cfloat36_8_hist", "cfloat17_5_hist",
        "float_sift", "half_sift", "posit32_2_sift", "posit16_2_sift", "bfloat16_sift", "cfloat36_8_sift", "cfloat17_5_sift",
        "float_fft_rmse", "half_fft_rmse", "posit32_2_fft_rmse", "posit16_2_fft_rmse", "bfloat16_fft_rmse", "cfloat36_8_fft_rmse", "cfloat17_5_fft_rmse"
    ])

    for key, files in groups.items():
        double_img = files.get("double")
        if not double_img:
            continue

        def path(dtype): return os.path.join(build_dir, files[dtype]) if dtype in files else None

        float_ssim = compare_ssim(path("float"), path("double")) if "float" in files else None
        float_hist = compare_hist(path("float"), path("double")) if "float" in files else None
        float_sift = compare_sift(path("float"), path("double")) if "float" in files else None
        float_fft_rmse = compare_fft_rmse(path("float"), path("double")) if "float" in files else None

        half_ssim = compare_ssim(path("half"), path("double")) if "half" in files else None
        half_hist = compare_hist(path("half"), path("double")) if "half" in files else None
        half_sift = compare_sift(path("half"), path("double")) if "half" in files else None
        half_fft_rmse = compare_fft_rmse(path("half"), path("double")) if "half" in files else None

        posit32_2_ssim = compare_ssim(path("posit32_2"), path("double")) if "posit32_2" in files else None
        posit32_2_hist = compare_hist(path("posit32_2"), path("double")) if "posit32_2" in files else None
        posit32_2_sift = compare_sift(path("posit32_2"), path("double")) if "posit32_2" in files else None
        posit32_2_fft_rmse = compare_fft_rmse(path("posit32_2"), path("double")) if "posit32_2" in files else None

        posit16_2_ssim = compare_ssim(path("posit16_2"), path("double")) if "posit16_2" in files else None
        posit16_2_hist = compare_hist(path("posit16_2"), path("double")) if "posit16_2" in files else None
        posit16_2_sift = compare_sift(path("posit16_2"), path("double")) if "posit16_2" in files else None
        posit16_2_fft_rmse = compare_fft_rmse(path("posit16_2"), path("double")) if "posit16_2" in files else None

        bfloat16_ssim = compare_ssim(path("bfloat16"), path("double")) if "bfloat16" in files else None
        bfloat16_hist = compare_hist(path("bfloat16"), path("double")) if "bfloat16" in files else None
        bfloat16_sift = compare_sift(path("bfloat16"), path("double")) if "bfloat16" in files else None
        bfloat16_fft_rmse = compare_fft_rmse(path("bfloat16"), path("double")) if "bfloat16" in files else None

        cfloat36_8_ssim = compare_ssim(path("cfloat36_8"), path("double")) if "cfloat36_8" in files else None
        cfloat36_8_hist = compare_hist(path("cfloat36_8"), path("double")) if "cfloat36_8" in files else None
        cfloat36_8_sift = compare_sift(path("cfloat36_8"), path("double")) if "cfloat36_8" in files else None
        cfloat36_8_fft_rmse = compare_fft_rmse(path("cfloat36_8"), path("double")) if "cfloat36_8" in files else None

        cfloat17_5_ssim = compare_ssim(path("cfloat17_5"), path("double")) if "cfloat17_5" in files else None
        cfloat17_5_hist = compare_hist(path("cfloat17_5"), path("double")) if "cfloat17_5" in files else None
        cfloat17_5_sift = compare_sift(path("cfloat17_5"), path("double")) if "cfloat17_5" in files else None
        cfloat17_5_fft_rmse = compare_fft_rmse(path("cfloat17_5"), path("double")) if "cfloat17_5" in files else None

        writer.writerow([
            key,
            f"{float_ssim:.4f}" if float_ssim is not None else "",
            f"{half_ssim:.4f}" if half_ssim is not None else "",
            f"{posit32_2_ssim:.4f}" if posit32_2_ssim is not None else "",
            f"{posit16_2_ssim:.4f}" if posit16_2_ssim is not None else "",
            f"{bfloat16_ssim:.4f}" if bfloat16_ssim is not None else "",
            f"{cfloat36_8_ssim:.4f}" if cfloat36_8_ssim is not None else "",
            f"{cfloat17_5_ssim:.4f}" if cfloat17_5_ssim is not None else "",
            f"{float_hist:.4f}" if float_hist is not None else "",
            f"{half_hist:.4f}" if half_hist is not None else "",
            f"{posit32_2_hist:.4f}" if posit32_2_hist is not None else "",
            f"{posit16_2_hist:.4f}" if posit16_2_hist is not None else "",
            f"{bfloat16_hist:.4f}" if bfloat16_hist is not None else "",
            f"{cfloat36_8_hist:.4f}" if cfloat36_8_hist is not None else "",
            f"{cfloat17_5_hist:.4f}" if cfloat17_5_hist is not None else "",
            f"{float_sift:.4f}" if float_sift is not None else "",
            f"{half_sift:.4f}" if half_sift is not None else "",
            f"{posit32_2_sift:.4f}" if posit32_2_sift is not None else "",
            f"{posit16_2_sift:.4f}" if posit16_2_sift is not None else "",
            f"{bfloat16_sift:.4f}" if bfloat16_sift is not None else "",
            f"{cfloat36_8_sift:.4f}" if cfloat36_8_sift is not None else "",
            f"{cfloat17_5_sift:.4f}" if cfloat17_5_sift is not None else "",
            f"{float_fft_rmse:.6f}" if float_fft_rmse is not None else "",
            f"{half_fft_rmse:.6f}" if half_fft_rmse is not None else "",
            f"{posit32_2_fft_rmse:.6f}" if posit32_2_fft_rmse is not None else "",
            f"{posit16_2_fft_rmse:.6f}" if posit16_2_fft_rmse is not None else "",
            f"{bfloat16_fft_rmse:.6f}" if bfloat16_fft_rmse is not None else "",
            f"{cfloat36_8_fft_rmse:.4f}" if cfloat36_8_fft_rmse is not None else "",
            f"{cfloat17_5_fft_rmse:.4f}" if cfloat17_5_fft_rmse is not None else "",
        ])
