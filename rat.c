#include "string.h"
#include "stdio.h"
#include "stdlib.h"

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

int list_records() {	
	if (is_file_empty()) {
		puts("No records");
	}
	FILE* file;
	file = fopen(".rat", "r");
	char* line = NULL;
	size_t length = 0;
	ssize_t read;
	
	if (file == NULL) {
		puts("No records");
		free(line);
		return EXIT_FAILURE;
	}
	
	int i = 0;
		
	while ( (read = getline(&line, &length, file)) != -1) {
		if (strcmp(line, "\n") == 0) {
			continue;
		}
		i++;
		printf("\033[0;32m[%d] \033[0;37m%s", i, line);
	}
	printf("\n");

	
	free(line);
	fclose(file);
	return EXIT_SUCCESS;
}

int delete_record(int n) {
	FILE* file = fopen(".rat", "r");
	FILE* temp = fopen(".rat_temp", "a");

	if (file == NULL || temp == NULL) {
		puts("Unable to open files for deletion");
		if (file) fclose(file);
		if (temp) fclose(temp);
		return EXIT_FAILURE;
	}

	char* line = NULL;
	size_t length = 0;
	ssize_t read;
	int i = 0;
	
	while ( (read = getline(&line, &length, file)) != -1) {
		++i;
		if (i == n || strcmp(line, "\n") == 0) {
			continue;
		} else {
			fprintf(temp, "%s", line);
		}
	}

	free(line);
	fclose(file);
	fclose(temp);

	remove(".rat");
	rename(".rat_temp", ".rat");
	puts("Record was deleted");
	return EXIT_SUCCESS;
}

int create_record(char* text) {
	FILE* file = fopen(".rat", "a");
	if (file == NULL) {
		puts("Unable to open the file");
		return EXIT_FAILURE;
	}
	if (is_file_empty()) {
		fprintf(file, "%s", text);
	} else {
		fprintf(file, "\n%s", text);
	}
	fclose(file);
	puts("Record was created");
	return EXIT_SUCCESS;
}


int main(int argc, char** argv) {

	if (argc < 2) {
		list_records();
		return EXIT_FAILURE;
	}

	for (size_t i = 1; i < argc; ++i) {
		char* current = argv[i];

		if (strcmp(current, "-l") == 0) {
			list_records();
		} else if (strcmp(current, "-rm") == 0) {
			if (argc < 3) {
				puts("Provide record number");
				return EXIT_FAILURE;
			}

			delete_record(atoi(argv[2]));

			return EXIT_SUCCESS;
		} else if (strcmp(current, "-mk") == 0) {
			if (argc < 3) {
				puts("Provide record text");
				return EXIT_FAILURE;
			} else if (argc > 3) {
				puts("Too many arguements. Use \"\" for record content. Usage: rat -mk \"text\"");
				return EXIT_FAILURE;
			}
			char* record_text = argv[2];
			if (strcmp(record_text, "") == 0) {
				puts("Provide record text");
				return EXIT_FAILURE;
			}
			create_record(record_text);
			return EXIT_FAILURE;
		} else {
			puts("Unknown command.\n -l        to list records \n -rm <x>   to remove record with given number x \n -mk <\"x\"> to create a record");
		}

	}
}

