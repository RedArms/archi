#!/bin/bash
set -e

# Build program
make clean >/dev/null 2>&1 || true
make >/dev/null

sizes=(256 512 1024 2048)
k=8
runs=3
out_file="results.csv"

# Write CSV header
printf "size,version,time_ms,std_ms\n" > "$out_file"

for n in "${sizes[@]}"; do
  for v in r c s; do
    times=()
    for ((i=0; i<runs; i++)); do
      line=$(./prog -$v $n $k | grep 'Average runtime')
      times+=("$(echo "$line" | awk '{print $(NF-1)}')")
    done
    mean=$(printf "%s\n" "${times[@]}" | awk '{s+=$1} END {print s/NR}')
    std=$(printf "%s\n" "${times[@]}" | awk -v m="$mean" '{s+=($1-m)^2} END {print sqrt(s/NR)}')
    printf "%s,%s,%.3f,%.3f\n" "$n" "$v" "$mean" "$std" >> "$out_file"
  done
done

python3 plot_results.py "$out_file"
