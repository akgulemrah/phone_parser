#include <stdio.h>		/* printf 			*/
#include <stdlib.h>		/* malloc, calloc, system calls */
#include <string.h>		/* strlen, strcat vs 		*/
#include <ctype.h>		/* isdigit				*/
#include <sys/types.h>		/* size_t				*/
#include "phoneparser.h"


#define PRINT_STDERR_MSG_FLAG	1
#define PRINT_STDERR_MSG(var_name) 								\
		do {										\
			if (PRINT_STDERR_MSG_FLAG)						\
				fprintf(stderr, "Error, variable name: "			\
				"%s phoneparser.c header file line: %d\n", var_name, __LINE__); \
		} while(0) 									\

// ham telefon numarasının işlenip ayrıştırılmadan önceki maksimum uzunluğu. 
#define MAX_RAW_PHONE_NUMBER	25

struct phone_data {
	char 		*name;
	char		*country_code;
	char		*area_code;
	char		*local_code;
	
	struct		phone_data *next;
	struct 		phone_data *prev;
};

struct phone_list {
	struct phone_data *p_data;
	
	int		(*add)(struct phone_list *p_list, const char *name, const char *number_array);
	void		(*print)(struct phone_list *p_list);
	int		(*read_json)(struct phone_list *p_list, FILE *json_file);
	int		(*write_json)(struct phone_list *p_list, FILE *json_file);
	void		(*free)(struct phone_list *p_list);
};


int init_phone_list(struct phone_list);
int phone_list_calloc(struct phone_list *p_list);
int extract_phone_number(struct phone_data *p_data, const char *extract_phone_number);
int add_number(struct phone_list *p_list, const char *name, const char *raw_number_array);
void phone_data_free(phone_data *p_data);



int init_phone_list(phone_list *p_list)
{
	/*
	phone_list nesnesini başlatmak için. liste içindeki 
	fonksiyon işaretçilerine ilk değerler burada atanır. 
	*/

	if (country_code_size < 1 && area_code_size < 1 && local_code_size < 1) {
		PRINT_STDERR_MSG("country_code_size || area_code_size || local_code_size");
		return NULL;
	}
	
	if ((p_list = (phone_list *)calloc(1, sizeof(phone_list))) == NULL) {
		PRINT_STDERR_MSG("p_list için hafıza tahsisi başarısız oldu");
		return -1;
	}

	//p_list->add = &add_number;
	//p_list->print = &print_phone_list;
	//p_list->read_json = &read_from_json;
	//p_list->write_json = &write_to_json;
	//p_list->free = &free_phone_list;

	return phone_list_buf;
}



struct phone_data *phone_data_calloc(void);
{
	//phone_list içindeki phone_data structı için alan tahsis eder.
	
	struct phone_data *p_data = NULL;
	if ((p_data = (struct phone_data *)calloc(1, sizeof(struct phone_data))) == NULL) {
		PRINT_STDERR_MSG("phone_data_calloc içinde p_data için bellek tahsisi başarısız");
		return NULL;
	}
	
	if ((p_data->country_code = (char *)calloc(country_code_size, sizeof(char))) == NULL) {
		PRINT_STDERR_MSG("phone_data_calloc içinde country_code için bellek tahsisi başarısız");
		phone_data_free(p_data);
		return NULL;
	}
	
	if ((p_data->area_code = (char *)calloc(area_code_size, sizeof(char))) == NULL) {
		PRINT_STDERR_MSG("phone_data_calloc içinde area_code için bellek tahsisi başarısız");
		phone_data_free(p_data);
		return NULL;
	}
	
	if ((p_data->local_code = (char *)calloc(local_code_size, sizeof(char))) == NULL) {
		PRINT_STDERR_MSG("phone_data_calloc içinde local_code için bellek tahsisi başarısız");
		phone_data_free(p_data);
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
	
	phone_number_size = strlen(phone_number);
	for (i = 0; i < phone_number_size; i++) {
		if (isdigit(*phone_number)) {
			*(phone_arr + j)= *phone_number;
			j++;
        	}
		phone_number++;
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



int add_number(struct phone_list *p_list, const char *name, const char *raw_number_array)
{
	/*
	 name ve raw_number_array parametrelerini işleyip bağlı lisede bir node oluşturup ona ekleyen bir fonksiyon.
	*/
	
	phone_data *next_node;
	phone_data *iter;
	
	if (name == NULL || strlen(name) < 2) {
		PRINT_STDERR_MSG("add_number fpnksiyonunun parametresi olan name, geçerli bir parametre değil.");
		return -2;
	}
	
	
	if (p_list->p_data == NULL) {
		p_list->p_data = phone_data_calloc();
		if(!p_list->p_data) {
			return -1;
		}
		
		if ((p_list->p_data->name = strdup(name)) == NULL) {
			PRINT_STDERR_MSG("name alanı doldurulamadı");
			phone_data_free(p_list->p_data);
			return -1;
		}
		
		if (extract_phone_number(p_list->p_data, raw_number_array) != 0) {
			phone_data_free(p_list->p_data);
			return -1;
		}
		p_list->p_data->prev = p_list->p_data;
		p_list->p_data->next = p_list->p_data;
	}
	
	/* <<<------------------------- burada kaldımm    */
}


void phone_data_free(phone_data *p_data)
{
	if(p_data){
		if(p_data->name) {
			free(p_data->name);
			p_data->name = NULL;
		}
		
		if(p_data->country_code) {
			free(p_data->country_code);
			p_data->country_code = NULL;
		}
		
		if(p_data->local_code) {
			free(p_data->local_code);
			p_data->local_code = NULL;
		}
		
		if(p_data->area_code) {
			free(p_data->area_code);
			p_data->area_code = NULL;
		}
		free(p_data);
		p_data = NULL;
	}
}