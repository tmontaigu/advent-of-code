#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <aoc.h>
#include <assert.h>

#define NUM_SLOPES 5
typedef struct {
	unsigned int move_right;
	unsigned int move_down;
} slope_t;

typedef enum { CELL_EMPTY,
	           CELL_TREE
} cell_t;


/// width ->
/// |
/// | height
typedef struct {
	unsigned int width;
	unsigned int height;

	cell_t *cells;
} map_t;

unsigned int cell_index(const map_t *map, unsigned int row, unsigned int col)
{
	return map->width * row + col;
}


int parse_map_from_file(const char *path, map_t *map) {
	FILE *map_file = fopen(path, "r");
	if (!map_file)
	{
		perror("Failed to open file\n");
		return -1;
	}

	int num_lines = count_lines(map_file);

	if (num_lines < 0)
	{
		fprintf(stderr, "Failed to count number of lines\n");
		goto failure;
	}

	if (fseek(map_file, 0, SEEK_SET))
	{
		fprintf(stderr, "Failed to seek after counting lines \n");
		goto failure;
	}

	map->height = num_lines;
	char line[BUFSIZ];

	int i = 0;
	char *current_char = NULL;
	while ((fgets(line, BUFSIZ, map_file)) != NULL)
	{
		if (map->cells == NULL)
		{
			// We are reading the first line,
			// Finish initializing the map
			map->width = strlen(line);
			if (map->width > 0)
			{
				// remove newline char
				map->width--;
			}

			map->cells = malloc(sizeof(cell_t) * map->width * map->height);
			if (map->cells == NULL)
			{
				fprintf(stderr, "Failed to allocate map of %d %d\n", map->width, map->height);
				goto failure;
			}
		}


		current_char = &line[0];
		while (*current_char != '\0' && *current_char != '\n') {
			assert(i < map->width * map->height);

			if (*current_char == '#')
			{
				map->cells[i] = CELL_TREE;
			} else
			{
				map->cells[i] = CELL_EMPTY;
			}
			i++;
			current_char++;
		}
		assert(i % map->width == 0);
	}
	assert(i == map->height * map->width);


	return 0;
failure:
	fclose(map_file);
	return -1;
}

unsigned int count_trees_on_path(const map_t *map, slope_t slope)
{
	int count = 0;
	unsigned int pos_w = 0;
	unsigned int pos_h = 0;

	while (pos_h < map->height - 1)
	{
		unsigned int new_pos_w = (pos_w + slope.move_right) % map->width;
		unsigned int new_pos_h = pos_h + slope.move_down;

		assert(cell_index(map, new_pos_h, new_pos_w) < map->height * map->width);
		if (map->cells[cell_index(map, new_pos_h, new_pos_w)] == CELL_TREE)
		{
			count += 1;
		}

		pos_w = new_pos_w;
		pos_h = new_pos_h;
	}
	return count;
}

int main(int argc, const char *argv[]) {
	if (argc < 2)
	{
		fprintf(stderr, "Usage %s INPUT_FILE\n", argv[0]);
		return EXIT_FAILURE;
	}


	map_t map = { 0 };
	if (parse_map_from_file(argv[1], &map)) {
		fprintf(stderr, "Failed to parse the map");
		return EXIT_FAILURE;
	}

	slope_t slopes_to_test[NUM_SLOPES] = {
			{ 1, 1 },
			{3, 1},
			{5, 1},
			{7, 1},
			{1, 2},
	};

	unsigned int product_result = 1;
	for (unsigned int i = 0; i < NUM_SLOPES; ++i)
	{
		unsigned int count = count_trees_on_path(&map, slopes_to_test[i]);
		printf("For the slope { %d, %d } I counted %d trees\n", slopes_to_test[i].move_right, slopes_to_test[i].move_down,count);
		product_result *= count;
	}

	printf("The product of the number of trees encountered is %u", product_result);

	free(map.cells);

	return EXIT_SUCCESS;
}