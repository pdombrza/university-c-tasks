#ifndef __FILE_SYSTEM_H__
#define __FILE_SYSTEM_H__

#define FILESYSTEM_SIZE 262144
#define INODES 15
#define DATA_BLOCKS 48
#define BLOCK_SIZE 4096
#define NAME_LENGTH 16
#define MAX_BLOCKS 16
#define DATA_SIZE (BLOCK_SIZE - sizeof(int) - sizeof (short))

typedef struct superblock
{
    int block_size;
    int i_nodes;
    int i_node_offset;
    int data_block_amount;
    int data_block_offset;

} superblock;

typedef struct datablock {
    short is_full;
    char data[DATA_SIZE];
    int next_datablock;

} datablock;

typedef struct inode
{
    char name[NAME_LENGTH];
    int size;
    short is_full;
    int data_offset;
    int blocks[MAX_BLOCKS];

} inode;


#endif