#!/usr/bin/env python3
import csv
import glob
import os
import sys


def load_rows(path):
    with open(path, "r", newline="") as f:
        reader = csv.DictReader(f)
        return reader.fieldnames, list(reader)


def is_number(value):
    try:
        float(value)
        return True
    except ValueError:
        return False


def main():
    if len(sys.argv) < 3:
        print("Usage: aggregate_csv.py <dir> <out_csv>")
        return 1

    in_dir = sys.argv[1]
    out_csv = sys.argv[2]
    files = sorted(glob.glob(os.path.join(in_dir, "seed_*.csv")))
    if not files:
        print("No seed CSV files found in", in_dir)
        return 1

    headers, first_rows = load_rows(files[0])
    if not headers:
        print("Missing headers in", files[0])
        return 1

    numeric_fields = [h for h in headers if h != "name"]

    sums = {}
    counts = {}

    for path in files:
        _, rows = load_rows(path)
        for row in rows:
            name = row.get("name", "")
            if not name:
                continue
            if name not in sums:
                sums[name] = {h: 0.0 for h in numeric_fields}
                counts[name] = {h: 0 for h in numeric_fields}
            for h in numeric_fields:
                val = row.get(h, "")
                if val == "":
                    continue
                if not is_number(val):
                    continue
                sums[name][h] += float(val)
                counts[name][h] += 1

    names = sorted(sums.keys())
    with open(out_csv, "w", newline="") as f:
        writer = csv.DictWriter(f, fieldnames=headers)
        writer.writeheader()
        for name in names:
            out_row = {"name": name}
            for h in numeric_fields:
                c = counts[name][h]
                out_row[h] = (sums[name][h] / c) if c else 0.0
            writer.writerow(out_row)

    return 0


if __name__ == "__main__":
    raise SystemExit(main())
