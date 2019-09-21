#include "listx.h"

struct list_head ready_queue, wait_queue, printQ;
struct pcb_t *active_proc;
void scheduler(int waiting);
void idle();
