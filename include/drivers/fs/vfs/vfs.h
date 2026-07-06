#ifndef VFS_H
#define VFS_H

#include <std/stdint.h>
#include <multiboot.h>
#include <memory.h>
#include <vsprintf.h>

#define VFS_FILE      0
#define VFS_DIR       1
#define VFS_BLOCKDEV  2
#define VFS_CHARDEV   3

#define VFS_MAX_NAME  64

typedef struct vfs_node {
    char name[VFS_MAX_NAME];
    uint32_t type;
    uint32_t size;
    uint32_t inode;
    void *priv;
    struct vfs_node *parent;
    struct vfs_node *children;
    struct vfs_node *next;
    
    uint32_t (*read)(struct vfs_node *node, uint32_t offset, uint32_t size, uint8_t *buf);
    uint32_t (*write)(struct vfs_node *node, uint32_t offset, uint32_t size, uint8_t *buf);
    struct vfs_node* (*finddir)(struct vfs_node *node, char *name);
    uint32_t (*ioctl)(struct vfs_node *node, uint32_t cmd, void *arg);
} vfs_node_t;

void init_vfs();
void vfs_init();
vfs_node_t* vfs_create_node(char *name, uint32_t type, vfs_node_t *parent);
void vfs_add_node(vfs_node_t *parent, vfs_node_t *node);
vfs_node_t* vfs_open(char *path);
uint32_t vfs_read(vfs_node_t *node, uint32_t offset, uint32_t size, uint8_t *buf);
uint32_t vfs_write(vfs_node_t *node, uint32_t offset, uint32_t size, uint8_t *buf);
vfs_node_t* vfs_finddir(vfs_node_t *dir, char *name);
void vfs_list(vfs_node_t *dir);

extern vfs_node_t *root_fs;

#endif