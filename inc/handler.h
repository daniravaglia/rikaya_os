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

void cpystate(state_t *dest, state_t *src);

unsigned int UpdateTime(unsigned int start);

void Passeren(int *sem);

void Verhogen(int *sem, int value);

int CreateProcess(state_t *statep, int priority,  void **cpid);

void WaitIO(unsigned int *dev_reg, unsigned int command);

void WaitClock();

void GetPid(void **pid, void **ppid);

int SpecPassup(int type, state_t *old, state_t *new);

void SetTutor();

int TerminateProcess(void **pid);

void GetCpuTime(unsigned int *user, unsigned int *kernel, unsigned int *wallclock);

void Terminal();

void IntervalTimer(state_t *old_area);

void PrintTapeDisk(int line);

void sys_break_handler();

void prgrm_trap_handler();

void tlb_mngmt_handler();

void intrpt_handler();   

