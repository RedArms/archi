#!/bin/bash
set -e
export LC_NUMERIC=C

# Build program
make clean >/dev/null 2>&1 || true
make >/dev/null

sizes=(256 512 1024 2048)
ks=(3 5 8 10)
runs=3

for k in "${ks[@]}"; do
  out_file="results_k${k}.csv"
  # Write CSV header
  printf "size,version,time_ms,std_ms\n" > "$out_file"

  for n in "${sizes[@]}"; do
    for v in r c s; do
      times=()
      for ((i=0; i<runs; i++)); do
        line=$(./prog -$v $n $k | grep 'Average runtime')
        times+=("$(echo "$line" | awk '{print $(NF-1)}')")
      done
      mean=$(printf "%s\n" "${times[@]}" | awk '{s+=$1} END {printf "%.3f", s/NR}')
      std=$(printf "%s\n" "${times[@]}" | awk -v m="$mean" '{s+=($1-m)^2} END {printf "%.3f", sqrt(s/NR)}')
      printf "%s,%s,%s,%s\n" "$n" "$v" "$mean" "$std" >> "$out_file"
    done
  done

  python3 plot_results.py "$out_file"
  mv runtimes.png "runtimes_k${k}.png" 2>/dev/null || true
done
