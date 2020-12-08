#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <vcruntime_string.h>

#include "aoc.h"


#define MAX_RULES 4
#define MAX_LEN_NAME 32


typedef struct {
	char name[MAX_LEN_NAME];
	unsigned int num;
} bag_rule_t;

typedef struct {
	char name[MAX_LEN_NAME];

	int num_rules;
	bag_rule_t rules[MAX_RULES];
} bag_t;

int parse_sub_rule(const char *begin, const char *end, bag_rule_t *rule) {
	if (begin == end)
	{
		return -1;
	}

	if (*begin == ' ')
	{
		begin++;
	}

	char *bag_description_start;
	unsigned int num = strtoul(begin, &bag_description_start, 10);

	if (*bag_description_start == ' ')
	{
		bag_description_start++;
	}

	const char *bag_description_end;
	if (num == 0)
	{
		fprintf(stderr, "0 is not a valid number\n");
		return -1;
	} else if (num == 1)
	{
		bag_description_end = find_word(bag_description_start, "bag", 3);
	} else
	{
		bag_description_end = find_word(bag_description_start, "bags", 4);
	}

	if (bag_description_end >= end)
	{
		fprintf(stderr, "Invalid rule\n");
		return -1;
	}

	if (bag_description_end - bag_description_start >= MAX_LEN_NAME)
	{
		fprintf(stderr, "Bag description is too long\n");
		return -1;
	}

	rule->num = num;
	strncpy(rule->name, bag_description_start, (bag_description_end - bag_description_start) - 1);
	rule->name[(bag_description_end - bag_description_start) - 1] = '\0';

	return 0;
}

int parse_rule(const char *line, bag_t *bag) {
	const char *bags_word = find_word(line, "bags", 4);
	if (!bags_word)
	{
		fprintf(stderr, "Invalid rule did not find the word 'bags'\n");
		return -1;
	}

	const char *contain_word = find_word(bags_word, "contain", 7);
	if (!contain_word)
	{
		fprintf(stderr, "Invalid rule did not find the word 'contain'\n");
		return -1;
	}

	const char *current_start = contain_word + 7 + 1;
	const char *current_end;
	bag->num_rules = 0;
	while (1)
	{
		if (bag->num_rules >= MAX_RULES)
		{
			fprintf(stderr, "Too many rules\n");
			return -1;
		}

		current_end = strchr(current_start, ',');

		if (!current_end)
		{
			current_end = strchr(current_start, '.');
			if (!current_end)
			{
				fprintf(stderr, "Invalid line, needs a closing '.'\n");
				return -1;
			}
		}

		if (strncmp(current_start, "no other bags.", strlen("no other bags.")) == 0)
		{
			break;
		} else if (parse_sub_rule(current_start, current_end, &bag->rules[bag->num_rules]) == -1)
		{
			return -1;
		}

		bag->num_rules++;
		current_start = current_end + 1;
		if (*current_end == '.')
		{
			break;
		}
	}
	bag->num_rules = bag->num_rules;

	strncpy(bag->name, line, (bags_word - line) - 1);
	bag->name[(bags_word - line) - 1] = '\0';
	return 0;
}


int can_reach_from(const bag_t *bags, unsigned int num_bags, const bag_t *source_bag, const char *bag_name) {
	if (strcmp(source_bag->name, bag_name) == 0)
	{
		return 1;
	}

	for (unsigned i = 0; i < source_bag->num_rules; ++i)
	{
		for (unsigned j = 0; j < num_bags; ++j)
		{
			if (strcmp(source_bag->rules[i].name, bags[j].name) == 0)
			{
				if (can_reach_from(bags, num_bags, &bags[j], bag_name))
				{
					return 1;
				}
			}
		}
	}
	return 0;
}

unsigned int count_individuable_bags(const bag_t *bags, unsigned int num_bags, const bag_t *source_bag) {
	unsigned int count = 1;

	for (unsigned i = 0; i < source_bag->num_rules; ++i)
	{
		for (unsigned j = 0; j < num_bags; ++j)
		{
			if (strcmp(source_bag->rules[i].name, bags[j].name) == 0)
			{
				count += (source_bag->rules[i].num * count_individuable_bags(bags, num_bags, &bags[j]));
			}
		}
	}
	return count;
}

int main(int argc, char *argv[]) {
	if (argc < 2)
	{
		fprintf(stderr, "USAGE: %s INPUT_FILE\n", argv[0]);
		return EXIT_FAILURE;
	}

	FILE *input_file = fopen(argv[1], "r");
	if (!input_file)
	{
		perror("Failed to open the file");
		return EXIT_FAILURE;
	}

	int num_lines = count_lines(input_file);

	if (fseek(input_file, 0, SEEK_SET))
	{
		fprintf(stderr, "Failed to seek in file\n");
		fclose(input_file);
		return EXIT_FAILURE;
	}

	bag_t *bags = malloc(num_lines * sizeof(bag_t));
	if (!bags)
	{
		fprintf(stderr, "Failed to allocate bags\n");
		fclose(input_file);
		return EXIT_FAILURE;
	}

	char line[BUFSIZ];
	unsigned int i = 0;
	while ((fgets(line, BUFSIZ, input_file) != NULL))
	{
		if (parse_rule(line, &bags[i]))
		{
			fprintf(stderr, "Failed to parse bag\n");
			abort();
		}
		i++;
	}

	unsigned int count = 0;
	unsigned int bags_within = 0;
	for (unsigned int j = 0; j < num_lines; ++j)
	{
		if (strcmp(bags[j].name, "shiny gold") != 0)
		{
			count += can_reach_from(bags, num_lines, &bags[j], "shiny gold");
		} else
		{
			bags_within = count_individuable_bags(bags, num_lines, &bags[j]) - 1;
		}
	}
	printf("count: %d\n", count);
	printf("bags within: %d\n", bags_within);


	fclose(input_file);
	free(bags);
	return 0;
}