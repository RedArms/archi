#!/bin/bash
set -e

# Build program
make clean >/dev/null 2>&1 || true
make >/dev/null

sizes=(256 512 1024)
k=3
out_file="results.csv"

# Write CSV header
printf "size,version,time_ms\n" > "$out_file"

for n in "${sizes[@]}"; do
  for v in r c s; do
    line=$(./prog -$v $n $k | grep 'Average runtime')
    time_ms=$(echo "$line" | awk '{print $(NF-1)}')
    printf "%s,%s,%s\n" "$n" "$v" "$time_ms" >> "$out_file"
  done
done

python3 plot_results.py "$out_file"
