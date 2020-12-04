#include <assert.h>
#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>


typedef enum {
	BIRTH_YEAR = 0,
	ISSUE_YEAR,
	EXPIRATION_YEAR,
	HEIGHT,
	HAIR_COLOR,
	EYE_COLOR,
	PASSPORT_ID,
	COUNTRY_ID
} passport_fields_t;


void get_field_availability(const char *data, unsigned int size, int (*fields)[8], int do_checks) {
	for (int i = 0; i < 8; ++i)
	{
		(*fields)[i] = 0;
	}
	const char *field_start = data;
	const char *field_end;
	while ((field_end = strchr(field_start, ' ')) != NULL || (field_end = strchr(field_start, '\0')) != NULL)
	{
		const char *key_end = strchr(field_start, ':');
		if (key_end == NULL)
		{
			// With the way we parse, adn replace '\n' with ' '
			// the last field will have a ' '  at the end
			break;
		}

		int key_length = key_end - field_start;
		const char *value_start = key_end + 1;
		const char *value_end = strchr(value_start, ' ');
		if (value_end == NULL)
		{
			value_end = strchr(value_start, '\0');
			assert(value_end != NULL);
		}

		if (strncmp(field_start, "byr", key_length) == 0)
		{
			if (do_checks)
			{
				char *end;
				unsigned int value = strtoul(value_start, &end, 10);
				assert(end <= value_end);
				(*fields)[BIRTH_YEAR] = (value >= 1920 && value <= 2002);
			} else
			{
				(*fields)[BIRTH_YEAR] = 1;
			}
		} else if (strncmp(field_start, "iyr", key_length) == 0)
		{
			if (do_checks)
			{
				char *end;
				unsigned int value = strtoul(value_start, &end, 10);
				assert(end <= value_end);
				(*fields)[ISSUE_YEAR] = (value >= 2010 && value <= 2020);
			} else
			{
				(*fields)[ISSUE_YEAR] = 1;
			}
		} else if (strncmp(field_start, "eyr", key_length) == 0)
		{
			if (do_checks)
			{
				char *end;
				unsigned int value = strtoul(value_start, &end, 10);
				assert(end <= value_end);
				(*fields)[EXPIRATION_YEAR] = (value >= 2020 && value <= 2030);
			} else
			{
				(*fields)[EXPIRATION_YEAR] = 1;
			}
		} else if (strncmp(field_start, "hgt", key_length) == 0)
		{
			if (do_checks)
			{
				char *end;
				unsigned int value = strtoul(value_start, &end, 10);
				if (end <= value_end - 2)
				{
					if (end[0] == 'c' && end[1] == 'm')
					{
						(*fields)[HEIGHT] = (value >= 150 && value <= 193);
					} else if (end[0] == 'i' && end[1] == 'n')
					{
						(*fields)[HEIGHT] = (value >= 59 && value <= 76);
					}
				}
			} else
			{
				(*fields)[HEIGHT] = 1;
			}
		} else if (strncmp(field_start, "hcl", key_length) == 0)
		{
			if (do_checks)
			{
				if (value_end - value_start == 7)
				{
					if (value_start[0] == '#')
					{
						(*fields)[HAIR_COLOR] = 1;
						for (int i = 0; i < 6; ++i)
						{
							if (!isalnum(value_start[i + 1]))
							{
								(*fields)[HAIR_COLOR] = 0;
							}
						}
					} else
					{
						(*fields)[HAIR_COLOR] = 0;
					}
				} else
				{
					(*fields)[HAIR_COLOR] = 0;
				}
			} else
			{
				(*fields)[HAIR_COLOR] = 1;
			}
		} else if (strncmp(field_start, "ecl", key_length) == 0)
		{
			if (do_checks)
			{
				if (value_end - value_start != 3)
				{
					(*fields)[EYE_COLOR] = 0;
				} else
				{
					if (strncmp(value_start, "amb", 3) == 0 ||
					    strncmp(value_start, "blu", 3) == 0 ||
					    strncmp(value_start, "brn", 3) == 0 ||
					    strncmp(value_start, "gry", 3) == 0 ||
					    strncmp(value_start, "grn", 3) == 0 ||
					    strncmp(value_start, "hzl", 3) == 0 ||
					    strncmp(value_start, "oth", 3) == 0)
					{
						(*fields)[EYE_COLOR] = 1;
					}
				}
			} else
			{
				(*fields)[EYE_COLOR] = 1;
			}
		} else if (strncmp(field_start, "pid", key_length) == 0)
		{
			if (do_checks)
			{
				int count = 0;
				while (*value_start != '\0' && *value_start != ' ')
				{
					if (!isdigit(*value_start))
					{
						break;
					}
					count++;
					value_start++;
				}
				(*fields)[PASSPORT_ID] = count == 9;
			} else
			{
				(*fields)[PASSPORT_ID] = 1;
			}
		} else if (strncmp(field_start, "cid", key_length) == 0)
		{
			(*fields)[COUNTRY_ID] = 1;
		}

		field_start = field_end + 1;
	}
}

int is_q1_valid(const int (*fields)[8]) {
	// COUNTRY_ID is optional and its the
	// last element in the array
	// that's why we loop over only 7
	for (int i = 0; i < 7; i++)
	{
		if ((*fields)[i] != 1)
		{
			return 0;
		}
	}
	return 1;
}
unsigned int count_valid_passports(FILE *file) {
	unsigned int num_valid = 0;

	char line[BUFSIZ];
	char one_password_data[BUFSIZ];
	int current_passport_data_length = 0;
	char *current_pos = one_password_data;

	int field_validity[8] = {0};

	while (!feof(file))
	{
		current_pos = one_password_data;
		current_passport_data_length = 0;
		do {
			if (fgets(line, BUFSIZ, file) == NULL)
			{
				break;
			}
			if (line[0] == '\n')
			{
				break;
			}
			int line_length = strnlen(line, BUFSIZ);
			if (line_length + current_passport_data_length > BUFSIZ)
			{
				fprintf(stderr, "Passport data is too long\n");
				return num_valid;
			}

			strncpy(current_pos, line, BUFSIZ - current_passport_data_length);
			if (line[line_length - 1] == '\n')
			{
				current_pos += line_length - 1;
				*current_pos = ' ';
				current_pos++;
			} else{
				current_pos += line_length;
			}
				current_passport_data_length += line_length;

		} while (strcmp(line, "\n") != 0);

		//printf("LOL %s\n", one_password_data);
		// one_passport_data contains a full passport
		// We can now check which fields are present


		//get_field_availability(one_password_data, current_passport_data_length, &field_validity, 0);
		//num_valid += is_q1_valid(&field_validity);

		get_field_availability(one_password_data, current_passport_data_length, &field_validity, 1);
		num_valid += is_q1_valid(&field_validity);

		if (ferror(file))
		{
			fprintf(stderr, "Error reading line\n");
			return num_valid;
		}
	}

	return num_valid;
}


int main(int argc, char *argv[]) {
	if (argc < 2)
	{
		fprintf(stderr, "Usage: %s INPUT_FILE", argv[1]);
		return EXIT_FAILURE;
	}

	FILE *passport_file = fopen(argv[1], "r");
	if (!passport_file)
	{
		perror("Failed to open the file: ");
		return EXIT_FAILURE;
	}

	unsigned int num_valid = count_valid_passports(passport_file);
	printf("There are %u valid passports\n", num_valid);

	return 0;
}