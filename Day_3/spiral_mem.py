import math
import sys

def nearest_perfect_odd_square(number: int) -> int:
    num = int(math.sqrt(number))
    num = num + 1 if num % 2 == 0 else num
    return num ** 2


def main():
    number = int(sys.argv[1])
    perfect_square = nearest_perfect_odd_square(number)

    perfect_square_dist_to_square_1 = math.sqrt(perfect_square) - 1
    distance_number_perfect_square = perfect_square - number
    if perfect_square > number:
        num_steps = perfect_square - number - perfect_square_dist_to_square_1
    else:
        num_steps = number - perfect_square + perfect_square_dist_to_square_1
    print(perfect_square, perfect_square_dist_to_square_1)
    print(distance_number_perfect_square)

    print("Data from square {} is {} steps away".format(number, num_steps))


if __name__ == '__main__':
    main()