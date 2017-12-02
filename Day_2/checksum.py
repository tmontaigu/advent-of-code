import sys

def parse_input_file(filepath):
    numbers = []
    with open(sys.argv[1]) as file:
        for line in file.readlines():
            numbers.append(map(int, line.split()))
    return numbers


def checksum(lines):
    return sum(numbers[-1] - numbers[0] for numbers in map(sorted, lines))


def main():
    lines = parse_input_file(sys.argv[1])
    cs = checksum(lines)
    print("The checksum is: {}".format(cs))


if __name__ == '__main__':
    main()