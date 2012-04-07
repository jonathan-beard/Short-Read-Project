#include "X.h"

#ifndef APMORE
#define APMORE 0
#endif

#ifndef APSTOP
#define APSTOP 1
#endif


struct ap_kstarhash_data {
   uint64_t send_count;
   int32_t  num_hashes;
};

void ap_kstarhash_init(struct ap_kstarhash_data *block);
void ap_kstarhash_destroy(struct ap_kstarhash_data *block);
void ap_kstarhash_push(struct ap_kstarhash_data *block, int port, void *ptr, int count);
int ap_kstarhash_go(struct ap_kstarhash_data *block);
void ap_kstarhash_push_signal(struct ap_kstarhash_data *block, int port, int type, int value);
