import sys


def main() -> int:
    if len(sys.argv) != 2:
        print("Usage: convert_pal_to_hex.py <path>", file=sys.stderr)
        return 1

    path = sys.argv[1]

    with open(path, "r", encoding="utf-8") as f:
        text = f.read().strip()

    # Flatten any existing newlines, split on commas
    parts = [p.strip() for p in text.replace("\n", " ").split(",") if p.strip()]

    try:
        values = [int(p) for p in parts]
    except ValueError as e:
        print(f"Failed to parse integer from palette data: {e}", file=sys.stderr)
        return 1

    lines: list[str] = []
    for i, v in enumerate(values):
        if i % 16 == 0:
            lines.append([])
        lines[-1].append(f"0x{v:04X}")

    out = "\n".join(", ".join(line) for line in lines) + "\n"

    with open(path, "w", encoding="utf-8") as f:
        f.write(out)

    return 0


if __name__ == "__main__":
    raise SystemExit(main())

