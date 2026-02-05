#!/usr/bin/env python3
import argparse
import os
import subprocess
import sys
import tempfile

PROFILE_PRESETS = {
    "default": "",
    "skew_sensitive": "E0-6=2,E7=4,video=1,steps=1",
    "video_optimized": "E0-6=2,E7=1,video=4,steps=1",
}


def demote_headings(text):
    lines = []
    for line in text.splitlines():
        if line.startswith("# "):
            line = "### " + line[2:]
        elif line.startswith("## "):
            line = "#### " + line[3:]
        lines.append(line)
    return "\n".join(lines)


def main():
    ap = argparse.ArgumentParser(description="Generate composite scorecards for multiple profiles.")
    ap.add_argument("inputs", nargs="+", help="peer_bench.csv files or run directories")
    ap.add_argument("--out", default="COMPOSITE_SCORECARD_PROFILES.md", help="Output markdown path")
    ap.add_argument("--baseline", default="M4_TimeSync", help="Baseline method label")
    ap.add_argument("--tolerance", type=float, default=0.0, help="Allowed % over baseline for E0-6")
    ap.add_argument("--top", type=int, default=10, help="Top N methods to list")
    ap.add_argument(
        "--profiles",
        default="default,skew_sensitive,video_optimized",
        help="Comma list of profile names (default, skew_sensitive, video_optimized)",
    )
    args = ap.parse_args()

    root_dir = os.path.dirname(os.path.dirname(os.path.abspath(__file__)))
    composite = os.path.join(root_dir, "bench", "report_composite_score.py")
    profiles = [p.strip() for p in args.profiles.split(",") if p.strip()]
    if not profiles:
        profiles = ["default"]

    sections = ["# Composite Scorecard Profiles", "", "Sources:"]
    for path in args.inputs:
        sections.append(f"- `{path}`")
    sections.append("")

    for profile in profiles:
        weight_spec = PROFILE_PRESETS.get(profile, profile)
        with tempfile.NamedTemporaryFile("r", delete=False, suffix=".md") as tmp:
            tmp_path = tmp.name
        try:
            cmd = [
                sys.executable,
                composite,
                *args.inputs,
                "--out",
                tmp_path,
                "--baseline",
                args.baseline,
                "--tolerance",
                str(args.tolerance),
                "--top",
                str(args.top),
            ]
            if weight_spec:
                cmd += ["--weights", weight_spec]
            subprocess.run(cmd, check=True)
            with open(tmp_path, "r", encoding="utf-8") as f:
                content = f.read().strip()
            sections.append(f"## Profile: {profile}")
            sections.append("")
            sections.append(demote_headings(content))
            sections.append("")
        finally:
            try:
                os.unlink(tmp_path)
            except OSError:
                pass

    out_path = args.out
    with open(out_path, "w", encoding="utf-8") as f:
        f.write("\n".join(sections))
    print("Wrote", out_path)
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
