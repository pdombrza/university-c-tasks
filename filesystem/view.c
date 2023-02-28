#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "filesystem.h"



int main(void) {
    FILE *disk;
    superblock s_block;
    inode i_node;
    datablock d_block;
    int datablocks[DATA_BLOCKS];
    int i, j;
    int block_count;
    long int current_pos;
    int offset;
    int block_num;

    memset(&datablocks, 0, sizeof(datablocks));

    disk = fopen("virtual_disk", "r+b");
    if (disk == NULL) {
        perror("Failed to open disk.");
        return -1;
    }
    
    fread(&s_block, sizeof(superblock), 1, disk);
    printf("block_size: %d, i_nodes: %d, i_node_offset: %d, data_block_amount: %d, data_block_offset: %d\n", s_block.block_size, s_block.i_nodes, s_block.i_node_offset, s_block.data_block_amount, s_block.data_block_offset);
    fseek(disk, s_block.i_node_offset, SEEK_SET);
    for (i = 0; i < INODES; i++) {
        fread(&i_node, sizeof(inode), 1, disk);
        if (i_node.is_full > 0) {
            printf("%d: %s, file size: %d\n", i + 1, i_node.name, i_node.size);
            
            for (j = 0; j < MAX_BLOCKS; j++) {
                offset = BLOCK_SIZE + BLOCK_SIZE * INODES + BLOCK_SIZE * i_node.blocks[j];
                block_num = i_node.blocks[j];
                if (block_num > 0) {
                    datablocks[block_num-1] = i + 1;
                }
            }
        }
        fseek(disk, BLOCK_SIZE - sizeof(inode), SEEK_CUR);
    }
    fclose(disk);
    /* print disk map */
    for (i = 0; i < DATA_BLOCKS; i++) {
        if (i % 16 == 0) {
            putchar('\n');
        }
        printf("%d ", datablocks[i]);
    }
    putchar('\n');
    return 0;
    }