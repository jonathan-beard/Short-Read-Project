/* nucformat.h - to be included in C kernels where the definition 
 * of the read struct and  k* structs are required.
 */

#ifndef __NUCFORMAT_H__
#define __NUCFORMAT_H__ 1

#include <stdint.h> 

#if (DEBUG == 1)
#include <inttypes.h>
/* NOTE: FOR DEBUGGING YOU CAN USE THIS AS A PRINT TYPE FOR printf
 * e.g. fprintf(stderr,"%"PRINUCREADT"\n",foo) where foo's type is
 * nuc_read_t
 */
#define PRINUCREADT  PRIx64
#define PRINUCKT     PRIx64
#endif

/* NOTE: To change cutoff of bases for Read structure
 * change only the NUMBER_OF_BASES_CUTOFF definition,
 * likewise to change the bits per base only modify the 
 * definition of the BITS_PER_BASE_XX definition.
 */

#define NUMBER_OF_BASES_CUTOFF   48
#define BITS_PER_BASE_READ       4
#define BITS_PER_BASE_K          2

#define BASES_PER_NUC_READ_T     (64 / BITS_PER_BASE_READ)
#define BASES_PER_NUC_K_T        (64 / BITS_PER_BASE_K)
#define READ_ARRAY_LENGTH        (NUMBER_OF_BASES_CUTOFF / BASES_PER_NUC_READ_T)

/* NOTE: K_LENGTH should be specified as a compile time
 * definition.  If not, 7 will be used for you and you 
 * will be warned.  When compiling you can use:
 * -DK_LENGTH=XX where XX = desired length of k*
 */

#ifndef K_LENGTH
#define K_LENGTH                 7
#warning K_LENGTH NOT DEFINED, USING 7!!
#endif

#ifndef USEPHRED
#define USEPHRED 0
#warning COMPILING WITHOUT PHRED SCORES!!
#endif


#define PACKREAD(VECTOR,BASE) (VECTOR | BASE) << BITS_PER_BASE_READ
#define PACKK(VECTOR,BASE)    (VECTOR | BASE) << BITS_PER_BASE_K


/* Nucleotide stuff:
 * Prefix of 4 = 4 bits
 * Prefix of 2 = 2 bits
 */

/* convert 4 bit base to two bit base */
#define FOURTOTWO(FOUR) (FOUR >> 2)
/* convert char into four bit base */
#define CHARTOFOUR(CHAR) (CHAR & 0x0f)

/* list of four bit bases */
#define A4     1   
#define C4     3
#define G4     7
#define T4     4
#define X4     8

/* list of two bit bases */
#define A2     0 
#define C2     1
#define G2     3
#define T2     2

/* nucleotide vector types */
typedef  uint64_t    nuc_read_t;
typedef  uint64_t    nuc_k_t;
#if (USEPHRED == 1)
typedef  uint8_t     score_t;
#endif

typedef struct READ_INDEX{
   /* set_delimiter - set high when this and everything 
    * till the next high set_delimiter is in this set
    * this should be set by the sort algorithm.  
    */

    /* read_index - self explanatory */
   uint32_t
      set_delimiter     :     1,
      read_index        :     31;
}READ_INDEX;

/* Read - used by parser to represent a read */
typedef struct Read{
   READ_INDEX     read_index;
   uint8_t        read_length;
   nuc_read_t     read[READ_ARRAY_LENGTH];
#if (USEPHRED == 1)
   score_t        read_score[NUMBER_OF_BASES_CUTOFF];
#endif 
}Read;


/* K - used by hash kernel to represent a hashed k_star sequence */
typedef struct K{
   READ_INDEX     read_index;
   uint8_t        k_offset;
   uint8_t        k_length;
   nuc_k_t        k_hash;
}K;

/* K_SMALL - output of sort kernel */
typedef struct K_Small{
   READ_INDEX     read_index;
   uint8_t        k_offset;
}K_SMALL;

/* ReadMerged - output of merge block */
typedef struct ReadMerged{
   Read           read;
   uint8_t        k_offset;
}ReadMerged;

/* ReadMatch - output of match block */
typedef struct ReadMatch{
   READ_INDEX     a;
   READ_INDEX     b;
}ReadMatch;

#define READSTRUCTSIZE sizeof(Read)
#define KSTRUCTSIZE    sizeof(K)

#endif /* end nucformat.h */
