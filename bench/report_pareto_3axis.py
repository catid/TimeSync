#!/usr/bin/env python3
import argparse
import csv
import statistics


def to_float(row, key, default=0.0):
    try:
        return float(row.get(key, default))
    except ValueError:
        return default


def median(values):
    if not values:
        return 0.0
    return float(statistics.median(values))


def scenario_group(name):
    if name.startswith(("E0_", "E1_", "E2_", "E3_", "E4_", "E5_", "E6_")):
        return "E0-6"
    if name.startswith("E7_"):
        return "E7"
    if "video" in name:
        return "video"
    return None


def dominates(a, b):
    return (a["E0-6"] <= b["E0-6"] and a["E7"] <= b["E7"] and a["video"] <= b["video"]
            and (a["E0-6"] < b["E0-6"] or a["E7"] < b["E7"] or a["video"] < b["video"]))


def main():
    ap = argparse.ArgumentParser(description="3-axis Pareto frontier (E0–E6, E7, video).")
    ap.add_argument("csv_path", help="peer_bench.csv path")
    ap.add_argument("--out", default="pareto_3axis.csv", help="Output CSV path")
    ap.add_argument("--out-md", default="pareto_3axis.md", help="Output markdown path")
    ap.add_argument("--require-poll", action="store_true",
                    help="Require poll_time_err_valid_ab/ba = 1")
    args = ap.parse_args()

    with open(args.csv_path, "r", newline="") as f:
        rows = list(csv.DictReader(f))
    if not rows:
        print("No rows in", args.csv_path)
        return 1

    grouped = {}
    for r in rows:
        grp = scenario_group(r.get("scenario", ""))
        if grp is None:
            continue
        if args.require_poll:
            if int(to_float(r, "poll_time_err_valid_ab", 0)) == 0:
                continue
            if int(to_float(r, "poll_time_err_valid_ba", 0)) == 0:
                continue
        key = (r["method"], r["estimator"], r["discipline"])
        grouped.setdefault(key, []).append(r)

    entries = []
    for key, items in grouped.items():
        vals = {"E0-6": [], "E7": [], "video": []}
        for r in items:
            grp = scenario_group(r.get("scenario", ""))
            if grp is None:
                continue
            err = max(to_float(r, "poll_time_err_p95_ab_us", 0.0),
                      to_float(r, "poll_time_err_p95_ba_us", 0.0))
            if err > 0.0:
                vals[grp].append(err)
        if not vals["E0-6"] or not vals["E7"] or not vals["video"]:
            continue
        entries.append({
            "method": key[0],
            "estimator": key[1],
            "discipline": key[2],
            "E0-6": median(vals["E0-6"]),
            "E7": median(vals["E7"]),
            "video": median(vals["video"]),
        })

    frontier = []
    for a in entries:
        if any(dominates(b, a) for b in entries):
            continue
        frontier.append(a)

    frontier.sort(key=lambda x: (x["E0-6"], x["E7"], x["video"]))

    with open(args.out, "w", newline="") as f:
        w = csv.writer(f)
        w.writerow(["method", "estimator", "discipline",
                    "e0_6_p95_us", "e7_p95_us", "video_p95_us"])
        for e in frontier:
            w.writerow([e["method"], e["estimator"], e["discipline"],
                        f"{e['E0-6']:.3f}", f"{e['E7']:.3f}", f"{e['video']:.3f}"])
    print("Wrote", args.out)

    lines = [
        "# 3-axis Pareto frontier",
        "",
        "| Method | E0-6 p95 | E7 p95 | video p95 |",
        "| --- | ---: | ---: | ---: |",
    ]
    for e in frontier:
        lines.append(f"| {e['method']} | {e['E0-6']:.2f} | {e['E7']:.2f} | {e['video']:.2f} |")
    with open(args.out_md, "w", encoding="utf-8") as f:
        f.write("\n".join(lines))
    print("Wrote", args.out_md)
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
