#include <assert.h>
#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>


#define NUM_SYMBOLS 26

typedef struct int_closure
{
	int value;
	int (*call)(struct int_closure *self, int);
} int_closure_t;

int is_int_equal(int_closure_t* self, int value)
{
	return self->value == value;
}

int is_int_not_equal(int_closure_t * self, int value)
{
	return self->value != value;
}

int count_if(const int *array, int len, int_closure_t *predicate) {
	int sum = 0;
	for (int i = 0; i < len; ++i)
	{
		sum += predicate->call(predicate, array[i]);
	}
	return sum;
}

void reset_array(int *array, int len)
{
	for (int i = 0; i < len; ++i)
	{
		array[i] = 0;
	}
}

int symbol_index(char symbol)
{
	if (isalpha(symbol))
	{
		return symbol - 'a';
	}
	return -1;
}

void count_symbols(const char *str, int symbol_count[NUM_SYMBOLS])
{
	const char *c = str;
	while (*c != '\0')
	{
		int index = symbol_index(*c);
		if (index != -1)
		{
			assert(index < NUM_SYMBOLS);
			symbol_count[index] += 1;
		}
		c++;
	}
}

int main(int argc, char * argv[]) {

	if (argc < 2)
	{
		fprintf(stderr, "Usage: %s INPUT_FILE\n", argv[0]);
		return EXIT_FAILURE;
	}

	FILE *input_file = fopen(argv[1], "r");
	if (input_file == NULL)
	{
		perror("Unable to open file");
		return EXIT_FAILURE;
	}

	char line[BUFSIZ];

	// We concat all the lines corresponding to
	// one group int this
	char group_data[BUFSIZ];
	char *current_pos = group_data;
	int num_members = 0;

	int symbol_count[NUM_SYMBOLS];
	reset_array(symbol_count, NUM_SYMBOLS);

	int answered_once_per_group = 0;
	int answered_by_all_group_members = 0;

	// The predicate we use to count
	int_closure_t predicate;

	while (1)
	{
		fgets(line, BUFSIZ, input_file);

		if (ferror(input_file))
		{
			fprintf(stderr, "Error while reading the file\n");
			break;
		}

		unsigned int line_len = strlen(line);
		if (line_len + (current_pos - group_data) > BUFSIZ)
		{
			fprintf(stderr, "Group data too long\n");
			break;
		}
		strcpy(current_pos, line);
		current_pos += (line_len == 1) ? line_len : line_len - 1;
		num_members += 1;

		if (line[0] == '\n' || line[0] == '\0' || ( strchr(line, '\n') == NULL ))
		{
			if (line[0] =='\n')
			{
				num_members--;
			}

			count_symbols(group_data, symbol_count);

			predicate.call = is_int_not_equal;
			predicate.value = 0;
			answered_once_per_group += count_if(symbol_count, NUM_SYMBOLS, &predicate);

			predicate.call = is_int_equal;
			predicate.value = num_members;
			answered_by_all_group_members += count_if(symbol_count, NUM_SYMBOLS, &predicate);

			reset_array(symbol_count, NUM_SYMBOLS);
			current_pos = group_data;
			num_members = 0;
		}

		if (feof(input_file))
		{
			break;
		}
	}

	printf("Total count is: %d\n", answered_once_per_group);
	printf("Total answered by all members count is: %d\n", answered_by_all_group_members);

	fclose(input_file);
	return 0;
}