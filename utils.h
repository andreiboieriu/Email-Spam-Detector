// Boieriu Andrei-Teodor 312CA
#pragma once

#include <stdlib.h>
#include <stdbool.h>
#include <stdio.h>

#define KEYWORD_LEN 45
#define NAME_LEN 256

typedef struct {
	char (*array)[KEYWORD_LEN];
	size_t **ap_count;
	size_t arr_size;
	size_t ap_count_size;
	double *stdev;
} keyword_arr;

typedef struct {
	char name[NAME_LEN];
	size_t keyword_count;
	size_t body_len;
	size_t caps_count;
	size_t spammer_score;
	size_t markdown_count;
	double keyword_score;
	int spam;
} email_file;

typedef struct {
	char address[256];
	size_t score;
} spammer;

typedef struct {
	spammer *array;
	size_t size;
} spammer_arr;

typedef struct {
	email_file *array;
	size_t size;
} email_arr;

void alloc_keywords_arr(keyword_arr *keywords);

void resize_ap_count(keyword_arr *keywords, size_t new_size);

void read_keywords(char *filename, keyword_arr *keywords);

double compute_stdev(size_t file_count, keyword_arr keywords, size_t index);

void free_keywords_arr(keyword_arr *keywords);

void to_lower_string(char *string, email_arr emails, size_t index);

void init_email_arr(email_arr *emails);

void resize_email_arr(email_arr *emails, size_t new_size);

void read_spammers(char *filename, spammer_arr *spammers);

double final_score(email_file email);

void free_spammer_arr(spammer_arr *spammers);

void free_email_arr(email_arr *emails);

bool is_spam(email_file email);

void write_statistics(keyword_arr keywords);

void write_prediction(email_arr emails, size_t file_count);

void compute_keyword_score(keyword_arr keywords, email_arr emails);

void read_mail(FILE *fp, size_t file_count, email_arr emails,
			   spammer_arr spammers, keyword_arr keywords,
			   keyword_arr add_keywords, double *avg_size);

void count_keywords(char *string, keyword_arr keywords, size_t index);

void count_add_keywords(char *string, keyword_arr keywords, size_t index);

void check_special_cases_b(char *string, email_arr emails, size_t index);

void check_special_cases_s(char *string, email_arr emails, size_t index);

void check_special_cases_f(char *string, email_arr emails, size_t index);

void to_lower_s(char *string);

void resize_arrays(size_t file_count, keyword_arr *keywords,
				   keyword_arr *add_keywords, email_arr *emails);
