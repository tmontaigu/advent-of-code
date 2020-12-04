import sys

def parse_input_file(filepath):
    numbers = []
    with open(sys.argv[1]) as file:
        for line in file.readlines():
            numbers.append(list(map(int, line.split())))
    return numbers


def checksum(lines):
    return sum(numbers[-1] - numbers[0] for numbers in map(sorted, lines))

def evenly_divide(a, b):
    return (a % b) == 0 or (b % a) == 0

def find_evenly_divisibles_values(line):
    for i, num in enumerate(line):
        for other in line[i+1:]:
            if evenly_divide(num, other):
                return (num, other) if num > other else (other, num)

def checksum_evenly(lines):
    evenly_divisibles = [find_evenly_divisibles_values(line) for line in lines]
    return sum(x / y for x, y in evenly_divisibles)


def main():
    lines = parse_input_file(sys.argv[1])
    cs = checksum(lines)
    cs_even = checksum_evenly(lines)
    print("The checksum is: {}".format(cs))
    print("The evenly divisibles checksum is: {}".format(cs_even))


if __name__ == '__main__':
    main()