def inverse_captcha(captcha):
    return sum(x for x, y in zip(captcha, captcha[1:] + [captcha[0]]) if x == y)

def main():
    captcha = input("Gimme de captcha:")
    answer = inverse_captcha(list(map(int, captcha)))
    print("The result is : {}".format(answer))


if __name__ == '__main__':
    main()