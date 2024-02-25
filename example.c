#include <stdio.h>
#include "parser.h"
				/* example: +90 525 3372557 */
const unsigned int country_code_size 	= 2; /* +90 */
const unsigned int area_code_size 	= 3; /* 525 */
const unsigned int local_code_size 	= 7; /* 3372567 */

int main(void)
{
	struct phone_list *list;
	int ret = init_phone_list(&list);
	if (list == NULL) {
		printf("işlem başarısız hacı\n");
		return 1;
	}
	list->add(&list, "Emrah", "+905dfv45g3372663");
	list->add(&list, "Cenk", "+905386d5789sd9f7");
	list->add(&list, "Ali", "lf90545şd887lkd3667");
	printf("ret: %d\n", ret);

	list->print(&list);

	list->free(&list);
	return 0;
}
