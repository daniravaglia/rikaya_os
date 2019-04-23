#define SYSOLDAREA 0x20000348
#define SYS3 3

void sys_break_handler();

void kill_process();

void prgrm_trap_handler();

void tlb_mngmt_handler();

void intrpt_handler();   

