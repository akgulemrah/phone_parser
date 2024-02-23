#ifndef _PHONEPARSER_H_
#define _PHONEPARSER_H_

#include <sys/types.h>
#include <stdio.h>

struct phone_data {
	char 	*name;
	char	*country_code;
	char	*area_code;
	char	*local_code;
	
	struct	phone_data *next;
	struct	phone_data *prev;
};

struct phone_list {
	struct phone_data 	*p_data;
	
	int	(*add)(struct phone_list **p_list, const char *name, const char *raw_number_array);
	void	(*print)(struct phone_list **p_list);
	int	(*read_json)(struct phone_list *p_list, FILE *json_file);
	int	(*write_json)(struct phone_list *p_list, FILE *json_file);
	int	(*free)(struct phone_list **p_list);
};

extern const unsigned int country_code_size; 	// (+)	90
extern const unsigned int area_code_size;	//	543
extern const unsigned int local_code_size;	// 	3372660

/*
 * Functions
 */
int init_phone_list(struct phone_list **p_list);
struct phone_data *phone_data_calloc(void);
int extract_phone_number(struct phone_data *p_data, const char *raw_number_array);
int add_number(struct phone_list **p_list, const char *name, const char *raw_number_array);
void print_phone_list(struct phone_list **p_list);
int read_from_json(struct phone_list *p_list, FILE *json_file);
int write_to_json(struct phone_list *p_list, FILE *json_file);
int free_phone_list(struct phone_list **p_list);
int phone_data_free(struct phone_data **p_data);

#endif /* _PHONEPARSER_H_ */
