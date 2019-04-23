
void sys_break_handler();

/*rimozione di tutta la progenie del processo "father" dalla ready queue*/
void kill_em_all(struct pcb_t *father);

void prgrm_trap_handler();

void tlb_mngmt_handler();

void intrpt_handler();   

