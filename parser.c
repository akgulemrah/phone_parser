#include <stdio.h>		/* printf 				*/
#include <stdlib.h>		/* malloc, calloc, system calls 	*/
#include <string.h>		/* strlen, strcat vs 			*/
#include <ctype.h>		/* isdigit				*/
#include <sys/types.h>		/* size_t				*/
#include "parser.h"

/*
 * This macro is just a simple code for debugging purposes. It is off by default (#define PRINT_STDERR_MSG_FLAG 0). 
 * To enable error messages in the file, just set the flag to 1.
 */
#define PRINT_STDERR_MSG_FLAG	0
#define PRINT_STDERR_MSG(var_name) 								\
		do {										\
			if (PRINT_STDERR_MSG_FLAG)						\
				fprintf(stderr, "Error, variable name: "			\
				"%s phoneparser.c header file line: %d\n", var_name, __LINE__); \
		} while(0) 									\


/*
 * The add_number function (struct phone_list->add) specifies the maximum length that the raw_phone_number 
 * parameter can be. Phone numbers can be surrounded by different characters when in a text, so you can change 
 * the length of the raw version of your variable before the digits are extracted. 
 */
#define MAX_RAW_PHONE_NUMBER	25


int init_phone_list(struct phone_list **p_list);
int phone_list_calloc(struct phone_list *p_list);
int extract_phone_number(struct phone_data *p_data, const char *extract_phone_number);
int add_number(struct phone_list **p_list, const char *name, const char *raw_number_array);
int phone_data_free(struct phone_data **p_data);
int free_phone_list(struct phone_list **p_list);
void print_phone_list(struct phone_list **p_list);



int init_phone_list(struct phone_list **p_list)
{
	/*
	phone_list nesnesini başlatmak için. liste içindeki 
	fonksiyon işaretçilerine ilk değerler burada atanır. 
	*/

	if (country_code_size < 1 && area_code_size < 1 && local_code_size < 1) {
		PRINT_STDERR_MSG("country_code_size || area_code_size || local_code_size");
		return -2;
	}
	
	if ((*p_list = (struct phone_list *)calloc(1, sizeof(struct phone_list))) == NULL) {
		PRINT_STDERR_MSG("p_list için hafıza tahsisi başarısız oldu");
		return -1;
	}

	(*p_list)->add = &add_number;
	(*p_list)->print = &print_phone_list;
	//p_list->read_json = &read_from_json;
	//p_list->write_json = &write_to_json;
	(*p_list)->free = &free_phone_list;

	return 0;
}



struct phone_data *phone_data_calloc(void)
{
	//phone_list içindeki phone_data structı için alan tahsis eder.
	
	struct phone_data *p_data = NULL;
	if ((p_data = (struct phone_data *)calloc(1, sizeof(struct phone_data))) == NULL) {
		PRINT_STDERR_MSG("phone_data_calloc içinde p_data için bellek tahsisi başarısız");
		return NULL;
	}
	
	if ((p_data->country_code = (char *)calloc(country_code_size, sizeof(char))) == NULL) {
		PRINT_STDERR_MSG("phone_data_calloc içinde country_code için bellek tahsisi başarısız");
		phone_data_free(&p_data);
		return NULL;
	}
	
	if ((p_data->area_code = (char *)calloc(area_code_size, sizeof(char))) == NULL) {
		PRINT_STDERR_MSG("phone_data_calloc içinde area_code için bellek tahsisi başarısız");
		phone_data_free(&p_data);
		return NULL;
	}
	
	if ((p_data->local_code = (char *)calloc(local_code_size, sizeof(char))) == NULL) {
		PRINT_STDERR_MSG("phone_data_calloc içinde local_code için bellek tahsisi başarısız");
		phone_data_free(&p_data);
		return NULL;
	}
	
	return p_data;
}


int extract_phone_number(struct phone_data *p_data, const char *raw_number_array)
{
	/*
	add_number fonksiyonuna parametre geçirilen ham metni parçalayıp 
 	phone_data içindeki ilgili alanlara vermek için hazırlayan ve geriye bu değerlerin toplamı
	uzunluğunda bir char array işaretçisi gönderen fonksiyon.
	*/
	int i = 0, j = 0;
    	char *phone_arr = NULL;
	unsigned int phone_buf_size = 0;
	size_t phone_number_size = 0;
 
	phone_arr = (char *)calloc((country_code_size + local_code_size + area_code_size + 1), sizeof(char));
	if (!phone_arr) {
		PRINT_STDERR_MSG("phone_arr");
		return -1;
	}
	
	phone_number_size = strlen(raw_number_array);
	for (i = 0; i < phone_number_size; i++) {
		if (isdigit(*raw_number_array)) {
			*(phone_arr + j)= *raw_number_array;
			j++;
        	}
		raw_number_array++;
	}
	*(phone_arr + j) = '\0';
	
	phone_buf_size = strlen(phone_arr);
	if (phone_buf_size != (country_code_size + local_code_size + area_code_size)) {
		PRINT_STDERR_MSG("country_code_size + local_code_size + area_code_size not aqual to phone number size!");
		free(phone_arr);
		return -2;
	}
	
	memmove(p_data->country_code, phone_arr, country_code_size);
	memmove(p_data->area_code, phone_arr + country_code_size, area_code_size);
	memmove(p_data->local_code, phone_arr + country_code_size + area_code_size, local_code_size);
	
	return 0;
}



int add_number(struct phone_list **p_list, const char *name, const char *raw_number_array)
{
	/*
	 name ve raw_number_array parametrelerini işleyip bağlı lisede bir node oluşturup ona ekleyen bir fonksiyon.
	*/
	
	struct phone_data *next_node;
	struct phone_data *iter;
	
	if (name == NULL || strlen(name) < 2) {
		PRINT_STDERR_MSG("add_number fpnksiyonunun parametresi olan name, geçerli bir parametre değil.");
		return -2;
	}
	
	
	if ((*p_list)->p_data == NULL) {
		(*p_list)->p_data = phone_data_calloc();
		if(!(*p_list)->p_data) {
			return -1;
		}
		
		if (((*p_list)->p_data->name = strdup(name)) == NULL) {
			PRINT_STDERR_MSG("name alanı doldurulamadı");
			phone_data_free(&(*p_list)->p_data);
			return -1;
		}
		
		if (extract_phone_number((*p_list)->p_data, raw_number_array) != 0) {
			phone_data_free(&(*p_list)->p_data);
			return -1;
		}
		(*p_list)->p_data->prev = NULL;
		(*p_list)->p_data->next = NULL;

		return 0;
	}

	if ((next_node = phone_data_calloc()) == NULL) {
		PRINT_STDERR_MSG("next_node için bellek tahsisi hatası");
		return -1;
	} else {
	
		if ((next_node->name = strdup(name)) == NULL) {
			PRINT_STDERR_MSG("name alanı doldurulamadı");
			phone_data_free(&next_node);
			return -1;
		}
		
		if (extract_phone_number(next_node, raw_number_array) != 0) {
			phone_data_free(&next_node);
			return -1;
		}

		iter = (*p_list)->p_data;
		while (iter->next != 0) {
			iter = iter->next;
		}

		next_node->prev = iter;
		next_node->next = NULL;
		iter->next = next_node;
		
		return 0;
	}
}


void print_phone_list(struct phone_list **p_list)
{
	struct phone_data *iter = NULL;
	iter = (*p_list)->p_data;
	while(iter != NULL) {
		printf("Name: %s\nCountry code: %s\nArea code: %s\nLocal code: %s\n\n",
						iter->name,
						iter->country_code,
						iter->area_code,
						iter->local_code);
		iter = iter->next;

	}
}



int phone_data_free(struct phone_data **p_data)
{
	if (*p_data) {
		if ((*p_data)->name) {
			free((*p_data)->name);
			(*p_data)->name = NULL;
		}
		
		if ((*p_data)->country_code) {
			free((*p_data)->country_code);
			(*p_data)->country_code = NULL;
		}
		
		if ((*p_data)->local_code) {
			free((*p_data)->local_code);
			(*p_data)->local_code = NULL;
		}
		
		if ((*p_data)->area_code) {
			free((*p_data)->area_code);
			(*p_data)->area_code = NULL;
		}
		free((*p_data));
		p_data = NULL;
	}
	return 0;
}

int free_phone_list(struct phone_list **p_list)
{
	int i = 0;
	struct phone_data *iter;
	while((*p_list)->p_data) {
		iter = (*p_list)->p_data;
		(*p_list)->p_data = (*p_list)->p_data->next;

		if (iter){
			if (iter->name) {
				free(iter->name);
				iter->name = NULL;
				printf("%d. iter->name free\n", i);
			}

			if (iter->country_code) {
				free(iter->country_code);
				iter->country_code = NULL;
			}

			if (iter->area_code) {
				free(iter->area_code);
				iter->area_code = NULL;
			}

			if (iter->local_code) {
				free(iter->local_code);
				iter->local_code = NULL;
			}
		}
		i++;
	}
	return 0;	
}
