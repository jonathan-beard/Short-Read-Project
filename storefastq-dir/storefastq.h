#ifndef __STOREFASTQ_H__
#define __STOREFASTQ_H__

#include <stdlib.h>
#include "X.h"

#ifndef APSTOP
#define APSTOP 1
#endif

#ifndef APMORE
#define APMORE 0
#endif

#ifndef SUCCESS
#define SUCCESS 0
#endif

#ifndef FAIL
#define FAIL -1
#endif


struct ap_storefastq_data {
   STRING filename;
   FILE* out;
   int num_inputs;
};

void ap_storefastq_init(struct ap_storefastq_data *block);
void ap_storefastq_destroy(struct ap_storefastq_data *block);
void ap_storefastq_push(struct ap_storefastq_data *block, int port, void *ptr, int count);
int ap_storefastq_go(struct ap_storefastq_data *block);
void ap_storefastq_push_signal(struct ap_storefastq_data *block, int port, int type, int value);

#endif /* END __STOREFASTQ_H__ */
