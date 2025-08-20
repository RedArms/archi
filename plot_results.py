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
with open(csv_path) as f:
    reader = csv.DictReader(f)
    for row in reader:
        size = int(row['size'])
        data[size][row['version']] = float(row['time_ms'])

sizes = sorted(data.keys())
row = [data[n].get('r', 0) for n in sizes]
col = [data[n].get('c', 0) for n in sizes]
simd = [data[n].get('s', 0) for n in sizes]

plt.figure()
plt.plot(sizes, row, marker='o', label='row-major')
plt.plot(sizes, col, marker='o', label='col-major')
plt.plot(sizes, simd, marker='o', label='SIMD')
plt.xlabel('Matrix size N')
plt.ylabel('Avg runtime (ms)')
plt.title('Convolution runtime comparison')
plt.legend()
plt.grid(True)
plt.savefig('runtimes.png')
