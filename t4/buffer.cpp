#include <stdio.h>
#include <time.h>
#include <iostream>
#include <string>
#include <thread>
#include <chrono>
#include <vector>
#include "monitor.h"
#include "message.h"
#include "buffer.h"

Buffer buffer;

void write_buf(Buffer *buffer, message_t msg)
{
    if (((buffer->write_idx + 1) % REAL_CAPACITY) != buffer -> read_idx) {
        buffer->contents[buffer->write_idx] = msg;
        buffer-> write_idx = (buffer->write_idx + 1) % REAL_CAPACITY;
    }
}

message_t read_buf(Buffer *buffer)
{
    if (buffer->read_idx != buffer->write_idx) {
        message_t tmp = buffer->contents[buffer->read_idx];
        buffer->read_idx = ((buffer->read_idx + 1) % REAL_CAPACITY);
        return tmp;
    }
    message_t error_obj;
    return error_obj;
};

void writer_write(int writer_id, int messages_to_write)
{
    for (size_t i = 0; i < messages_to_write; i++)
    {
        printf("Writer %d, start writing...\n", writer_id); fflush(stdout);
        message_t msg;
        msg.id = writer_id + 10;
        msg.text = "text";
        std::this_thread::sleep_for(std::chrono::milliseconds(get_sleep_time()));
        buffer.putBuf(msg, writer_id);
    }
}

void reader_read(int reader_id, int messages_to_read)
{
    for (size_t i = 0; i < messages_to_read; i++)
    {
        printf("Reader %d, start reading...\n", reader_id); fflush(stdout);
        std::this_thread::sleep_for(std::chrono::milliseconds(get_sleep_time()));
        message_t msg = buffer.getBuf(reader_id);
    }
}


unsigned int get_sleep_time()
{
    unsigned int rand_time = 1000 + rand() % 1000;
    return rand_time;
}


int main(int argc, char **argv)
{
    if(argc != 5)
        {
        printf("Invalid argument amount.\nUsage: ./monitor <writer_amount> <how_many_write> <reader_amount> <how_many_read>\n");
            return 1;
        }
    srand((unsigned int)time(NULL));
    int writer_amount = atoi(argv[1]);
    int messages_to_write = atoi(argv[2]);
    int reader_amount = atoi(argv[3]);
    int messages_to_read = atoi(argv[4]);
    std::vector<std::thread> writers {};
    std::vector<std::thread> readers {};

    // create reader and writer threads
    for (size_t i = 0; i < writer_amount; ++i)
    {
        buffer.buf_id = 1;
        writers.push_back(std::thread(writer_write, i, messages_to_write));
    }

    for (size_t i = 0; i < reader_amount; ++i)
    {
        readers.push_back(std::thread(reader_read, i, messages_to_read));
    }

    // join threads
    for (size_t i = 0; i < writers.size(); ++i)
    {
        writers.at(i).join();
    }
    for (size_t i = 0; i < readers.size(); ++i)
    {
        readers.at(i).join();
    }
    std::cout << "Parent ended" << std::endl;
    return 0;
}
