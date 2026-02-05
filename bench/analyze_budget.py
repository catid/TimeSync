#!/usr/bin/env python3
import argparse
import csv
from collections import defaultdict

def parse_args():
    ap = argparse.ArgumentParser(description="List over-budget rows in peer_bench CSV")
    ap.add_argument("csv_path", help="peer_bench.csv path")
    ap.add_argument("--summary", action="store_true", help="Print grouped summary only")
    return ap.parse_args()


def to_float(row, key):
    try:
        return float(row.get(key, 0.0))
    except ValueError:
        return 0.0


def main():
    args = parse_args()
    with open(args.csv_path, newline="") as f:
        rows = list(csv.DictReader(f))

    over_rows = [r for r in rows if to_float(r, "over_budget") > 0.5]
    summary = defaultdict(list)
    for r in over_rows:
        key = (r.get("scenario"), r.get("method"), r.get("estimator"), r.get("discipline"))
        summary[key].append(to_float(r, "budget_margin_bps"))

    if not args.summary:
        for r in over_rows:
            print(
                f"{r.get('scenario')}, {r.get('method')}|{r.get('estimator')}|{r.get('discipline')}, "
                f"seed={r.get('seed')}, overhead_bps={r.get('overhead_bps')}, "
                f"budget_margin_bps={r.get('budget_margin_bps')}"
            )
        if over_rows:
            print("---")

    print(f"Over-budget rows: {len(over_rows)}/{len(rows)}")
    for key, margins in sorted(summary.items()):
        if not margins:
            continue
        mean_margin = sum(margins) / len(margins)
        max_margin = max(margins)
        print(
            f"{key[0]} | {key[1]}|{key[2]}|{key[3]}: "
            f"count={len(margins)}, mean_margin={mean_margin:.1f}, max_margin={max_margin:.1f}"
        )


if __name__ == "__main__":
    raise SystemExit(main())
