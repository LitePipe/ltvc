# Convert LiteVector test files to libfuzz corpus examples
import os
from pathlib import Path

counter = 0
os.makedirs("corpus", exist_ok=True)

lines = Path("litevectors_positive.txt").read_text().splitlines()
for data in lines[1::2]:
    Path("corpus", str(counter) + ".bin").write_bytes(bytes.fromhex(data))
    counter += 1

lines = Path("litevectors_negative.txt").read_text().splitlines()
for data in lines[1::2]:
    Path("corpus", str(counter) + ".bin").write_bytes(bytes.fromhex(data))
    counter += 1
