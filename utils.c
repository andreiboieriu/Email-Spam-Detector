// Boieriu Andrei-Teodor 312CA
#include "utils.h"

#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include <string.h>
#include <ctype.h>
#include <stdbool.h>

#define INITIAL_SIZE 16
#define THRESHOLD 77
#define MARKDOWN_THRESHOLD 4

// memory allocation function for keyword_arr struct
void alloc_keywords_arr(keyword_arr *keywords)
{
	keywords->array = malloc(keywords->arr_size * sizeof(char[KEYWORD_LEN]));

	if (!keywords->array) {
		perror("malloc failed");
		exit(EXIT_FAILURE);
	}

	keywords->stdev = malloc(keywords->arr_size * sizeof(double));

	if (!keywords->stdev) {
		perror("malloc failed");
		exit(EXIT_FAILURE);
	}

	keywords->ap_count = malloc(keywords->arr_size * sizeof(size_t *));

	if (!keywords->ap_count) {
		perror("malloc failed");
		exit(EXIT_FAILURE);
	}

	for (size_t i = 0; i < keywords->arr_size; i++) {
		keywords->ap_count[i] = calloc(INITIAL_SIZE, sizeof(size_t));

		if (!keywords->ap_count[i]) {
			perror("calloc failed");
			exit(EXIT_FAILURE);
		}
	}

	keywords->ap_count_size = INITIAL_SIZE;
}

// resize function for keyword_arr
void resize_ap_count(keyword_arr *keywords, size_t new_size)
{
	for (size_t i = 0; i < keywords->arr_size; i++) {
		size_t *p = realloc(keywords->ap_count[i], new_size * sizeof(size_t));

		if (!p) {
			perror("realloc failed");
			exit(EXIT_FAILURE);
		}

		keywords->ap_count[i] = p;

		for (size_t j = keywords->ap_count_size; j < new_size; j++)
			keywords->ap_count[i][j] = 0;
	}

	keywords->ap_count_size = new_size;
}

// function that reads keywords from file
void read_keywords(char *filename, keyword_arr *keywords)
{
	FILE *fp = fopen(filename, "r");

	if (!fp) {
		perror("fopen failed");
		exit(EXIT_FAILURE);
	}

	fscanf(fp, "%zu", &keywords->arr_size);
	fseek(fp, 1, SEEK_CUR);

	char *k_word = NULL;
	size_t buffer_size = 0;

	alloc_keywords_arr(&(*keywords));

	for (size_t i = 0; i < keywords->arr_size; i++) {
		getline(&k_word, &buffer_size, fp);

		if (k_word[strlen(k_word) - 1] == '\n')
			k_word[strlen(k_word) - 1] = 0; // remove newline

		strcpy(keywords->array[i], k_word);
	}

	free(k_word);

	fclose(fp);
}

// function that computes the standard deviation for a keyword
double compute_stdev(size_t file_count, keyword_arr keywords, size_t index)
{
	double average = (double)keywords.ap_count[index][0] / file_count;

	double stdev = 0;

	for (size_t i = 1; i <= file_count; i++)
		stdev += pow((double)keywords.ap_count[index][i] - average, 2);

	stdev = sqrt(stdev / file_count);

	return stdev;
}

// function that frees allocated memory for a keyword_arr
void free_keywords_arr(keyword_arr *keywords)
{
	free(keywords->array);
	keywords->array = NULL;

	for (size_t i = 0; i < keywords->arr_size; i++) {
		free(keywords->ap_count[i]);
		keywords->ap_count[i] = NULL;
	}

	free(keywords->ap_count);

	keywords->ap_count = NULL;

	free(keywords->stdev);

	keywords->stdev = NULL;

	keywords->ap_count_size = 0;
	keywords->arr_size = 0;
}

// function that makes all characters lower case and counts upper case
void to_lower_string(char *string, email_arr emails, size_t index)
{
	for (size_t i = 0; i < strlen(string); i++) {
		if (string[i] >= 'A' && string[i] <= 'Z') {
			string[i] = tolower(string[i]);
			emails.array[index].caps_count++;
		}
	}
}

// function that initializes an email_arr
void init_email_arr(email_arr *emails)
{
	emails->array = calloc(1, sizeof(email_file));

	if (!emails->array) {
		perror("malloc failed");
		exit(EXIT_FAILURE);
	}

	emails->size = 1;
}

// function that resizes an email_arr
void resize_email_arr(email_arr *emails, size_t new_size)
{
	email_file *p = realloc(emails->array, new_size * sizeof(email_file));

	if (!p) {
		perror("realloc failed");
		exit(EXIT_FAILURE);
	}

	emails->array = p;

	for (size_t i = emails->size; i < new_size; i++) {
		emails->array[i].body_len = 0;
		emails->array[i].caps_count = 0;
		emails->array[i].spammer_score = 0;
		emails->array[i].keyword_score = 0;
		emails->array[i].spam = 0;
		emails->array[i].markdown_count = 0;
	}

	emails->size = new_size;
}

// function that initializes a spammer_arr
void init_spammer_arr(spammer_arr *spammers, size_t size)
{
	spammers->array = malloc(size * sizeof(spammer));

	if (!spammers->array) {
		perror("malloc failed");
		exit(EXIT_FAILURE);
	}

	spammers->size = size;
}

// function that reads spammer data from a file
void read_spammers(char *filename, spammer_arr *spammers)
{
	FILE *fp = fopen(filename, "r");

	if (!fp) {
		perror("fopen failed");
		exit(EXIT_FAILURE);
	}

	size_t size;

	fscanf(fp, "%zu", &size);

	init_spammer_arr(&(*spammers), size);

	for (size_t i = 0; i < size; i++) {
		fscanf(fp, "%s", spammers->array[i].address);
		fscanf(fp, "%zu", &spammers->array[i].score);
	}

	fclose(fp);
}

// function that calculates final score for an email
double final_score(email_file email)
{
	return email.keyword_score * 10 +
		   (email.caps_count > email.body_len / 3) * 70 +
		   email.spammer_score * 2.5;
}

// function that calculates if an email is spam or not
bool is_spam(email_file email)
{
	if (email.spam == 5)
		return false;

	if (email.markdown_count > MARKDOWN_THRESHOLD)
		return false;

	double score = final_score(email);

	if (score > THRESHOLD)
		return true;

	return false;
}

// function that frees allocated memory for an email_arr
void free_email_arr(email_arr *emails)
{
	free(emails->array);
	emails->array = NULL;
	emails->size = 0;
}

// function that frees allocated memory for a spammer_arr
void free_spammer_arr(spammer_arr *spammers)
{
	free(spammers->array);
	spammers->array = NULL;
	spammers->size = 0;
}

// function that writes statistics to a file
void write_statistics(keyword_arr keywords)
{
	FILE *fp = fopen("statistics.out", "w");

	for (size_t i = 0; i < keywords.arr_size; i++) {
		fprintf(fp, "%s ", keywords.array[i]);
		fprintf(fp, "%zu ", keywords.ap_count[i][0]);
		fprintf(fp, "%lf\n", keywords.stdev[i]);
	}

	fclose(fp);
}

// function that writes predictions to a file
void write_prediction(email_arr emails, size_t file_count)
{
	FILE *fp = fopen("prediction.out", "w");

	for (size_t i = 0; i < file_count; i++) {
		for (size_t j = 0; j < file_count; j++) {
			if ((size_t)atoi(emails.array[j].name) == i) {
				fprintf(fp, "%d\n", is_spam(emails.array[j]));

				break;
			}
		}
	}

	fclose(fp);
}

// function that calculates keyword scores for a keyword_arr
void compute_keyword_score(keyword_arr keywords, email_arr emails)
{
	for (size_t i = 0; i < keywords.arr_size; i++) {
		keywords.stdev[i] = compute_stdev(emails.size, keywords, i);

		if (keywords.arr_size < 90 && !strcmp(keywords.array[i], "click"))
			continue;

		for (size_t j = 1; j <= emails.size; j++)
			emails.array[j - 1].keyword_score += keywords.ap_count[i][j] *
												 keywords.stdev[i];
	}
}

// function that reads an email
void read_mail(FILE *fp, size_t file_count, email_arr emails,
			   spammer_arr spammers, keyword_arr keywords,
			   keyword_arr add_keywords, double *avg_size)
{
	char *line = NULL;
	size_t buffer_size = 0;
	bool read_from = false;

	while (getline(&line, &buffer_size, fp) != -1) {
		if (!read_from) {
			if (strstr(line, "Subject:")) {
				check_special_cases_s(line, emails, file_count);
			} else if (strstr(line, "From:")) {
				for (size_t i = 0; i < spammers.size; i++)
					if (strstr(line, spammers.array[i].address))
						emails.array[file_count].spammer_score =
						spammers.array[i].score;

				to_lower_s(line);
				check_special_cases_f(line, emails, file_count);

				read_from = true;
			}
		} else {
			to_lower_string(line, emails, file_count);

			check_special_cases_b(line, emails, file_count);

			*avg_size += strlen(line);
			emails.array[file_count].body_len += strlen(line);

			count_keywords(line, keywords, file_count);
			count_add_keywords(line, add_keywords, file_count);
		}
	}

	free(line);
}

// function that counts keywords in a string
void count_keywords(char *string, keyword_arr keywords, size_t index)
{
	for (size_t i = 0; i < keywords.arr_size; i++) {
		char *temp = strstr(string, keywords.array[i]);
		while (temp) {
			keywords.ap_count[i][index + 1]++;
			keywords.ap_count[i][0]++;
			temp = strstr(temp + 1, keywords.array[i]);
		}
	}
}

// function that counts additional keywords in a string
void count_add_keywords(char *string, keyword_arr keywords, size_t index)
{
	for (size_t i = 0; i < keywords.arr_size; i++) {
		// ignore "leeft click" and "right click"
		if (!strcmp(keywords.array[i], "click")) {
			size_t count = 0;

			char left[] = "left click";
			char right[] = "right click";

			char *temp = strstr(string, keywords.array[i]);
			while (temp) {
				count++;
				temp = strstr(temp + 1, keywords.array[i]);
			}

			temp = strstr(string, left);
			while (temp) {
				count--;
				temp = strstr(temp + 1, left);
			}

			temp = strstr(string, right);
			while (temp) {
				count--;
				temp = strstr(temp + 1, right);
			}

			keywords.ap_count[i][index + 1] += count;
			keywords.ap_count[i][0] += count;
		} else {
			char *temp = strstr(string, keywords.array[i]);
			while (temp) {
				keywords.ap_count[i][index + 1]++;
				keywords.ap_count[i][0]++;
				temp = strstr(temp + 1, keywords.array[i]);
			}
		}
	}
}

// function that checks special cases for an email body
void check_special_cases_b(char *string, email_arr emails, size_t index)
{
	if (strstr(string, "yahoo! groups sponsor"))
		emails.array[index].spam = 5;

	if (strstr(string, "pgp signature"))
		emails.array[index].spam = 5;

	if (strstr(string, "email is sponsored by"))
		emails.array[index].spam = 5;

	if (strchr(string, '>'))
		emails.array[index].markdown_count++;
}

// function that checks special cases for an email subject
void check_special_cases_s(char *string, email_arr emails, size_t index)
{
	if (strstr(string, "Re:"))
		emails.array[index].spam = 5;
}

// function that checks special cases for an email from: section
void check_special_cases_f(char *string, email_arr emails, size_t index)
{
	if (strstr(string, "rssfeeds"))
		emails.array[index].spam = 5;
}

// function that makes all characters in a string lower case
void to_lower_s(char *string)
{
	for (size_t i = 0; i < strlen(string); i++)
		string[i] = tolower(string[i]);
}

// function that resizes keyword and email arrays
void resize_arrays(size_t file_count, keyword_arr *keywords,
				   keyword_arr *add_keywords, email_arr *emails)
{
	if (file_count + 1 >= keywords->ap_count_size)
		resize_ap_count(&(*keywords), 2 * keywords->ap_count_size);

	if (file_count + 1 >= add_keywords->ap_count_size)
		resize_ap_count(&(*add_keywords), 2 * add_keywords->ap_count_size);

	if (file_count >= emails->size)
		resize_email_arr(&(*emails), file_count + 1);
}
