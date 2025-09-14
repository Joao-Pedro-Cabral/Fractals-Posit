from skimage.metrics import structural_similarity as ssim
from PIL import Image
import numpy as np
import os
import re
import csv
import cv2
import sys
from scipy.fft import fft2, fftshift
from math import log10


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

    rmse = np.sqrt(np.mean((mag1 - mag2) ** 2))
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


def compare_mse(image1_path, image2_path):
    img1 = Image.open(image1_path)
    img2 = Image.open(image2_path)

    if img1.size != img2.size:
        return -1

    arr1 = np.array(img1, dtype=np.float32)
    arr2 = np.array(img2, dtype=np.float32)
    return float(np.mean((arr1 - arr2) ** 2))


def compare_psnr(image1_path, image2_path):
    mse = compare_mse(image1_path, image2_path)
    if mse <= 0:
        return float("inf")
    return 10.0 * log10((255.0**2) / mse)


def compare_hist(image1_path, image2_path):
    image1 = cv2.imread(image1_path)
    image2 = cv2.imread(image2_path)
    hist_img1 = cv2.calcHist(
        [image1], [0, 1, 2], None, [256, 256, 256], [0, 256, 0, 256, 0, 256]
    )
    cv2.normalize(hist_img1, hist_img1, alpha=0, beta=1, norm_type=cv2.NORM_MINMAX)
    hist_img2 = cv2.calcHist(
        [image2], [0, 1, 2], None, [256, 256, 256], [0, 256, 0, 256, 0, 256]
    )
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


# Regex pattern (added softposit32/softposit16 and cpp_dec_float_1000)
dtype_group = r"(cfloat32_8|cfloat16_5|cfloat64_11|posit32_2|posit16_1|posit16_2|posit16_3|bfloat16_8|cfloat36_8|cfloat17_5|softposit32|softposit16|cpp_dec_float_1000)"
mandelbrot_pattern = re.compile(
    rf"^mandelbrot_([+-]?\d+\.\d{{6}})_([+-]?\d+\.\d{{6}})_([0-9.eE+-]+)_{dtype_group}\.png$"
)
julia_pattern = re.compile(
    rf"^julia_set_([+-]?\d+\.\d{{6}})_([+-]?\d+\.\d{{6}})_([0-9.eE+-]+)_([+-]?\d+\.\d{{6}})_([+-]?\d+\.\d{{6}})_{dtype_group}\.png$"
)

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

    dtypes = [
        "cfloat32_8",
        "cfloat16_5",
        "posit32_2",
        "posit16_1",
        "posit16_2",
        "posit16_3",
        "bfloat16_8",
        "cfloat36_8",
        "cfloat17_5",
        "softposit32",
        "softposit16",
    ]
    metrics = [
        ("ssim", compare_ssim, 4),
        ("psnr", compare_psnr, 2),
        ("mse", compare_mse, 2),
        ("hist", compare_hist, 4),
        ("sift", compare_sift, 4),
        ("fft_rmse", compare_fft_rmse, 6),
    ]

    # Write header
    header = ["parameters"] + [f"{dt}_{m}" for m, _, _ in metrics for dt in dtypes]
    writer.writerow(header)

    for key, files in groups.items():
        # cpp_dec_float_1000 is the new baseline
        if "cpp_dec_float_1000" not in files:
            continue

        def path(dtype):
            return os.path.join(build_dir, files[dtype]) if dtype in files else None

        results = {}
        for dtype in dtypes:
            if dtype in files:
                for metric_name, func, precision in metrics:
                    try:
                        val = func(path(dtype), path("cpp_dec_float_1000"))
                    except Exception:
                        val = float("nan")
                    # handle inf and nan formatting explicitly
                    if isinstance(val, float) and (np.isinf(val) or np.isnan(val)):
                        results[(dtype, metric_name)] = str(val)
                    else:
                        results[(dtype, metric_name)] = f"{val:.{precision}f}"

        row = [key] + [results.get((dt, m), "") for m, _, _ in metrics for dt in dtypes]
        writer.writerow(row)
