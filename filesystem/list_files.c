#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "filesystem.h"


int main(void) {
    FILE *disk;
    superblock s_block;
    inode i_node;
    datablock d_block;
    int i;

    disk = fopen("virtual_disk", "r+b");

    if (disk == NULL) {
        perror("disk not found");
        return -1;
    }

    fread(&s_block, sizeof(superblock), 1, disk);
    fseek(disk, s_block.i_node_offset, SEEK_SET);

    for (i = 0; i < INODES; i++) {
        fread(&i_node, sizeof(inode), 1, disk);
        if (i_node.is_full > 0) {
            puts(i_node.name);
        }
        fseek(disk, BLOCK_SIZE - sizeof(inode), SEEK_CUR);
    }
    fclose(disk);
    return 0;
}