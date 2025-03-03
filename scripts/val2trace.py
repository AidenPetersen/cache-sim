import argparse

parser = argparse.ArgumentParser(
    prog="val2trace",
    description="Turns valgrind memory trace into cache-sim trace")

parser.add_argument('input_file')
parser.add_argument('output_file')

args = parser.parse_args()

with open(args.input_file, 'r') as in_file, open(args.output_file, 'w') as out_file:
    for line in in_file:
        [typ, addrsize] = line.strip().split(" ")
        [addr, size] = addrsize.split(",")
        if typ == "M":
            out_file.write(f"L {addr} {size}\n")
            out_file.write(f"S {addr} {size}\n")
        elif typ == "S":
            out_file.write(f"S {addr} {size}\n")
        elif typ == "L":
            out_file.write(f"S {addr} {size}\n")
