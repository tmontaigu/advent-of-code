#include <stdio.h>



int count_lines(FILE *file) {
	int count = 1;
	int current_char;
	while ((current_char = fgetc(file)) != EOF)
	{
		if (current_char == '\n')
		{
			count++;
		}
	}

	if (!feof(file))
	{
		return -1;
	}

	return count;
}