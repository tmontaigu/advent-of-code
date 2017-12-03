from typing import List
from itertools import cycle, islice

def parse_input(input: str) -> List[int]:
    return list(map(int, input))

def inverse_captcha(captcha):
    return sum(x for x, y in zip(captcha, captcha[1:] + [captcha[0]]) if x == y)

def inverse_captcha_halfway_around(captcha: List[int]) -> int:
    halfway = int(len(captcha) / 2) 
    captcha_cycle = islice(cycle(captcha), halfway, None)
    return sum(x for x, y in zip(captcha, captcha_cycle) if x == y)


def main():
    captcha = parse_input(input("Gimme de captcha:"))
    answer = inverse_captcha(captcha)
    answer_halfway = inverse_captcha_halfway_around(captcha)
    print("The result is : {}".format(answer))
    print("The halfway around captch  is : {}".format(answer_halfway))


if __name__ == '__main__':
    main()