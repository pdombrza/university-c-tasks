#include <string.h>
#include <semaphore.h>

#ifndef __semafor_h
#define __semafor_h

struct buffer *create_buf();
struct buffer *alloc_buf();
struct sem_struct *create_sem_struct();
struct sem_struct *alloc_sem_struct();
void create_writer();
void create_reader();
void reader_read();
float random_sleep_time();
void writer_write();


struct sem_struct
{
  sem_t buffer_status;
  sem_t buffer_free;
  sem_t buffer_full;
};

#endif
