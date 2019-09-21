#include <const_rikaya.h>

struct semdev *sd;

struct semd_t *smdv;
int *p_termT[8];

SEMAPHORE	terminal_t[8],
    		terminal_r[8],
    		disk[8],
			tape[8],
			network[8],
			printer[8];

void sys_break_handler();

/*rimozione di tutta la progenie del processo "father" dalla ready queue*/
void kill_em_all(struct pcb_t *father);

void prgrm_trap_handler();

void tlb_mngmt_handler();

void intrpt_handler();   

