import sys
import os
from pathlib import Path

def find_and_replace_sources(dir):
    if not Path(os.path.join(dir, "CMakeLists.txt")).exists():
        return
    print("Processing: ", dir)
    with open(os.path.join(dir, "CMakeLists.txt"), "r") as inf:
        with open(os.path.join(dir, "CMakeLists.txt.tmp"), "w") as outf:

            for line in inf:
                outf.write(line)
                stripped = line.strip()

                if stripped.lower().startswith("add_subdirectory("):
                    newdir = stripped.replace(")", "")[17:]
                    find_and_replace_sources(os.path.join(dir, newdir))

                if stripped.lower().startswith("add_library(") or stripped.lower().startswith("add_executable("):
                    source_files = list(Path(dir).rglob("*.[ch]*"))
                    for source in source_files:
                        outf.write("    \"" + os.path.relpath(source, dir) + "\"\n")
                    while inf.readline().strip() != ")":
                        pass
                    outf.write(")\n")                

    if Path(os.path.join(dir, "CMakeLists.txt.tmp")).exists():
        os.remove(os.path.join(dir, "CMakeLists.txt"))
        os.rename(os.path.join(dir, "CMakeLists.txt.tmp"), os.path.join(dir, "CMakeLists.txt"))
 

if __name__ == "__main__":
    find_and_replace_sources(os.getcwd())
        