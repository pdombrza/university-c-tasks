#include "buffer.h"


message_t read_buf(struct buffer* buffer) {
  if (buffer->read_idx != buffer->write_idx) {
    message_t tmp = buffer->contents[buffer->read_idx];
    buffer->read_idx = ((buffer->read_idx + 1) % REAL_CAPACITY);
    return tmp;
  }
  message_t error_obj;
  error_obj.id = -1;
  return error_obj;
}


void write_buf(struct buffer* buffer, message_t val) {
  if (((buffer->write_idx + 1) % REAL_CAPACITY) != buffer -> read_idx) {
    buffer->contents[buffer->write_idx] = val;
    buffer-> write_idx = (buffer->write_idx + 1) % REAL_CAPACITY;
  }
}
