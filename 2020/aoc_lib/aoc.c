#include <stdio.h>
#include <string.h>


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


const char *find_word(const char *string, const char *word, unsigned int word_len) {
	const char *first_letter_pos = string;

	const char * word_pos = NULL;

	while ((first_letter_pos = strchr(first_letter_pos, word[0])))
	{
		if (strncmp(first_letter_pos, word, word_len) == 0)
		{
			word_pos = first_letter_pos;
			break;
		}
		first_letter_pos++;
	}
	return word_pos;
}