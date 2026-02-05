#!/usr/bin/env python3
import argparse
import os
import subprocess
import sys
import tempfile


def demote_headings(text):
    lines = []
    for line in text.splitlines():
        if line.startswith("# "):
            line = "### " + line[2:]
        elif line.startswith("## "):
            line = "#### " + line[3:]
        lines.append(line)
    return "\n".join(lines)


def run_profiles(inputs, baseline, tolerance, top, profiles):
    root_dir = os.path.dirname(os.path.dirname(os.path.abspath(__file__)))
    script = os.path.join(root_dir, "bench", "report_composite_profiles.py")
    with tempfile.NamedTemporaryFile("r", delete=False, suffix=".md") as tmp:
        tmp_path = tmp.name
    try:
        cmd = [
            sys.executable,
            script,
            *inputs,
            "--out",
            tmp_path,
            "--baseline",
            baseline,
            "--tolerance",
            str(tolerance),
            "--top",
            str(top),
            "--profiles",
            profiles,
        ]
        subprocess.run(cmd, check=True)
        with open(tmp_path, "r", encoding="utf-8") as f:
            content = f.read().strip()
        return demote_headings(content)
    finally:
        try:
            os.unlink(tmp_path)
        except OSError:
            pass


def main():
    ap = argparse.ArgumentParser(description="Generate core leaderboards for drift/no-drift runs.")
    ap.add_argument("--with-drift", required=True, help="Run dir or CSV with drift overlay")
    ap.add_argument("--no-drift", required=True, help="Run dir or CSV without drift overlay")
    ap.add_argument("--out", default="CORE_LEADERBOARDS.md", help="Output markdown path")
    ap.add_argument("--baseline", default="M4_TimeSync", help="Baseline method label")
    ap.add_argument("--tolerance", type=float, default=0.0, help="Allowed % over baseline for E0-6")
    ap.add_argument("--top", type=int, default=10, help="Top N methods to list")
    ap.add_argument(
        "--profiles",
        default="default,skew_sensitive,video_optimized",
        help="Comma list of profile names",
    )
    args = ap.parse_args()

    sections = ["# Core Leaderboards (Drift vs No-Drift)", ""]

    drift_content = run_profiles([args.with_drift], args.baseline, args.tolerance, args.top, args.profiles)
    sections.append("## Core-with-drift")
    sections.append("")
    sections.append(drift_content)
    sections.append("")

    nodrift_content = run_profiles([args.no_drift], args.baseline, args.tolerance, args.top, args.profiles)
    sections.append("## Core-no-drift")
    sections.append("")
    sections.append(nodrift_content)
    sections.append("")

    with open(args.out, "w", encoding="utf-8") as f:
        f.write("\n".join(sections))
    print("Wrote", args.out)
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
