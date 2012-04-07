#ifndef __STOREKSTAR_H__
#define __STOREKSTAR_H__

#include <stdlib.h>

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

#include "X.h"

struct ap_storekstar_data {
   STRING filename;
   FILE* out;
};

void ap_storekstar_init(struct ap_storekstar_data *block);
void ap_storekstar_destroy(struct ap_storekstar_data *block);
void ap_storekstar_push(struct ap_storekstar_data *block, int port, void *ptr, int count);
int ap_storekstar_go(struct ap_storekstar_data *block);
void ap_storekstar_push_signal(struct ap_storekstar_data *block, int port, int type, int value);
#endif /* END __STOREKSTAR_H__ */
