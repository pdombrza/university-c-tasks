#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "filesystem.h"

int main(int argc, char **argv) {
    FILE *disk;
    superblock s_block;
    inode i_node;
    datablock d_block;
    int i_node_pos;
    char fname[NAME_LENGTH];
    char nullbyte = '\0';
    int i_node_blocks[MAX_BLOCKS];
    int required_blocks;
    int offset;
    int i, j;

    if (argc < 2) {
        perror("no file given");
        return -1;
    }

    strcpy(fname, argv[1]);

    disk = fopen("virtual_disk", "r+b");
    if (disk == NULL) {
        perror("failed to open disk");
        return -1;
    }



    fread(&s_block, sizeof(superblock), 1, disk);
    fseek(disk, s_block.i_node_offset, SEEK_SET);

    for (i = 0; i < INODES; i++) {
        i_node_pos = ftell(disk);
        fread(&i_node, sizeof(inode), 1, disk);
        if (!strcmp(i_node.name, fname)) {
            fseek(disk, i_node_pos, SEEK_SET);
            memcpy(i_node_blocks, i_node.blocks, sizeof(i_node.blocks));
            required_blocks = i_node.size / BLOCK_SIZE;
            memset(&i_node, 0, sizeof(i_node)); /* free inode */
            fwrite(&i_node, sizeof(inode), 1, disk);
            memset(&d_block, 0, sizeof(d_block)); /* free d_block */
            for (j = 0; j < MAX_BLOCKS; j++) {
                if (i_node_blocks[j] != 0) {
                    offset = BLOCK_SIZE + INODES * BLOCK_SIZE + i_node_blocks[j-1] * BLOCK_SIZE;
                    fseek(disk, offset, SEEK_SET);
                    fwrite(&d_block, sizeof(d_block), 1, disk);
                    fwrite(&nullbyte, sizeof(char), BLOCK_SIZE - sizeof(d_block), disk);
                }
            }
            fclose(disk);
            return 0;
        }
        fseek(disk, BLOCK_SIZE - sizeof(inode), SEEK_CUR);
    }
    fclose(disk);
    perror("file not found");
    return -1;
}