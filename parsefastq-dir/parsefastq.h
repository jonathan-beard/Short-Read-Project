#ifndef __PARSEFASTQ_H__
#define __PARSEFASTQ_H__

#include "X.h"
#include "quickparse.h"

#ifndef APMORE
#define APMORE 0
#endif

#ifndef APSTOP
#define APSTOP 1
#endif


struct ap_parsefastq_data {
   STRING filename;
   SequenceData *sd;
   uint64_t num_reads_remaining;
};

void ap_parsefastq_init(struct ap_parsefastq_data *block);
void ap_parsefastq_destroy(struct ap_parsefastq_data *block);
void ap_parsefastq_push(struct ap_parsefastq_data *block, int port, void *ptr, int count);
int ap_parsefastq_go(struct ap_parsefastq_data *block);
void ap_parsefastq_push_signal(struct ap_parsefastq_data *block, int port, int type, int value);

#endif /* END __PARSEFASTQ_H__ */
