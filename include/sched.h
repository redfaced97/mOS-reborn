#ifndef SCHED_H
#define SCHED_H

#define TASK_RUNNING        0
#define TASK_INTERRUPTIBLE  1
#define TASK_UNINTERRUPTIBLE 2

struct task_struct {
    int state;
    struct task_struct *next;
};

extern struct task_struct *current;

extern void schedule(void);

extern void sleep_on(struct task_struct **p);
extern void interruptible_sleep_on(struct task_struct **p);
extern void wake_up(struct task_struct **p);

void panic(const char *str);

#endif