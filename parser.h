#ifndef _PHONEPARSER_H_
#define _PHONEPARSER_H_

#include <sys/types.h>
#include <stdio.h>


struct phone_data {
	char 		*name;
	char		*country_code;
	char		*area_code;
	char		*local_code;
	
	struct		phone_data *next;
	struct 		phone_data *prev;
};


struct phone_list {
	struct phone_data 	*p_data;
	
	int			(*add)(struct phone_list **p_list, const char *name, const char *raw_number_array);
	void			(*print)(struct phone_list **p_list);
	int			(*read_json)(struct phone_list *p_list, FILE *json_file);
	int			(*write_json)(struct phone_list *p_list, FILE *json_file);
	int			(*free)(struct phone_list **p_list);
};



extern const unsigned int country_code_size; 	// (+)	55
extern const unsigned int area_code_size;	//	543
extern const unsigned int local_code_size;	// 	3372660

/*	#	#	#	#	#	#	#	#	#	#	#	#	#
#													#
#						FUNCTIONS					#
#													#
#	#	#	#	#	#	#	#	#	#	#	#	#	*/

// phone_list nesnesini başlatmak için. liste içindeki 
// fonksiyon işaretçilerine ilk değerler burada atanır. 
int init_phone_list(struct phone_list **p_list);

//phone_list içindeki phone_data structı için alan tahsis eder.
struct phone_data *phone_data_calloc(void);


// add_number fonksiyonuna parametre geçirilen ham metni parçalayıp 
// phone_data içindeki ilgili alanlara vermek için hazırlayan ve geriye bu değerlerin toplamı
// uzunluğunda bir char array işaretçisi gönderen fonksiyon.
int extract_phone_number(struct phone_data *p_data, const char *raw_number_array);

/*
name ve raw_number_array parametrelerini işleyip bağlı lisede bir node oluşturup ona ekleyen bir fonksiyon.
*/
int add_number(struct phone_list **p_list, const char *name, const char *raw_number_array);



void print_phone_list(struct phone_list **p_list);
int read_from_json(struct phone_list *p_list, FILE *json_file);
int write_to_json(struct phone_list *p_list, FILE *json_file);

int free_phone_list(struct phone_list **p_list);
int phone_data_free(struct phone_data **p_data);

#endif
