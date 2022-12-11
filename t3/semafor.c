#include <unistd.h>
#include <stdio.h>
#include <sys/wait.h>
#include <stdlib.h>
#include <string.h>
#include <sys/shm.h>
#include <semaphore.h>
#include <time.h>

#include "buffer.h"
#include "semafor.h"

int buf_mem_id = 0;
int sem_mem_id = 0;

struct buffer *create_buf();
struct sem_struct *create_sem_struct();

int main(int argc, char **argv)
{
	if(argc != 5)
	{
    printf("Invalid argument amount.\nUsage: ./semafor <writer_amount> <how_many_write> <reader_amount> <how_many_read>\n");
		return 1;
	}
	int writer_amount = atoi(argv[1]);
	int messages_to_write = atoi(argv[2]);
	int reader_amount = atoi(argv[3]);
	int messages_to_read = atoi(argv[4]);
	static int msg_id = 0;
	create_buf();
	create_sem_struct();

	for(int i = 0; i < reader_amount; i++) {
		create_reader(messages_to_read);
	}
	for(int i = 0; i < writer_amount; i++) {
		create_writer(messages_to_write, msg_id);
	}
	wait(NULL);
	return 0;
}

struct buffer *create_buf()
{
	struct buffer *buf = alloc_buf();
	memset(buf, 0, sizeof(struct buffer));
	return buf;
}


struct buffer *alloc_buf()
{
	if(buf_mem_id == 0)
		buf_mem_id = shmget(IPC_PRIVATE, sizeof(struct buffer) + REAL_CAPACITY * sizeof(message_t), SHM_W | SHM_R);
	if(buf_mem_id <= 0)
	{
		perror("Failed to allocate buffer memory\n");
    	exit(EXIT_FAILURE);
	}
	void *data = shmat(buf_mem_id, NULL, 0);
	struct buffer *buf = (struct buffer*) data;
	return buf;
}

struct sem_struct *create_sem_struct()
{
	struct sem_struct *semaphores = alloc_sem_struct();
  sem_init(&semaphores->buffer_status, 1, 1);
  sem_init(&semaphores->buffer_full, 1, 0);
  sem_init(&semaphores->buffer_free, 1, CAPACITY);
  return semaphores;
}


struct sem_struct *alloc_sem_struct()
{
	if(sem_mem_id == 0)
		sem_mem_id = shmget(IPC_PRIVATE, sizeof(struct sem_struct), SHM_W | SHM_R);

	if(sem_mem_id <= 0)
	{
		perror("Failed to allocate semaphore memory\n");
		exit(EXIT_FAILURE);
	}
	return (struct sem_struct*) shmat(sem_mem_id, NULL, 0);
}


void create_writer(int messages_to_write, int msg_id)
{
	int result = fork();
	if(result == 0)
	{
		writer_write(messages_to_write, msg_id);
		exit(EXIT_SUCCESS);
	}
}


void create_reader(int messages_to_read)
{
	int result = fork();
	if(result == 0)
	{
		reader_read(messages_to_read);
		exit(EXIT_SUCCESS);
	}
}


void reader_read(int messages_to_read)
{
	unsigned int reader_id = getpid();
	printf("Reader %d, start reading...\n", reader_id);

	struct buffer* buf = alloc_buf();
	struct sem_struct *semaphores = alloc_sem_struct();

	unsigned int messages_read = 0;
	while(messages_read < messages_to_read)
	{
		usleep(random_sleep_time()*900000);

    	sem_wait(&semaphores->buffer_full);
		sem_wait(&semaphores->buffer_status);
		message_t msg = read_buf(buf);
		sem_post(&semaphores->buffer_status);
		sem_post(&semaphores->buffer_free);
    if (msg.id != -1) {
		  printf("Read message. Reader id: %d. Message id: %d, contents: %s.\n", reader_id, msg.id, msg.text);
		  messages_read++;
     	}
	}
	printf("Reader %d finished.\n", reader_id);
}


float random_sleep_time()
{
	srand((unsigned int)time(NULL));
	return ((float)(rand() % 100)) / 100.0f + 1.0f;
}


void writer_write(int messages_to_write, int msg_id)
{
	unsigned int writer_id = getpid();
	printf("Writer %d, start writing...\n", writer_id);
	struct buffer *buf = alloc_buf();
	struct sem_struct *semaphores = alloc_sem_struct();

	unsigned int sent_messages = 0;
	message_t msg;
  	const char* message = "message";
  	strcpy((void*)msg.text, (void*) message);
  	message_t message_ptr = msg;

	while(sent_messages < messages_to_write)
	{
		msg_id++;
		message_ptr.id = writer_id + msg_id;
		usleep(random_sleep_time()*900000);
		sem_wait(&semaphores->buffer_free);
		sem_wait(&semaphores->buffer_status);
		write_buf(buf, message_ptr);
		printf("Writer: %d wrote message: id: %d, %s\n", writer_id, message_ptr.id, message_ptr.text);
		sem_post(&semaphores->buffer_status);

		sem_post(&semaphores->buffer_full);

		sent_messages++;
	}

	printf("Writer %d finished.\n", writer_id);
}
