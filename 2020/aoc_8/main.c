
#include <aoc.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>


typedef enum {
	NOP,
	ADD,
	JMP
} operation_t;


typedef struct {
	operation_t op;
	int arg;
	int executed;
} instruction_t;

typedef struct
{
	unsigned int num_instructions;
	instruction_t *instructions;
} program_t;


typedef struct {
	int return_value;
	int recursion_detected;
	int error;
} program_result_t;


// TODO free on error
int load_program(FILE *file, program_t *program) {
	program->num_instructions = count_lines(file);

	if (fseek(file, 0, SEEK_SET))
	{
		return 1;
	}

	program->instructions = malloc(program->num_instructions * sizeof(instruction_t));
	if (program->instructions == NULL)
	{
		fprintf(stderr, "Failed to allocate program\n");
		return 1;
	}

	char line[BUFSIZ];
	unsigned int current_instruction = 0;
	while (fgets(line, BUFSIZ, file))
	{
		const char *sep = strchr(line, ' ');
		instruction_t *curr_inst = &program->instructions[current_instruction];
		if (!sep)
		{
			fprintf(stderr, "Format should be '<op> <arg>'");
			return 1;
		}

		if (strncmp(line, "nop", 3) == 0)
		{
			curr_inst->op = NOP;
		} else if (strncmp(line, "acc", 3) == 0)
		{
			curr_inst->op = ADD;
		} else if (strncmp(line, "jmp", 3) == 0)
		{
			curr_inst->op = JMP;
		} else
		{
			fprintf(stderr, "Invalid operation '%.*s'", sep - line, line);
			return 1;
		}

		char *end;
		curr_inst->arg = strtol(sep + 1, &end, 10);
		curr_inst->executed = 0;

		current_instruction++;
	}

	if (ferror(file))
	{
		return -1;
	}
	return 0;
}

program_result_t execute_until_loop(const program_t *program) {
	program_result_t result = {0};

	if (program->num_instructions == 0)
	{
		return result;
	}

	const instruction_t *first_inst = &program->instructions[0];
	const instruction_t *last_inst = &program->instructions[program->num_instructions - 1];


	instruction_t *inst = &program->instructions[0];
	while (1)
	{
		int offset_to_next = 1;


		if (inst->executed == 1)
		{
			result.recursion_detected = 1;
			break;
		}

		if (inst->op == ADD)
		{
			result.return_value += inst->arg;
		} else if (inst->op == JMP)
		{
			offset_to_next = inst->arg;
		}
		inst->executed = 1;

		if (inst == last_inst)
		{
			break;
		}

		inst += offset_to_next;
		if (inst < first_inst || inst > last_inst)
		{
			fprintf(stderr, "Program jumps to non existing instruction\n");
			result.error = 1;
			break;
		}
	}
	return result;
}

void reset_execution_state(program_t *program)
{
	for (unsigned int i = 0; i < program->num_instructions; ++i)
	{
		program->instructions[i].executed = 0;
	}
}

void swap_nop_jmp(instruction_t* inst)
{
	if (inst->op == NOP)
	{
		inst->op = JMP;
	} else if (inst->op == JMP)
	{
		inst->op = NOP;
	}
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

	program_t program;
	if (load_program(input_file, &program))
	{
		return EXIT_FAILURE;
	}

	program_result_t result = execute_until_loop(&program);

	printf("Value is %d\n", result.return_value);

	for (unsigned int i = 0; i < program.num_instructions; ++i)
	{
		reset_execution_state(&program);
		instruction_t *inst = &program.instructions[i];
		swap_nop_jmp(inst);
		result = execute_until_loop(&program);
		swap_nop_jmp(inst);

		if (!result.recursion_detected)
		{
			printf("Value when no recursion: %d\n", result.return_value);
			break;
		}
	}

	fclose(input_file);
	free(program.instructions);
	return 0;
}