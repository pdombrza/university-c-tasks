#include "string.h"

#ifndef __bufor_h
#define __bufor_h

typedef struct MSG_T message_t;
#define CAPACITY 5
#define REAL_CAPACITY (CAPACITY + 1)

struct MSG_T
{
  int id;
  char text[10];
};

struct buffer
{
  int write_idx;
  int read_idx;
  message_t contents[REAL_CAPACITY];
};

void write_buf(struct buffer* buffer, message_t msg);
message_t read_buf(struct buffer* buffer);


#endif