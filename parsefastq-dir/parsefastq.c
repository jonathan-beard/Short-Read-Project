#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <inttypes.h>

#include "nucformat.h"
#include "quickparse.h"
#include "parsefastq.h"



void ap_parsefastq_init(struct ap_parsefastq_data *block)
{
   block->sd = (SequenceData*) malloc(sizeof(SequenceData));
   block->sd->number_of_reads = 0;
   quickparse_init(block->filename,block->sd);
   quickparse_go(block->sd);
   block->num_reads_remaining = block->sd->number_of_reads;
}

void ap_parsefastq_destroy(struct ap_parsefastq_data *block)
{
   quickparse_destroy(block->sd);
};

void ap_parsefastq_push(struct ap_parsefastq_data *block, int port, void *ptr, int count){};

int ap_parsefastq_go(struct ap_parsefastq_data *block)
{
   if(block->num_reads_remaining > 0){
      const int max_zero = (ap_get_max_send(block,0) / READSTRUCTSIZE);
      const int max_one  = (ap_get_max_send(block,1) / READSTRUCTSIZE);
      int max = min(max_zero,max_one);
      if(max > block->num_reads_remaining)
         max = block->num_reads_remaining;
      const int num_bytes_to_send = max * READSTRUCTSIZE;
      void *read0 = ap_allocate(block,0,num_bytes_to_send);
      void *read1 = ap_allocate(block,1,num_bytes_to_send);
      memcpy(read0,block->sd->read_ptr,num_bytes_to_send);
      memcpy(read1,block->sd->read_ptr,num_bytes_to_send);
      block->num_reads_remaining -= max;
      (block->sd->read_ptr) += max;
      ap_send(block,0,num_bytes_to_send);
      ap_send(block,1,num_bytes_to_send);
      return (APMORE);
   }
   else
      return (APSTOP);
};

void ap_parsefastq_push_signal(struct ap_parsefastq_data *block, int port, int type, int value){};

