import sys
import os
from pathlib import Path

if __name__ == "__main__":
    if len(sys.argv) <= 1:
        print("No files provided! Exiting...")
        exit(1)

    if sys.argv[1] == "--outfiles":
        if len(sys.argv) < 3:
            exit(1)

        for in_file in sys.argv[2:]:
            out_dir = Path(in_file).parent
            out_file = Path(in_file).stem + ".gen.hpp"
            print(os.path.join(out_dir, out_file), end=";")
        exit(0)

    for in_file in sys.argv[1:]:
        out_dir = Path(in_file).parent
        out_file = Path(in_file).stem + ".gen.hpp"
        with open(in_file, "rt", encoding="utf-8") as shader:
            print(" -> " + out_file)
            with open(
                os.path.join(out_dir, out_file), "wt", encoding="utf-8"
            ) as header:
                # Write header
                _ = header.write("#pragma once\n")
                _ = header.write(
                    "constexpr char " + Path(in_file).stem + "_source[] = \n"
                )
                for line in shader.readlines():
                    if line.strip() == "":
                        continue
                    _ = header.write('  "' + line.strip() + ' "\n')
                _ = header.write(";\n")
