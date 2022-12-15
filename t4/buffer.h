#ifndef __bufor_h
#define __bufor_h

#include <string>
#include <iostream>
#include "monitor.h"
#include "message.h"

#define CAPACITY 5
#define REAL_CAPACITY (CAPACITY + 1)


void write_buf(struct Buffer* buffer, message_t msg);
message_t read_buf(struct Buffer* buffer);
void reader_read(int reader_id, int messages_to_read);
void writer_write(int writer_id, int messages_to_write);
unsigned int get_sleep_time();

struct Buffer : Monitor
{
    int buf_id = 0;
    int write_idx = 0;
    int read_idx = 0;
    int buff_size = REAL_CAPACITY;
    Condition full, empty;
    message_t contents[REAL_CAPACITY];
    Buffer() = default;
    ~Buffer() = default;
    // int count = 0;
    int element_count() {return (buff_size + write_idx - read_idx) % buff_size;};
    void putBuf(message_t msg, int writer_id)
    {
        enter();
        if (element_count() == CAPACITY)
            wait(full);
        write_buf(this, msg);
        std::cout << "Writer: " << writer_id << " wrote message: " << msg.text << " id: " << msg.id << std::endl;
        // count++;
        if (element_count() == 1)
            signal(empty);
        leave();
    }
    message_t getBuf(int reader_id)
    {
        enter();
        if (element_count() == 0)
            wait(empty);
        auto msg = read_buf(this);
        std::cout << "Reader: " << reader_id << " read message: " << msg.text << " id: " << msg.id << std::endl;
        // count--;
        if (element_count() == CAPACITY-1)
            signal(full);
        leave();
        return msg;
    }
};

#endif
