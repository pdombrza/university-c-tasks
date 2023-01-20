#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "filesystem.h"


int main(int argc, char **argv) {
    FILE *disk;
    superblock s_block;
    inode i_node;
    datablock d_block;
    int i;
    char nullbyte = '\0';
    disk = fopen("virtual_disk", "r");
    if (disk != NULL) {
        perror("File system already exists");
        return -1;
    }

    s_block.block_size = BLOCK_SIZE;
    s_block.data_block_amount = DATA_BLOCKS;
    s_block.i_nodes = INODES;
    s_block.i_node_offset = BLOCK_SIZE;
    s_block.data_block_offset = BLOCK_SIZE + INODES * BLOCK_SIZE; /* super block + inodes*/


    i_node.data_offset = 0;
    i_node.size = 0;
    memset(&(i_node.blocks), 0, (sizeof(i_node.blocks)));
    i_node.is_full = 0;
    memset(&(i_node.name), 0, sizeof(i_node.name));

    memset(&d_block, 0, sizeof(datablock));

    disk = fopen("virtual_disk", "a+b");
    if (disk == NULL) {
        perror("Failed to open disk");
        return -1;
    }
    fwrite(&s_block, sizeof(superblock), 1, disk);
    fwrite(&nullbyte, sizeof(char), BLOCK_SIZE - sizeof(superblock), disk);
    for (i = 0; i < INODES; i++) {
        fwrite(&i_node, sizeof(inode), 1, disk);
        fwrite(&nullbyte, sizeof(char), BLOCK_SIZE - sizeof(inode), disk);
    }
    for (i = 0; i < DATA_BLOCKS; i++) {
        fwrite(&i_node.is_full, sizeof(short), 1, disk);
        fwrite(&nullbyte, sizeof(char), BLOCK_SIZE - sizeof(short), disk);
    }
    fclose(disk);
    return 0;
}