#include <assert.h>
#include <stdio.h>
#include <stdlib.h>


#define NUM_ROWS 128
#define NUM_COLS 8

#define NUM_PARTITIONINGS 10

typedef enum {
	FRONT,
	BACK,
	LEFT,
	RIGHT
} partitioning_t;

typedef struct {
	partitioning_t partitionings[NUM_PARTITIONINGS];
} boarding_pass_t;

int parse_boarding_pass_into(boarding_pass_t *pass, const char *line) {
	const char *c = line;
	unsigned int i = 0;
	while (*c != '\n' && *c != '\0')
	{
		if (i == NUM_PARTITIONINGS)
		{
			break;
		}
		if (*c == 'F')
		{
			pass->partitionings[i] = FRONT;
		} else if (*c == 'B')
		{
			pass->partitionings[i] = BACK;

		} else if (*c == 'R')
		{
			pass->partitionings[i] = RIGHT;
		} else if (*c == 'L')
		{
			pass->partitionings[i] = LEFT;
		} else
		{
			return -1;
		}
		c++;
		i++;
	}
	assert(i == 10);
	return 0;
}


int seat_id(const boarding_pass_t *pass) {
	unsigned int min_row = 0;
	unsigned int max_row = NUM_ROWS;

	unsigned int min_col = 0;
	unsigned int max_col = NUM_COLS;

	unsigned int row_len = max_row - min_row;
	unsigned int col_len = max_col - min_col;

	unsigned int i = 0;
	while (row_len > 1)
	{
		if (i == NUM_PARTITIONINGS)
		{
			break;
		}

		row_len = max_row - min_row;

		if (pass->partitionings[i] == FRONT)
		{
			max_row -= row_len / 2;

		} else if (pass->partitionings[i] == BACK)
		{
			min_row += row_len / 2;
		}
		row_len = max_row - min_row;
		i++;
	}

	while (col_len)
	{
		if (i == NUM_PARTITIONINGS)
		{
			break;
		}

		col_len = max_col - min_col;

		if (pass->partitionings[i] == LEFT)
		{
			max_col -= col_len / 2;

		} else if (pass->partitionings[i] == RIGHT)
		{
			min_col += col_len / 2;
		}
		col_len = max_col - min_col;
		i++;
	}

	return (min_row * NUM_COLS) + min_col;
}

int main(int argc, char *argv[]) {

	if (argc < 2)
	{
		fprintf(stderr, "Usage %s INPUT_FILE\n", argv[0]);
		return EXIT_FAILURE;
	}

	FILE *boarding_passes = fopen(argv[1], "r");
	if (!boarding_passes)
	{
		perror("Failed to open the file");
		return EXIT_FAILURE;
	}

	char line[BUFSIZ];
	boarding_pass_t current_pass;
	int max_seat_id = -1;

	int *seats = calloc(NUM_ROWS * NUM_COLS, sizeof(int));
	if (seats == NULL)
	{
		fprintf(stderr, "Could not allocate array of seats\n");
		fclose(boarding_passes);
		return EXIT_FAILURE;
	}

	while ((fgets(line, BUFSIZ, boarding_passes)) != NULL)
	{
		if (parse_boarding_pass_into(&current_pass, line) == -1)
		{
			continue;
		}

		int id = seat_id(&current_pass);
		seats[id] = 1;
		max_seat_id = max(id, max_seat_id);
	}


	printf("Max seat ID is %d\n", max_seat_id);

	for (unsigned int i = 2; i < NUM_ROWS - 1; ++i)
	{
		for (unsigned int j = 0; j < NUM_COLS - 0; ++j)
		{
			int id = (i * NUM_COLS) + j;
			if (seats[id] == 0 && seats[id + 1] && seats[id - 1])
			{
				printf("My Seat Id id %d\n", id);
				goto exit;
			}
		}
	}

exit:
	free(seats);
	fclose(boarding_passes);
	return EXIT_SUCCESS;
}