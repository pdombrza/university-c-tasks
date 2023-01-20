#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>

#include "filesystem.h"


int main(int argc, char **argv) {
    FILE *disk, *source;
    superblock s_block;
    inode i_node;
    datablock block;
    char fname[NAME_LENGTH];
    char f_content[DATA_SIZE];
    int file_size = 0;
    short free_inodes = 1;
    short free_block_count = 0;
    int last_block_size = 0;
    long d_block_position;
    long curr_block_position;
    int next_data_block = 0;
    long i_node_position;
    int blocks_required;
    char nullbyte = '\0';
    int i, j;

    if (argc < 2) {
        perror("Invalid argument. Usage: ./copy_to [filename]");
        return -1;
    }
    disk = fopen("virtual_disk", "r+b");
    if (disk == NULL) {
        perror("failed to open disk");
        return -1;
    }

    fread(&s_block, sizeof(superblock), 1, disk);
    strcpy(fname, argv[1]);

    /* check if file already exists */
    fseek(disk, s_block.i_node_offset, SEEK_SET);
    for (i = 0; i < INODES; i++) {
        fread(&i_node, sizeof(inode), 1, disk);
        if (!strcmp(i_node.name, fname)) {
            perror("file already exists");
            return -1;
        }
        fseek(disk, BLOCK_SIZE - sizeof(inode), SEEK_CUR);
    }
    fclose(disk);

    /* get file metadata */
    source = fopen(fname, "r+b");
    if (source == NULL) {
        perror("Failed to open file");
        return -1;
    }
    fseek(source, 0, SEEK_END);
    file_size = ftell(source);
    fseek(source, 0, SEEK_SET);
    blocks_required = file_size / DATA_SIZE;
    fclose(source);

    if (blocks_required > DATA_BLOCKS) {
        perror("not enough space in virtual disk");
        return -1;
    }
    /* look for free inode */
    disk = fopen("virtual_disk", "r+b");
    if (disk == NULL) {
        perror("Failed to open file");
        return -1;
    }
    fseek(disk, s_block.i_node_offset, SEEK_SET);
    for (i = 0; i < INODES; i++) {
        i_node_position = ftell(disk);
        fread(&i_node, sizeof(inode), 1, disk);
        fseek(disk, BLOCK_SIZE - sizeof(inode), SEEK_CUR);
        if (i_node.is_full == 0) {
            free_inodes = 0;
            break;
        }
    }
    /* if no inodes - quit */
    if (free_inodes == 1) {
        perror("no inodes available");
        return -1;
    }
    /* write to inode */
    i_node.is_full = 1;
    strcpy(i_node.name, fname);
    i_node.size = file_size;

    /* check if enough datablocks are available */
    fseek(disk, s_block.data_block_offset, SEEK_SET);
    for (i = 0; i < DATA_BLOCKS; i++) {
        fread(&block, sizeof(datablock), 1, disk);
        if (block.is_full == 0) {
            free_block_count++;
        }
    }
    if (free_block_count < blocks_required || free_block_count == 0) {
        perror("not enough blocks available");
        return -1;
    }

    /* go to first data block */
    fseek(disk, s_block.data_block_offset, SEEK_SET);

    source = fopen(fname, "r+b");
    if (source == NULL)
    {
        perror("Failed to open file");
        return -1;
    }
    /* look for first empty datablock */
    for (i = 0; i < DATA_BLOCKS; i++) {
        d_block_position = ftell(disk);
        fread(&block, sizeof(datablock), 1, disk); /* sizeof(datablock) = 4096 */
        if (block.is_full == 0) {
            break;
        }
    }
    curr_block_position = d_block_position;

    fseek(disk, d_block_position, SEEK_SET); /* cursor is at first empty block */
    fseek(disk, BLOCK_SIZE, SEEK_CUR); /* cursor at next block */
    for (i = 0; i <= blocks_required; i++) {
        if (i != blocks_required) {
            while (1) {
                next_data_block = ftell(disk);
                fread(&block, sizeof(datablock), 1, disk); /* at the end of loop cursor is set to next block (after last empty found)*/
                if (block.is_full == 0) {
                    break;
                }
            }
            block.is_full = 1;
            block.next_datablock = next_data_block;
            i_node.blocks[i] = (curr_block_position - s_block.data_block_offset) / BLOCK_SIZE + 1;
            fread(&f_content, DATA_SIZE, 1, source);
            strcpy(block.data, f_content);
            fseek(disk, curr_block_position, SEEK_SET);
            fwrite(&block, sizeof(datablock), 1, disk);
        } 
        else { /* last block */
            block.is_full = 1;
            i_node.blocks[i] = (curr_block_position - s_block.data_block_offset) / BLOCK_SIZE + 1;
            last_block_size = file_size - (DATA_SIZE * blocks_required) + 1; /* size of data in last block */
            fread(&f_content, last_block_size, 1, source);
            strcpy(block.data, f_content);
            fseek(disk, curr_block_position, SEEK_SET);
            fwrite(&block, last_block_size+1, 1, disk);
            fwrite(&nullbyte, 1,  DATA_SIZE - last_block_size, disk);
        }
        if (i == 0) {
            i_node.data_offset = d_block_position;
        }
        curr_block_position = next_data_block;
        fseek(disk, curr_block_position + BLOCK_SIZE, SEEK_SET);
    }
    fseek(disk, i_node_position, SEEK_SET);
    fwrite(&i_node, sizeof(inode), 1, disk);
    putchar('\n');

    fclose(source);
    fclose(disk);
    return 0;
}