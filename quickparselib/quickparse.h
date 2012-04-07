#ifndef __QUICKPARSE_H__
#define __QUICKPARSE_H__

#include <stdint.h>
#include "nucformat.h"

typedef struct SequenceData{
   Read     *read;
   Read     *read_ptr; /* use this one to read off of */
   uint64_t number_of_reads;
}SequenceData;

/*
 * quickparse_init - use this to initialize the parser
 * filename - name of file to be parsed
 * data     - initialized pointer, location of sequence data will be here
 */
int32_t quickparse_init(char *filename, struct SequenceData *data);

/*
 * quickparse_destroy - call when completely done to release
 * data structures 
 */
int32_t quickparse_destroy(struct SequenceData *data);

/* quickparse_go - call go start the parsing process */
int32_t quickparse_go(struct SequenceData *data);

#endif /* END QUICKPARSE.H */
