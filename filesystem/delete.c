#include <stdio.h>
#include <stdlib.h>

#include "filesystem.h"

int main(void) {
    int ret_val;
    ret_val = remove("virtual_disk");
    if (ret_val != 0) {
        perror("Remove failed");
        return -1;
    }
    return 0;
}