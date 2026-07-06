
#include <protected/protected.h>
#include <blk/blk.h>

#include <vsprintf.h>

#define SECTORS_PER_BLOCK 2

struct request request[NR_REQUEST];
static struct request *free_list = NULL;
struct task_struct *wait_for_request = NULL;

struct blk_dev_struct blk_dev[NR_BLK_DEV] = {
    { NULL, NULL },
    { NULL, NULL },
    { NULL, NULL },
    { NULL, NULL },
    { NULL, NULL },
    { NULL, NULL },
    { NULL, NULL }
};

static inline void lock_buffer(struct buffer_head *bh)
{
    cli();
    while (bh->b_lock) {
        sti();
        sleep_on(&bh->b_wait);
        cli();
    }
    bh->b_lock = 1;
    sti();
}

static inline void unlock_buffer(struct buffer_head *bh)
{
    if (!bh->b_lock)
        printk("blk: unlock of unlocked buffer\n");

    bh->b_lock = 0;
    wake_up(&bh->b_wait);
}

static struct request *alloc_request(void)
{
    struct request *req;

    cli();
    req = free_list;
    if (req)
        free_list = req->next;
    sti();

    return req;
}

static void free_request(struct request *req)
{
    cli();
    req->dev = -1;
    req->next = free_list;
    free_list = req;
    sti();
}

static inline int normalize_rw(int rw, int *ahead)
{
    *ahead = 0;

    if (rw == READA) {
        *ahead = 1;
        return READ;
    }

    if (rw == WRITEA) {
        *ahead = 1;
        return WRITE;
    }

    return rw;
}

static void fill_request(struct request *req, int rw, struct buffer_head *bh)
{
    req->dev = bh->b_dev;
    req->cmd = rw;
    req->errors = 0;
    req->sector = bh->b_blocknr * SECTORS_PER_BLOCK;
    req->nr_sectors = SECTORS_PER_BLOCK;
    req->buffer = bh->b_data;
    req->waiting = NULL;
    req->bh = bh;
    req->next = NULL;
}


static void add_request(struct blk_dev_struct *dev, struct request *req)
{
    struct request *tmp;

    if (!dev->request_fn) {
        printk("blk: no request_fn\n");
        return;
    }

    req->next = NULL;

    cli();

    if (req->bh)
        req->bh->b_dirt = 0;

    tmp = dev->current_request;

    if (!tmp) {
        dev->current_request = req;
        sti();
        dev->request_fn();
        return;
    }

    /* simple elevator */
    for (; tmp->next; tmp = tmp->next) {
        if ((IN_ORDER(tmp, req) ||
            !IN_ORDER(tmp, tmp->next)) &&
            IN_ORDER(req, tmp->next))
            break;
    }

    req->next = tmp->next;
    tmp->next = req;

    sti();
}


static void make_request(int major, int rw, struct buffer_head *bh)
{
    int rw_ahead;

    if (!bh || !bh->b_data) {
        printk("blk: invalid buffer\n");
        return;
    }

    rw = normalize_rw(rw, &rw_ahead);

    if (rw != READ && rw != WRITE)
        panic("blk: bad rw");

    lock_buffer(bh);

    if ((rw == WRITE && !bh->b_dirt) ||
        (rw == READ && bh->b_uptodate)) {
        unlock_buffer(bh);
        return;
    }

    struct request *req = alloc_request();

    if (!req) {
        if (rw_ahead) {
            unlock_buffer(bh);
            return;
        }
        sleep_on(&wait_for_request);
        unlock_buffer(bh);
        return;
    }

    fill_request(req, rw, bh);
    add_request(major + blk_dev, req);
}


void ll_rw_block(int rw, struct buffer_head *bh)
{
    unsigned int major;

    if (!bh) {
        printk("blk: null buffer\n");
        return;
    }

    major = MAJOR(bh->b_dev);

    if (major >= NR_BLK_DEV || !blk_dev[major].request_fn) {
        printk("blk: invalid device\n");
        return;
    }

    make_request(major, rw, bh);
}


void blk_dev_init(void)
{
    for (int i = 0; i < NR_REQUEST; i++) {
        request[i].dev = -1;
        request[i].next = free_list;
        free_list = &request[i];
    }
}