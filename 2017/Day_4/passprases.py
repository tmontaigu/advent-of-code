import sys
from collections import Counter

def parse_input(filename):
    with open(filename) as file:
        return file.read().splitlines()

def how_many_valid(lines):
    return sum(all(count == 1 for count in Counter(line.split()).values())
           for line in lines)

def any_anagram(line):
    if len(line) == len({tuple(sorted(word)) for word in line}):
        return False
    return True

def how_many_valid_part_2(lines):
    return sum(not any_anagram(line.split()) for line in lines)

def main():
    lines = parse_input(sys.argv[1])
    print("Num valid passphrases: {}".format(how_many_valid(lines)))
    print("Num valid passphrases: {}".format(how_many_valid_part_2(lines)))

if __name__ == '__main__':
    main()
