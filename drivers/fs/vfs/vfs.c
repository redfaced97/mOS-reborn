#include <drivers/fs/vfs/vfs.h>
#include <std/string.h>
#include <io.h>

extern uint32_t initrd_start;
extern uint32_t initrd_end;

vfs_node_t *root_fs = NULL;

static uint32_t memfs_read(vfs_node_t *node, uint32_t offset, uint32_t size, uint8_t *buf) {
    if (!node->priv || offset >= node->size) return 0;
    if (offset + size > node->size) size = node->size - offset;
    memcpy(buf, (uint8_t*)node->priv + offset, size);
    return size;
}

static uint32_t memfs_write(vfs_node_t *node, uint32_t offset, uint32_t size, uint8_t *buf) {
    if (!node->priv) {
        node->priv = kmalloc(offset + size);
        node->size = offset + size;
    }
    if (offset + size > node->size) {
        uint8_t *new = kmalloc(offset + size);
        memcpy(new, node->priv, node->size);
        kfree(node->priv);
        node->priv = new;
        node->size = offset + size;
    }
    memcpy((uint8_t*)node->priv + offset, buf, size);
    return size;
}

static vfs_node_t* vfs_default_finddir(vfs_node_t *dir, char *name) {
    if (!dir || dir->type != VFS_DIR) return NULL;
    vfs_node_t *child = dir->children;
    while (child) {
        if (!strcmp(child->name, name)) return child;
        child = child->next;
    }
    return NULL;
}

void vfs_init() {
    root_fs = vfs_create_node("/", VFS_DIR, NULL);
    root_fs->finddir = vfs_default_finddir;
    
    // Создаём /dev
    vfs_node_t *dev = vfs_create_node("dev", VFS_DIR, root_fs);
    dev->finddir = vfs_default_finddir;
    vfs_add_node(root_fs, dev);
    
    // Создаём /proc
    vfs_node_t *proc = vfs_create_node("proc", VFS_DIR, root_fs);
    proc->finddir = vfs_default_finddir;
    vfs_add_node(root_fs, proc);
    
    printk("VFS initialized\n");
}

vfs_node_t* vfs_create_node(char *name, uint32_t type, vfs_node_t *parent) {
    vfs_node_t *node = (vfs_node_t*)kmalloc(sizeof(vfs_node_t));
    memset(node, 0, sizeof(vfs_node_t));
    strcpy(node->name, name);
    node->type = type;
    node->parent = parent;
    node->finddir = vfs_default_finddir;
    
    if (type == VFS_FILE) {
        node->read = memfs_read;
        node->write = memfs_write;
    }
    
    return node;
}

void vfs_add_node(vfs_node_t *parent, vfs_node_t *node) {
    if (!parent || !node) return;
    node->next = parent->children;
    parent->children = node;
    node->parent = parent;
}

vfs_node_t* vfs_open(char *path) {
    if (!path || path[0] != '/') return NULL;
    if (path[1] == '\0') return root_fs;
    
    vfs_node_t *current = root_fs;
    char *p = path + 1;
    char name[VFS_MAX_NAME];
    
    while (*p) {
        int i = 0;
        while (*p && *p != '/') name[i++] = *p++;
        name[i] = '\0';
        if (*p == '/') p++;
        
        if (!current->finddir) return NULL;
        current = current->finddir(current, name);
        if (!current) return NULL;
    }
    return current;
}

uint32_t vfs_read(vfs_node_t *node, uint32_t offset, uint32_t size, uint8_t *buf) {
    if (!node || !node->read) return 0;
    return node->read(node, offset, size, buf);
}

uint32_t vfs_write(vfs_node_t *node, uint32_t offset, uint32_t size, uint8_t *buf) {
    if (!node || !node->write) return 0;
    return node->write(node, offset, size, buf);
}

vfs_node_t* vfs_finddir(vfs_node_t *dir, char *name) {
    if (!dir || !dir->finddir) return NULL;
    return dir->finddir(dir, name);
}

void vfs_list(vfs_node_t *dir) {
    if (!dir || dir->type != VFS_DIR) return;
    vfs_node_t *child = dir->children;
    char *types[] = {"FILE", "DIR ", "BLK ", "CHR "};
    while (child) {
        printk("%s %s (%d bytes)\n", types[child->type], child->name, child->size);
        child = child->next;
    }
}

void init_vfs() {
    vfs_init();   // /, /dev, /proc

    if (!initrd_start || !initrd_end) {
        printk("No initrd\n");
        return;
    }

    uint8_t *data = (uint8_t*)initrd_start;
    uint32_t size = initrd_end - initrd_start;
    printk("initrd: 0x%x - 0x%x (%d KB)\n", initrd_start, initrd_end, size/1024);

    if (size > 512 && data[257] == 'u' && data[258] == 's') {
        printk("Tar signature found\n");
        uint32_t off = 0;
        while (off < size && data[off] != '\0') {
            char *fname = (char*)(data + off);

            if (fname[0] == '.' && fname[1] == '/')
                fname += 2;

            char *sz = (char*)(data + off + 124);
            uint32_t fsize = 0;
            while (*sz >= '0' && *sz <= '7')
                fsize = fsize * 8 + (*sz++ - '0');

            uint32_t type = (data[off + 156] == '5') ? VFS_DIR : VFS_FILE;
            int namelen = strlen(fname);
            if (namelen > 0 && fname[namelen-1] == '/')
                fname[namelen-1] = '\0';

            printk("[%s] size=%d\n", fname, fsize);

            vfs_node_t *node = vfs_create_node(fname, type, NULL);
            if (type == VFS_FILE) {
                node->priv = data + off + 512;
                node->size = fsize;
                node->read = memfs_read;
                node->write = NULL;
            }
            vfs_add_node(root_fs, node);

            off += 512 + ((fsize + 511) / 512) * 512;
        }
        printk("Ramdisk mounted\n");
    } else {
        printk("No tar signature\n");
    }
}