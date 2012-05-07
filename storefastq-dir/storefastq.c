#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <stdint.h>
#include <inttypes.h>

#include "storefastq.h"
#include "nucformat.h"


void ap_storefastq_init(struct ap_storefastq_data *block){
   block->out = fopen(block->filename, "w");
   if(block->out == NULL){
      printf("ERROR creating output file\n");
      exit(FAIL);
   }
   block->num_inputs = 0;
}

void ap_storefastq_destroy(struct ap_storefastq_data *block){
   fflush(block->out);
   fclose(block->out);
}

void ap_storefastq_push(struct ap_storefastq_data *block, int port, void *ptr, int count){
#if(DEBUG == 1) /* store text to debug */
   const int num_read_inputs = (count / READSTRUCTSIZE);
   int i;
   Read *read = (Read*) ptr;
   for(i = 0; i < num_read_inputs; i++)
   {
      fprintf(block->out,"READID: %"PRIx32"\n",(uint32_t)read->read_index.read_index);
      fprintf(block->out,"LENGTH: %"PRIi8"\n",(uint8_t)read->read_length);
      fprintf(block->out,"READ:   \n");
      int j;
      for(j = 0; j < READ_ARRAY_LENGTH; j++)
         fprintf(block->out,"%"PRINUCREADT"\n",read->read[j]);
      fprintf(block->out,"\n");
#if (USEPHRED == 1)
      fprintf(block->out,"RSCORE: \n");
      char *score = (char*)read->read_score;
      int k;
      for(k = 0; k < NUMBER_OF_BASES_CUTOFF; k++)
      {
         fprintf(block->out,"%c",score[k]);
      }
      fprintf(block->out,"\n");
#endif
      read++;
   }
   block->num_inputs += num_read_inputs;
   ap_release(block,0,num_read_inputs * READSTRUCTSIZE);
#else /* store binary */   
   fwrite(ptr,sizeof(UNSIGNED8),count,block->out);
   ap_release(block,0,count);
#endif
}

int ap_storefastq_go(struct ap_storefastq_data *block){
   return (APSTOP);
}


void ap_storefastq_push_signal(struct ap_storefastq_data *block, int port, int type, int value)
{
}
