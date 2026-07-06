#ifndef BLK_H
#define BLK_H

#include <std/stdint.h>

#define NR_REQUEST 32
#define NR_BLK_DEV 7

#define READ   0
#define WRITE  1
#define READA  2
#define WRITEA 3

#define MAJOR(dev) ((dev) >> 8)

struct buffer_head {
    uint32_t b_dev;
    uint32_t b_blocknr;
    uint8_t *b_data;

    uint8_t b_lock;
    uint8_t b_dirt;
    uint8_t b_uptodate;

    void *b_wait;
};

struct request {
    uint32_t dev;
    uint32_t cmd;

    uint32_t errors;
    uint32_t sector;
    uint32_t nr_sectors;

    uint8_t *buffer;

    void *waiting;
    struct buffer_head *bh;

    struct request *next;
};

struct blk_dev_struct {
    void (*request_fn)(void);
    struct request *current_request;
};

#define IN_ORDER(s1, s2) \
    ((s1)->sector < (s2)->sector)

#endif