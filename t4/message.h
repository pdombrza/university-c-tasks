#ifndef __message_h
#define __message_h

#include <string>

struct message_t
{
    int id {};
    std::string text {};
    message_t() : id(-1) {};
};

#endif