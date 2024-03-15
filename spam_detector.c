// Boieriu Andrei-Teodor 312CA
#include "utils.h"

#include <stdio.h>
#include <dirent.h>
#include <stdlib.h>
#include <string.h>
#include <strings.h>
#include <stdbool.h>

#define EMAILS_DIR "data/emails/"
#define KEYWORDS_FILE "data/keywords"
#define ADD_KEYWORDS_FILE "additional_keywords"
#define SPAMMERS_FILE "data/spammers"
#define WORD_LEN 30

int main(void)
{
	keyword_arr keywords;
	keyword_arr add_keywords;
	email_arr emails;
	spammer_arr spammers;

	// read keywords and spammers
	read_keywords(KEYWORDS_FILE, &keywords);
	read_keywords(ADD_KEYWORDS_FILE, &add_keywords);
	read_spammers(SPAMMERS_FILE, &spammers);

	init_email_arr(&emails);

	double avg_size = 0;
	size_t file_count = 0;
	DIR *dirp;
	struct dirent *entry;

	// open email directory
	dirp = opendir(EMAILS_DIR);

	if (!dirp) {
		perror("opendir failed");
		exit(EXIT_FAILURE);
	}

	// iterate through email directory
	while ((entry = readdir(dirp))) {
		if (entry->d_type == DT_REG) {
			char path[256];
			strcpy(path, EMAILS_DIR);
			strcat(path, entry->d_name);

			// open email
			FILE *fp = fopen(path, "r");

			if (!fp) {
				perror("fopen failed");
				continue;
			}

			// resize arrays
			resize_arrays(file_count, &keywords, &add_keywords, &emails);

			strcpy(emails.array[file_count].name, entry->d_name);

			// read email
			read_mail(fp, file_count, emails, spammers, keywords, add_keywords,
					  &avg_size);

			file_count++;

			fclose(fp);
		}
	}

	avg_size /= file_count;

	// compute keyword scores
	compute_keyword_score(keywords, emails);
	compute_keyword_score(add_keywords, emails);

	for (size_t i = 0; i < file_count; i++) {
		emails.array[i].keyword_score *= avg_size;
		emails.array[i].keyword_score /= (double)emails.array[i].body_len;
	}

	// write results
	write_statistics(keywords);
	write_prediction(emails, file_count);

	closedir(dirp);

	// free resources
	free_keywords_arr(&keywords);
	free_keywords_arr(&add_keywords);
	free_spammer_arr(&spammers);
	free_email_arr(&emails);

	return 0;
}
