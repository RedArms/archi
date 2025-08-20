#!/usr/bin/env python3
import csv
import sys
import os
import math
from collections import defaultdict
import matplotlib.pyplot as plt
import re

if len(sys.argv) < 2:
    print("Usage: plot_results.py results.csv [results2.csv ...]")
    sys.exit(1)

csv_paths = sys.argv[1:]
num = len(csv_paths)
cols = min(2, num)
rows = int(math.ceil(num / cols))
fig, axes = plt.subplots(rows, cols, figsize=(6 * cols, 4 * rows))

if num == 1:
    axes = [axes]
else:
    axes = axes.flatten()

for ax, csv_path in zip(axes, csv_paths):
    data = defaultdict(dict)
    errors = defaultdict(dict)
    with open(csv_path) as f:
        reader = csv.DictReader(f)
        for row in reader:
            size = int(row['size'])
            try:
                time_ms = float(row['time_ms'])
                std_ms = float(row.get('std_ms', 0.0))
            except ValueError:
                print(
                    f"Warning: skipping row with non-numeric values {row}",
                    file=sys.stderr,
                )
                continue
            data[size][row['version']] = time_ms
            errors[size][row['version']] = std_ms

    sizes = sorted(data.keys())
    row = [data[n].get('r', 0) for n in sizes]
    row_err = [errors[n].get('r', 0) for n in sizes]
    col = [data[n].get('c', 0) for n in sizes]
    col_err = [errors[n].get('c', 0) for n in sizes]
    simd = [data[n].get('s', 0) for n in sizes]
    simd_err = [errors[n].get('s', 0) for n in sizes]

    ax.errorbar(sizes, row, yerr=row_err, marker='o', label='row-major')
    ax.errorbar(sizes, col, yerr=col_err, marker='o', label='col-major')
    ax.errorbar(sizes, simd, yerr=simd_err, marker='o', label='SIMD')
    ax.set_xlabel('Matrix size N')
    ax.set_ylabel('Average runtime (ms)')

    base = os.path.basename(csv_path)
    m = re.search(r'k(\d+)', base)
    k_label = f"k={m.group(1)} " if m else ""
    ax.set_title(f'{k_label}Convolution runtime')
    ax.legend(title='Variant')
    ax.grid(True)

for ax in axes[len(csv_paths):]:
    ax.set_visible(False)

plt.tight_layout()
plt.savefig('runtimes.png')
