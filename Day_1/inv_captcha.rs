use std::io;



fn parse_input() -> Vec<u8> {
    let mut input = String::new();
    println!("Enter captcha: ");

    let chars_read = io::stdin().read_line(&mut input).unwrap();
    let mut numbers = Vec::<u8>::with_capacity(chars_read);
    for c in input.chars() {
        match c.to_string().parse::<u8>() {
            Ok(number) => numbers.push(number),
            Err(_) => {},
        }
    }
    numbers
}

fn captcha_solution(numbers: Vec<u8>) -> i32{
    let mut answer: i32 = 0;
    for x_y in numbers.windows(2) {
        if x_y[0] == x_y[1] {
            answer += x_y[0] as i32;
        }
    }

    if numbers[0] == numbers[numbers.len() -1] {
        answer += numbers[0] as i32;
    }
    answer
}

fn main() {
    let numbers = parse_input();
    println!("codes : {:?}", numbers);

    let answer = captcha_solution(numbers);
    println!("The answer is: {}", answer);
}