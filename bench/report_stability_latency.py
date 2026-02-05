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
    if name.startswith(("E8_", "E9_")):
        return "E8-9"
    return None


def main():
    ap = argparse.ArgumentParser(description="Stability vs latency tradeoff summary.")
    ap.add_argument("csv_path", help="peer_bench.csv path")
    ap.add_argument("--group", default="all",
                    help="Scenario group: all, E0-6, E7, E8-9")
    ap.add_argument("--out", default="stability_latency.csv", help="Output CSV path")
    ap.add_argument("--plot", default="stability_latency.pdf", help="Output PDF path")
    ap.add_argument("--require-poll", action="store_true",
                    help="Require poll_time_err_valid_ab/ba = 1")
    args = ap.parse_args()

    rows = []
    with open(args.csv_path, "r", newline="") as f:
        reader = csv.DictReader(f)
        for row in reader:
            if args.require_poll:
                if int(to_float(row, "poll_time_err_valid_ab", 0)) == 0:
                    continue
                if int(to_float(row, "poll_time_err_valid_ba", 0)) == 0:
                    continue
            if args.group != "all":
                grp = scenario_group(row.get("scenario", ""))
                if grp != args.group:
                    continue
            rows.append(row)

    if not rows:
        print("No rows for selection")
        return 1

    grouped = {}
    for row in rows:
        key = (row.get("method", ""),
               row.get("estimator", ""),
               row.get("discipline", ""))
        grouped.setdefault(key, []).append(row)

    summary = []
    for key, items in grouped.items():
        stability = []
        latency = []
        for r in items:
            err = max(to_float(r, "poll_time_err_p95_ab_us", 0.0),
                      to_float(r, "poll_time_err_p95_ba_us", 0.0))
            owd = max(to_float(r, "owd_p95_ab_us", 0.0),
                      to_float(r, "owd_p95_ba_us", 0.0))
            if err > 0.0:
                stability.append(err)
            if owd > 0.0:
                latency.append(owd)
        if not stability or not latency:
            continue
        summary.append({
            "method": key[0],
            "estimator": key[1],
            "discipline": key[2],
            "stability_p95_us": median(stability),
            "latency_p95_us": median(latency),
        })

    summary.sort(key=lambda x: (x["stability_p95_us"], x["latency_p95_us"]))

    with open(args.out, "w", newline="") as f:
        w = csv.writer(f)
        w.writerow(["method", "estimator", "discipline",
                    "stability_p95_us", "latency_p95_us"])
        for row in summary:
            w.writerow([row["method"], row["estimator"], row["discipline"],
                        f"{row['stability_p95_us']:.3f}",
                        f"{row['latency_p95_us']:.3f}"])
    print("Wrote", args.out)

    try:
        import matplotlib.pyplot as plt
    except Exception as exc:
        print("matplotlib not available:", exc)
        return 0

    x = [r["latency_p95_us"] for r in summary]
    y = [r["stability_p95_us"] for r in summary]
    labels = [r["method"] for r in summary]

    fig, ax = plt.subplots(figsize=(7.5, 5.0))
    ax.scatter(x, y, alpha=0.8)
    ax.set_xlabel("OWD p95 (us)")
    ax.set_ylabel("Time error p95 (us)")
    ax.set_title(f"Stability vs latency tradeoff ({args.group})")
    ax.grid(linestyle=":", alpha=0.5)
    for lx, ly, label in zip(x, y, labels):
        ax.annotate(label, (lx, ly), fontsize=7, alpha=0.6)
    fig.tight_layout()
    fig.savefig(args.plot)
    plt.close(fig)
    print("Wrote", args.plot)
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
