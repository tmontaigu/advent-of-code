#include <assert.h>
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>

#include "aoc.h"

#define BUF_SIZE 25

#define TRUE_TARGET 2020

int compare_uints(const void *a, const void *b) {
	unsigned int arg1 = *(const unsigned int *) a;
	unsigned int arg2 = *(const unsigned int *) b;

	if (arg1 < arg2) return -1;
	if (arg1 > arg2) return 1;
	return 0;
}



int read_numbers(FILE *file, unsigned int *number, unsigned int num_numbers) {
	char buf[BUF_SIZE];
	int i = 0;
	char *str_end;

	while ((fgets(buf, BUF_SIZE, file)) != NULL && (i < num_numbers))
	{
		number[i] = strtoul(buf, &str_end, 10);
		if (errno == ERANGE)
		{
			printf("range error");
			errno = 0;
		} else
		{
			++i;
		}
	}

	if (ferror(file))
	{
		return -1;
	}
	return i;
}

int find_number_in_sorted(const unsigned int *numbers, unsigned int num_numbers, unsigned target) {
	int pos = -1;
	for (int i = 0; i < num_numbers; ++i)
	{
		if (numbers[i] == target)
		{
			return i;
		}

		if (numbers[i] > target)
		{
			break;
		}
	}

	return pos;
}

typedef struct {
	int first;
	int second;
} pair_t;

pair_t find_numbers_that_adds(const unsigned int *numbers, unsigned int num_numbers, unsigned target) {
	pair_t result = {-1, -1};
	for (unsigned int i = 0; i < num_numbers; ++i)
	{
		unsigned num = numbers[i];
		if (num > target)
		{
			break;
		}

		unsigned complementary_num = target - num;

		int pos = find_number_in_sorted(numbers + i, num_numbers - i, complementary_num);
		if (pos > 0)
		{
			result.first = (int) i;
			result.second = pos;
		}
	}
	return result;
}

void find_numbers_that_adds_n(const unsigned int *numbers, unsigned int num_numbers, unsigned target) {


	for (unsigned int i = 0; i < num_numbers; ++i)
	{
		unsigned num = numbers[i];
		if (num > target)
		{
			break;
		}

		unsigned complementary_num = target - num;

		for (unsigned int j = i + 1; j < num_numbers; ++j)
		{
			unsigned num_2 = numbers[j];
			if (num_2 > complementary_num)
			{
				break;
			}

			unsigned complementary_num_2 = complementary_num - num_2;

			int pos = find_number_in_sorted(numbers + j, num_numbers - j, complementary_num_2);
			if (pos > 0) {
				printf("%d %d %d \n", num, num_2, numbers[pos + j]);
				return;
			}
		}

	}
}


int main(int argc, const char *argv[]) {
	if (argc != 2)
	{
		printf("Usage: %s INPUT_FILE\n", argv[0]);
		return EXIT_FAILURE;
	}

	FILE *input_file = fopen(argv[1], "r");
	if (!input_file)
	{
		perror("File opening failed");
		return EXIT_FAILURE;
	}


	int num_lines = count_lines(input_file);
	if (num_lines < 0)
	{
		printf("Failed to count lines");
		fclose(input_file);
		return EXIT_FAILURE;
	}

	printf("Counted %d lines\n", num_lines);

	if (fseek(input_file, 0, SEEK_SET))
	{
		printf("Failed to seek to start of file");
		fclose(input_file);
		return EXIT_FAILURE;
	}

	unsigned int *numbers = malloc(num_lines * sizeof(unsigned int));
	if (!numbers)
	{
		printf("Failed to allocate memory for the numbers");
		fclose(input_file);
		return EXIT_FAILURE;
	}

	int num_read = read_numbers(input_file, numbers, num_lines);
	assert(num_read == num_lines);
	printf("Successfully read %d numbers\n", num_read);

	qsort(numbers, num_read, sizeof(unsigned int), compare_uints);

	pair_t pos = find_numbers_that_adds(numbers, num_read, TRUE_TARGET);
	if (pos.first >= 0 && pos.second >= 0)
	{
		unsigned int first_num = numbers[pos.first];
		unsigned int second_num = numbers[pos.second];
		printf("The two numbers are %d %d -> their prod is %d\n", first_num, second_num, first_num * second_num);
	}
	else
	{
		printf("Could not find numbers matching the criteria");
	}
	find_numbers_that_adds_n(numbers, num_read, TRUE_TARGET);
	return 0;
}
