#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>
#include <sys/mman.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>

#if __APPLE__
#include <malloc/malloc.h>
#else
#include <malloc.h>
#endif

#include <errno.h>
#include <inttypes.h>

#include "quickparse.h"
#include "nucformat.h"

#ifndef SUCCESS
#define SUCCESS 0
#endif

#ifndef FAIL
#define FAIL -1
#endif

#ifndef ALIGN
#define ALIGN 16
#endif

#ifndef DEBUG
#define DEBUG 0
#endif

#ifndef ALIGNMENT
#define ALIGNMENT 16
#endif


/* declarations */
void panic(char *);
void (**functions)(char*,SequenceData *data);
void add_functions();
void init_read(char *,SequenceData *data);
void parsename(char *,SequenceData *data);
void parseseq(char *,SequenceData *data);
void parseseqname(char *,SequenceData *data);
void parsescore(char *,SequenceData *data);
void send(char *,SequenceData *data);
void error(char *,SequenceData *data);
void nothing(char *,SequenceData *data);
static uint64_t sequence_count = 1;
static uint8_t  shiftcount = BASES_PER_NUC_READ_T;

/* SEQDATA - struct for sequence data */
struct{
   char *ptr;                    /* data pointer */
   char *start_ptr;              /* start pointer, don't touch this */
   size_t byte_length;           /* length of memory mapped segment...including no exec section at end */
   uint64_t curr_read_position;  /* used to keep track of how long the read is */
   uint8_t state;
   uint32_t sequence_index;
   char *end_ptr;
}SEQDATA;

#define CHARS 12
#define STATES 7

#define ACTION(IN)      (IN  >> 4)
#define NEXTSTATE(IN)   (IN & 0x0f)
 

/* ACTIONS - these aren't really used for anything except for reference on the hex values below*/
#define ERROR        0
#define NOTHING      1
#define INIT         2
#define PARSENAME    3
#define PARSESEQ     4
#define PARSESEQNAME 5
#define PARSESCORE   6
#define SEND         7


/* note the macro above that gets expanded below... */
const uint8_t fsm_table[STATES][CHARS]=
   {
      /* 0 - @       */ {[9]=0x21},
      /* 1 - NAME    */ {[0 ... 8]=0x11 ,[11]=0x32},
      /* 2 - SEQ     */ {[1 ... 2]=0x42 ,[5]=0x42,[8]=0x42,[11]=0x13},
      /* 3 - \n      */ {[1 ... 2]=0x42 ,[5]=0x42,[8]=0x42,[10]=0x14}, 
      /* 4 - NAME2   */ {[0 ... 8]=0x54 ,[11]=0x15}, 
      /* 5 - SCORE   */ {[0 ... 10]=0x65,[11]=0x16},
      /* 6 - SEND/SO */ {[9] = 0x71,[11] = 0x16}
   };

/* this is a bit more space than I'd like but in the grand scheme of things...it's not much */
const uint8_t col_lut[128] = {
                              [10] = 11         /* \n    */,
                              [43] = 10         /* +     */,
                              [45] = 7          /* -     */, 
                              [46] = 5          /* .     */,
                              [48 ... 57] = 3   /* 0-9   */, 
                              [58] = 6          /* :     */, 
                              [64] = 9          /* @     */, 
                              [65 ... 90] = 1   /* A-Z   */, 
                              [95] = 4          /* _     */, 
                              [97 ... 122] = 2  /* a-z   */, 
                              [126] = 8         /* ~     */
                              }; /* everything else = 0 */


int32_t quickparse_go(SequenceData *data)
{
   if(data == NULL)
      panic("SequenceData struct cannot be null at quickparse_go!!\n");
   __builtin_prefetch(SEQDATA.ptr,1,3);
   __builtin_prefetch(col_lut,1,3);
   while(SEQDATA.ptr != SEQDATA.end_ptr)
   {
      const uint8_t column       = col_lut[*SEQDATA.ptr];
      const uint8_t val          = fsm_table[SEQDATA.state][column];
      const uint8_t action       = ACTION(val);
      const uint8_t next_state   = NEXTSTATE(val); 
      functions[action](SEQDATA.ptr,data);
      SEQDATA.state = next_state;
      SEQDATA.ptr++;
      __builtin_prefetch(SEQDATA.ptr,1,3);
      __builtin_prefetch(col_lut,1,3);
      __builtin_prefetch(fsm_table[next_state],1,3);
   }
   /* send last */
   send(" ",data);
   return (SUCCESS);
}


int32_t quickparse_init(char *filename,SequenceData *data)
{
   if(filename == NULL)
   {
      panic("Filename to init cannot be null!!\n");
   }
   /* open file for reading */
   int seq_file = open(filename,O_RDONLY);
   if(seq_file < 0)
   {
      panic("Couldn't open file descriptor!!\n");
   }   
   struct stat st;
   if(fstat(seq_file,&st) != SUCCESS)
   {
      panic("Couldn't stat file!!\n");
   }
   SEQDATA.byte_length = st.st_size; /* set file byte length */
   SEQDATA.ptr = SEQDATA.start_ptr = (char*) mmap(NULL, SEQDATA.byte_length, (PROT_READ), (MAP_PRIVATE) , seq_file, 0);
   if(SEQDATA.ptr == MAP_FAILED)
   {
      panic("Failed to memory map sequence file!!\n");
   }
   SEQDATA.end_ptr = (SEQDATA.start_ptr + SEQDATA.byte_length);

   data->read_ptr = data->read = (Read*) mmap(NULL,SEQDATA.byte_length  /*should be smaller than this*/,(PROT_READ | PROT_WRITE), (MAP_SHARED | MAP_ANON),-1,0);
   
   if(data->read == MAP_FAILED)
   {
      panic("Failed to map memory for sequence data!!\n");
   }
   SEQDATA.curr_read_position = 0;
   SEQDATA.state = 0;
   SEQDATA.sequence_index = 0; 
   add_functions();


   return (SUCCESS);
}

int32_t quickparse_destroy(SequenceData *data)
{
   if(munmap(SEQDATA.start_ptr,SEQDATA.byte_length) != SUCCESS)
   {
      return (FAIL);
   }
   if(munmap(data->read,SEQDATA.byte_length) != SUCCESS)
   {
      return (FAIL);
   }
   free(functions);
   return (SUCCESS);
}

void add_functions()
{
   functions = (void*) malloc(8 * sizeof(void*));
   if(functions == NULL)
      panic("Couldn't allocate memory for functions pointer!!\n");
   functions[ERROR]        = &error;
   functions[NOTHING]      = &nothing;
   functions[INIT]         = &init_read;
   functions[PARSENAME]    = &parsename;
   functions[PARSESEQ]     = &parseseq;
   functions[PARSESEQNAME] = &parseseqname;
   functions[PARSESCORE]   = &parsescore;
   functions[SEND]         = &send;
}

void panic(char *err)
{
   perror(err);
   exit(FAIL);
}

void error(char *c, SequenceData *data)
{
   panic(c);
}

/* nothing - does nothing */
void nothing(char *c, SequenceData *data){}

/* init_read - initialize Read structure ref, nuc_format.h */
void init_read(char *c, SequenceData *data)
{
   SEQDATA.curr_read_position = 0;
}

/* parsename - gets called when we hit the return after a sequence name */
void parsename(char *c, SequenceData *data)
{
   data->read->read_index.read_index = SEQDATA.sequence_index++; 
}

/* parseseq - called while we're in the sequence state */
void parseseq(char *c, SequenceData *data)
{
   if(SEQDATA.curr_read_position == NUMBER_OF_BASES_CUTOFF)
      return;
   /* convert from 8 bits to 4 bits....upper 4 of uint8_t will be zeros */
   const uint8_t temp_base = CHARTOFOUR(*c);
   /* or sequence vector with base */
   const uint8_t seq_index = (SEQDATA.curr_read_position / (BASES_PER_NUC_READ_T));
   if(((SEQDATA.curr_read_position + 1) % BASES_PER_NUC_READ_T) != 0)
   {
      data->read->read[seq_index] = PACKREAD((uint64_t)data->read->read[seq_index],temp_base);
   }
   else
   {
      data->read->read[seq_index] = ((uint64_t)data->read->read[seq_index] | temp_base);
   }
   SEQDATA.curr_read_position++;
}

void parseseqname(char *c, SequenceData *data)
{
   /* lets set length here...maybe add another state or something later */
   data->read->read_length = (SEQDATA.curr_read_position);
}

void parsescore(char *c, SequenceData *data)
{
#if (USEPHRED == 1)
   /* lets consume the whole block and ffwd the parser to the new line char*/
   char* temp_read_score = ((char*)r->read_score);
   /* at this point lets assume that our score >= 1 */
   uint16_t i;
   for(i = 0; i < SEQDATA.curr_read_position; i++)
   {
      *(temp_read_score++) = *(SEQDATA.ptr++); 
   }
   SEQDATA.ptr--;
   SEQDATA.curr_read_position = 0;
#endif
}



void send(char *c, SequenceData *data)
{
#if (DEBUG == 1)
   fprintf(stderr,"READID: %"PRIx32"\n",(uint32_t)data->read->read_index.read_index);
   fprintf(stderr,"LENGTH: %"PRIi8"\n",(uint8_t)data->read->read_length);
   uint8_t i;
   fprintf(stderr,"READ:   \n");
   for(i = 0; i < READ_ARRAY_LENGTH; i++)
      fprintf(stderr,"%"PRINUCREADT"\n",data->read->read[i]);
   fprintf(stderr,"\n");
#if (USEPHRED == 1)
   fprintf(stderr,"RSCORE: \n");
   char *score = (char*)data->read->read_score;

   for(i = 0; i < NUMBER_OF_BASES_CUTOFF; i++)
   {
      fprintf(stderr,"%c",score[i]);
   }
   fprintf(stderr,"\n");
#endif
#endif
   /* ALWAYS DO THIS */
   (data->read)++;
   data->number_of_reads++;
   SEQDATA.curr_read_position = 0;

}
