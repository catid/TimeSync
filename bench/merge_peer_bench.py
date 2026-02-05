#!/usr/bin/env python3
import argparse
import csv
import os


def read_header(path):
    with open(path, "r", newline="") as f:
        r = csv.reader(f)
        return next(r, None)


def main():
    ap = argparse.ArgumentParser(description="Merge peer_bench CSVs with identical headers.")
    ap.add_argument("out_csv", help="Output merged CSV path")
    ap.add_argument("inputs", nargs="+", help="Input peer_bench.csv paths")
    ap.add_argument("--dedupe", action="store_true",
                    help="Remove duplicate rows by (scenario,method,estimator,discipline,seed)")
    args = ap.parse_args()

    inputs = [p for p in args.inputs if os.path.isfile(p)]
    if not inputs:
        print("No input files found.")
        return 1

    base_header = read_header(inputs[0])
    if not base_header:
        print("Empty header in", inputs[0])
        return 1

    for path in inputs[1:]:
        hdr = read_header(path)
        if hdr != base_header:
            print("Header mismatch:", path)
            return 1

    seen = set()
    with open(args.out_csv, "w", newline="") as out_f:
        writer = csv.writer(out_f)
        writer.writerow(base_header)
        for path in inputs:
            with open(path, "r", newline="") as in_f:
                reader = csv.DictReader(in_f)
                for row in reader:
                    if args.dedupe:
                        key = (row.get("scenario", ""), row.get("method", ""),
                               row.get("estimator", ""), row.get("discipline", ""),
                               row.get("seed", ""))
                        if key in seen:
                            continue
                        seen.add(key)
                    writer.writerow([row.get(h, "") for h in base_header])
    print("Wrote", args.out_csv)
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
