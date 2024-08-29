#include "string.h"
#include "stdio.h"
#include "stdlib.h"

typedef struct {
	char** records;
	int size;
} Records; 

int is_file_empty() {
	FILE* file = fopen(".rat", "r");
	if (file == NULL) {
		return EXIT_FAILURE;
	}
	fseek(file, 0, SEEK_END);
	long file_size = ftell(file); 
	fclose(file);

	return (file_size == 0);
}

int init_records_file() {
	FILE* file = fopen(".rat", "a");
	if (file == NULL) {
		puts("Failed to open file");
		return EXIT_FAILURE;
	}
	fclose(file);
	return EXIT_SUCCESS;
}

int init_records(Records* records) {
	records->size = 0;
	records->records = malloc(sizeof(char*));
	return EXIT_SUCCESS;
}

int push_record(Records* records, char* record) {
	if (records == NULL || record == NULL) {
		puts("Failed to open file or read record");
		return EXIT_FAILURE;
	}

	records->records = realloc(records->records, (records->size + 1) * sizeof(char*));
	if (records->records == NULL) {
		puts("Failed to reallocate memory (read record)");
		return EXIT_FAILURE;
	}

	records->records[records->size] = malloc(strlen(record) + 1);
	if (records->records[records->size] == NULL) {
		puts("Failed to allocate memory (add record)");
		return EXIT_FAILURE;
	}

	strcpy(records->records[records->size], record);
	records->size++;
	
	return EXIT_SUCCESS;
}

int remove_record(Records* records, int n) {
	if (records->size < 0 || n >= records->size) {
		puts("Can't remove record. Id is out of range");
		return EXIT_FAILURE;
	}

	free(records->records[n]);

	for (int i = n; i < records->size - 1; ++i) {
		records->records[i] = records->records[i + 1];
	}

	records->size--;
	
	return EXIT_SUCCESS;
}

int load_records(Records* records) {
	FILE* file = fopen(".rat", "r");
	if (file == NULL) {
		puts("Failed to read records. Try creating one");
		return EXIT_FAILURE;
	}

	char* line = NULL;
	size_t length = 0;
	ssize_t read;

	while ( (read = getline(&line, &length, file)) != -1 ) {
		if (strcmp(line, "\n") == 0) {
			continue;
		} else if (line[read - 1] == '\n') {
			line[read - 1] = '\0';
		}

		push_record(records, line);
	}

	fclose(file);
	return EXIT_SUCCESS;
}

int write_records(Records* records) {
	FILE* file = fopen(".rat", "w");
	if (file == NULL) {
		puts("Failed to open file");
		return EXIT_FAILURE;
	}

	for (int i = 0; i < records->size; i++) {
		fprintf(file, "%s\n", records->records[i]);
		free(records->records[i]);
	}
	
	free(records->records);
	fclose(file);
	return EXIT_SUCCESS;
}

int list_records(Records* records) {
	if (records->size > 0) {
		for (int i = 0; i < records->size; ++i) {
			printf("[%d] %s\n", i + 1, records->records[i]);
		}
	} else {
		puts("No records. Try creating one");
	}
	return EXIT_SUCCESS;
}



int main(int argc, char** argv) {
	Records records;
	init_records_file();
	init_records(&records);
	load_records(&records);

	
	if (argc < 2) {
		list_records(&records);
		return EXIT_FAILURE;
	}

	for (size_t i = 1; i < argc; ++i) {
		char* current = argv[i];

		if (strcmp(current, "-l") == 0) {
			list_records(&records);
		} else if (strcmp(current, "-rm") == 0) {
			if (argc < 3) {
				puts("Provide record number");
				return EXIT_FAILURE;
			}

			remove_record(&records, atoi(argv[2]) - 1); // -1 for 0 based indexing

			write_records(&records);
			return EXIT_SUCCESS;
		} else if (strcmp(current, "-mk") == 0) {
			if (argc < 3) {
				puts("Provide record text");
				return EXIT_FAILURE;
			} else if (argc > 3) {
				puts("Too many arguements. Use \"\" for record content. Usage: rat -mk \"text\"");
				return EXIT_FAILURE;
			}

			if (strcmp(argv[2], "") == 0) {
				puts("Provide record text");
				return EXIT_FAILURE;
			}
			push_record(&records, argv[2]);
			write_records(&records);
			return EXIT_SUCCESS;
		} else {
			puts("Unknown command.\n -l        to list records \n -rm <x>   to remove record with given number x \n -mk <\"x\"> to create a record");
		}

	}
}

