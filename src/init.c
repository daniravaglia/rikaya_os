#include <listx.h>
#include <const.h>
#include <pcb.h>
#include <asl.h>
#include <umps/libumps.h>
#include <handler.h>
#include <scheduler.h>
#include "p1.5test_rikaya_v0.h"
#include <p2test_rikaya.h>
#include <types_rikaya.h>
#include <const_rikaya.h>

void bug1(unsigned int a){ }
void bug2(unsigned int a){}

void populate(state_t *area, memaddr addr)
{
    state_t *new_area = area;
    new_area->pc_epc = addr;
    new_area->reg_sp = (memaddr) RAMTOP;
    new_area->status = (memaddr) 0x10000000; /* tutti i bit settati a 0 */
    /*new_area->status = (memaddr) 0;*/
}

void populate2(state_t *area, memaddr addr)
{
    state_t *new_area = area;
    new_area->pc_epc = addr;
    new_area->reg_sp = (memaddr) RAMTOP;
    new_area->status = (memaddr) 0x1000FF04; /* tutti i bit settati a 0 */
}

int main(void)
{
    populate((state_t*) SYSCALL_BREAK_NEWAREA, (memaddr) sys_break_handler);
    populate((state_t*) PRGM_TRAP_NEWAREA,     (memaddr) prgrm_trap_handler);
    populate((state_t*) TLB_MNGMT_NEWAREA,     (memaddr) tlb_mngmt_handler);
    populate((state_t*) INTRPT_NEWAREA,        (memaddr) intrpt_handler);
    
    
    initPcbs();
    initASL();
    INIT_LIST_HEAD(&ready_queue);
    INIT_LIST_HEAD(&wait_queue);
    INIT_LIST_HEAD(&printQ);
    
	for (int i = 0; i < 8; i++)
	{
		terminal_t[i] = 0;
		terminal_r[i] = 0;
		disk[i] = 0;
		tape[i] = 0;
		network[i] = 0;
		printer[i] = 0;
	}
    
    /*p_termT[0] = (int) &terminal_t[0];*/
    
    /*TEST 1*/
    struct pcb_t *p1 = allocPcb();
    p1->p_s.status = (memaddr) 0x1000FF04;

    /*p1->p_s.status = (memaddr) (1<<2| 0xFF00 | 1<<27);*/

    p1->p_s.reg_sp = (memaddr) (RAMTOP - FRAMESIZE * 1);
    p1->p_s.pc_epc = (memaddr) test;
    p1->p_semkey = NULL;
    p1->orig_priority = 100;
    p1->priority = 100;
    p1->handler[0] = NULL;
    p1->handler[1] = NULL;
    p1->handler[2] = NULL;
    p1->old[0] = NULL;
    p1->old[1] = NULL;
    p1->old[2] = NULL;
    p1->timer = 0;
    
    /* IDLE PROC */
    struct pcb_t *idlep = allocPcb();
    /*idlep->p_s.status = (memaddr) (1<<2| 0xFF00 | 1<<27);*/
    idlep->p_s.status = (memaddr) 0x1000FF04;
    idlep->p_s.reg_sp = (memaddr) (RAMTOP - FRAMESIZE * 2);
    idlep->p_s.pc_epc = (memaddr) idle;
    idlep->p_semkey = NULL;
    idlep->orig_priority = 0;
    idlep->priority = 0;
    idlep->handler[0] = NULL;
    idlep->handler[1] = NULL;
    idlep->handler[2] = NULL;
    idlep->old[0] = NULL;
    idlep->old[1] = NULL;
    idlep->old[2] = NULL;
    idlep->timer = 0;
    /*TEST 2*/
    /*
    struct pcb_t *p2 = allocPcb();
    p2->p_s.status = (memaddr) 0x10000404;
    p2->p_s.reg_sp = (memaddr) (RAMTOP - FRAMESIZE * 2);
    p2->p_s.pc_epc = (memaddr) test2;
    p2->orig_priority = 2;
    p2->priority = 2;
    */
    /*TEST 3*/
    /*
    struct pcb_t *p3 = allocPcb();
    p3->p_s.status = (memaddr) 0x10000404;
    p3->p_s.reg_sp = (memaddr) (RAMTOP - FRAMESIZE * 3);
    p3->p_s.pc_epc = (memaddr) test3;
    p3->orig_priority = 3;
    p3->priority = 3;
    */
    insertProcQ(&ready_queue, p1);
    insertProcQ(&ready_queue, idlep);
    /*
    insertProcQ(&ready_queue, p2);
    insertProcQ(&ready_queue, p3);
	*/
    scheduler(0);
    return 0;
}
