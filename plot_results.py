#!/usr/bin/env python3
import csv
import sys
from collections import defaultdict
import matplotlib.pyplot as plt

if len(sys.argv) != 2:
    print("Usage: plot_results.py results.csv")
    sys.exit(1)

csv_path = sys.argv[1]

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

plt.figure()
plt.errorbar(sizes, row, yerr=row_err, marker='o', label='row-major')
plt.errorbar(sizes, col, yerr=col_err, marker='o', label='col-major')
plt.errorbar(sizes, simd, yerr=simd_err, marker='o', label='SIMD')
plt.xlabel('Matrix size N')
plt.ylabel('Average runtime (ms)')
plt.title('Convolution runtime comparison\n(error bars = std dev across runs)')
plt.legend(title='Variant')
plt.grid(True)
plt.savefig('runtimes.png')
