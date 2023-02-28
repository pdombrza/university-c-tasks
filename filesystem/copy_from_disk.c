#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "filesystem.h"


int count_non_zero(int arr[], int size) {
    int i;
    int count = 0;
    for (i = 0; i < size; i++) {
        if (arr[i] != 0) count++;
    }
    return count;
}


int main(int argc, char **argv) {
    FILE *disk, *file;
    superblock s_block;
    inode i_node;
    datablock d_block;
    char fname[NAME_LENGTH];
    int required_blocks;
    int *nonzero_blocks;
    int *arr_ptr;
    int count;
    int i, j;
    int offset;

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
        fread(&i_node, sizeof(inode), 1, disk);
        if (!strcmp(i_node.name, fname)) {
            required_blocks = i_node.size / BLOCK_SIZE;
            /* choose nonzero blocks from inode */
            count = count_non_zero(i_node.blocks, MAX_BLOCKS);
            nonzero_blocks = calloc(count, sizeof(int));
            arr_ptr = nonzero_blocks;
            for (j = 0; j < MAX_BLOCKS; j++) {
                if(i_node.blocks[j] != 0) {
                    *arr_ptr = i_node.blocks[j];
                    arr_ptr++;
                }
            }
            file = fopen(fname, "a+b");
            /* write blocks */
            for (j = 0; j < count-1; j++) {
                offset = BLOCK_SIZE + INODES * BLOCK_SIZE + i_node.blocks[j-1] * BLOCK_SIZE;
                fseek(disk, offset, SEEK_SET);
                fread(&d_block, sizeof(datablock), 1, disk);
                fwrite(&d_block, sizeof(datablock), 1, disk);
            }
            /* last block */
            offset = BLOCK_SIZE + INODES * BLOCK_SIZE + (nonzero_blocks[count-1]-1) * BLOCK_SIZE;

            fseek(disk, offset, SEEK_SET);
            fread(&d_block, BLOCK_SIZE, 1, disk);
            fwrite(&d_block.data, i_node.size - (required_blocks * DATA_SIZE), 1, file);

            /* free memory */
            free(nonzero_blocks);
            fclose(file);
            fclose(disk);
            return 0;
        }
        fseek(disk, BLOCK_SIZE - sizeof(inode), SEEK_CUR);
    }
    fclose(disk);

    perror("file not found");
    return -1;
}