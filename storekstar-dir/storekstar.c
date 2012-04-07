#define _GNU_SOURCE

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <stdint.h>
#include <inttypes.h>

#include "storekstar.h"
#include "nucformat.h"

void printkstar(K *k,char **buffer);

void ap_storekstar_init(struct ap_storekstar_data *block){
   block->out = fopen(block->filename, "w");
   if(block->out == NULL){
      printf("ERROR creating output file\n");
      exit(FAIL);
   }
}

void ap_storekstar_destroy(struct ap_storekstar_data *block){
   fflush(block->out);
   fclose(block->out);
}

void ap_storekstar_push(struct ap_storekstar_data *block, int port, void *ptr, int count){
#if(DEBUG == 1) /* debug look at text */   
   const int num_k_star = (count / KSTRUCTSIZE);
   if(num_k_star > 0)
   {
      int i;
      K* k = (K*) ptr;
      for(i = 0; i < num_k_star;i++)
      {
         char **k_string = (char**) alloca(sizeof(char*));
         printkstar(k,k_string);
         fprintf(block->out,"%s\n",*k_string);
         k++;
      }
      ap_release(block,0,(num_k_star * KSTRUCTSIZE));
   }
#else /* look at binary */   
   fwrite(ptr,sizeof(uint8_t),count,block->out);
   ap_release(block,0,count);
#endif
}

int ap_storekstar_go(struct ap_storekstar_data *block){
   return (GOSTOP);
}


void ap_storekstar_push_signal(struct ap_storekstar_data *block, int port, int type, int value)
{
}

void printkstar(K *k, char **buffer)
{
   int index;
   char **buffer_one = (char**)malloc(sizeof(char*));
   index = asprintf(buffer_one,"Read Index: %"PRIu32"\nK-Offset: %"PRIu8"\nK-Length: %"PRIu8"",k->read_index.read_index, k->k_offset, k->k_length);
   char *buffer_two = (char*) malloc(sizeof(char) * (K_LENGTH + 1));
   const nuc_k_t mask = 3;
   int i;
   for(i = (K_LENGTH); i >= 0; i--)
   {
      switch(((k->k_hash) & mask))
      {
         case A2:
            buffer_two[i] = 'A';
            break;
         case C2:
            buffer_two[i] = 'C';
            break;
         case G2:
            buffer_two[i] = 'G';
            break;
         case T2:
            buffer_two[i] = 'T';
            break;
         default:
            /*do nothing */
            fprintf(stderr,"ERR: %"PRIx64"\n",((k->k_hash) & mask));
            break;
      }
      k->k_hash = k->k_hash >> BITS_PER_BASE_K;
   }
   buffer_two[K_LENGTH] = '\0';
   
   *buffer = (char*) malloc(sizeof(char) * (index + K_LENGTH + 2));
   snprintf(*buffer,(index + K_LENGTH+2),"%s\n%s",*(buffer_one),buffer_two);
   free(*buffer_one);
   free(buffer_two);
   free(buffer_one);
}
