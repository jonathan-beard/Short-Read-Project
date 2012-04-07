#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <inttypes.h>

#include "nucformat.h"
#include "kstarhash.h"


/* declarations */

#define copy_info_to_k(r,k,offset)  k->read_index = r->read_index; \
                                    k->k_offset = offset; \
                                    k->k_length = K_LENGTH 

#define BITS_IN_NUC_READ_T sizeof(nuc_read_t) * 8
#define BASES_PER_NUC_READ_T_POW2 ((BITS_IN_NUC_READ_T-1) - __builtin_clzl(BASES_PER_NUC_READ_T))
#define BITS_PER_BASE_READ_POW2 ((BITS_IN_NUC_READ_T-1) - __builtin_clzl(BITS_PER_BASE_READ))
#define BITS_FOR_READ_T_K_STAR (K_LENGTH << BITS_PER_BASE_READ_POW2)
#define READ_TO_K_BITMASK ((UINT64_MAX >> (BITS_IN_NUC_READ_T - BITS_PER_BASE_READ)) << (BITS_IN_NUC_READ_T - BITS_PER_BASE_READ))
#define GET_READ_HEAD(IN)     ((READ_TO_K_BITMASK & IN) >> (BITS_IN_NUC_READ_T - BITS_PER_BASE_READ))

inline nuc_k_t make_hash(Read *r,const uint8_t offset);

void ap_kstarhash_init(struct ap_kstarhash_data *block)
{
   block->num_hashes = -1;
}

void ap_kstarhash_destroy(struct ap_kstarhash_data *block)
{
};

void ap_kstarhash_push(struct ap_kstarhash_data *block, int port, void *ptr, int count){
   const int num_input_structs = (count / READSTRUCTSIZE);
   if(num_input_structs > 0){
      Read *read = (Read*)ptr;
      const int num_hashes = read->read_length - K_LENGTH;
      const int num_hash_bytes = (num_hashes * KSTRUCTSIZE);
      K *k = ap_allocate(block,0,num_hash_bytes);
      int i;
      for(i = 0; i <= num_hashes; k++,i++)
      {
         copy_info_to_k(read,k,i);
         k->k_hash = make_hash(read,i);
      }
      ap_send(block,0,num_hash_bytes);
      ap_release(block,0,READSTRUCTSIZE);
   }
};



int ap_kstarhash_go(struct ap_kstarhash_data *block){ return (APSTOP);  };

void ap_kstarhash_push_signal(struct ap_kstarhash_data *block, int port, int type, int value){};
 

/* make_hash - actually does the hashing.  Takes in a Read struct and offset
 * returns a nuc_k_t hash bitvector. 
 */

inline nuc_k_t make_hash(Read *r,const uint8_t offset)
{
   nuc_k_t out = 0x0;
   /* get offset with reference to nuc_read_t array in type Read */
   const uint8_t array_offset = (offset >> BASES_PER_NUC_READ_T_POW2);
   /* get index within the above nuc_read_t offset to start at */
   const uint8_t index_to_start = ((offset - (array_offset << BASES_PER_NUC_READ_T_POW2))) << BITS_PER_BASE_READ_POW2;
   /* get nuc_read_t vector with the start of our index in the high bit */
   nuc_read_t k_length_vector = (r->read[array_offset] << index_to_start);
   uint8_t i;
   /* should be unrollable at compile time */
   for(i = 1; i < K_LENGTH; i++)
   {
        out = PACKK(out,FOURTOTWO(GET_READ_HEAD(k_length_vector)));
        k_length_vector = (k_length_vector << BITS_PER_BASE_READ);
   }
   out = (out | FOURTOTWO(GET_READ_HEAD(k_length_vector)));
   return (out);
}
