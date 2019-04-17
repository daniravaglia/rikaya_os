#include <listx.h>
#include <const.h>
#include <pcb.h>
#include <umps/libumps.h>
#include <handler.h>
#include <scheduler.h>
#include "p1.5test_rikaya_v0.h"

#define IT 0x10000020

void populate(state_t *area, memaddr addr)
{
    state_t *new_area = area;
    STST(new_area);
    new_area->pc_epc = addr;
    new_area->reg_sp = (memaddr) RAMTOP;
    new_area->status = 0x10000000; /* tutti i bit settati a 0 */
}

int main(void)
{
    populate((state_t*) SYSCALL_BREAK_NEWAREA, (memaddr) sys_break_handler);
    populate((state_t*) PRGM_TRAP_NEWAREA,     (memaddr) prgrm_trap_handler);
    populate((state_t*) TLB_MNGMT_NEWAREA,     (memaddr) tlb_mngmt_handler);
    populate((state_t*) INTRPT_NEWAREA,        (memaddr) intrpt_handler);
    
    initPcbs();
    INIT_LIST_HEAD(&ready_queue);
    
    /*TEST 1*/
    struct pcb_t *p1 = allocPcb();
    /*0010 1000 0000 0000 1111 1111 0000 0001*/
    p1->p_s.status = (unsigned int) 0x1000FF01;
    p1->p_s.reg_sp = (memaddr) (RAMTOP - FRAMESIZE * 1);
    p1->p_s.pc_epc = (memaddr) test1;
    p1->orig_priority = 1;
    p1->priority = 1;
    
    /*TEST 2*/
    struct pcb_t *p2 = allocPcb();
    p2->p_s.status = 0x1000FF01;
    p2->p_s.reg_sp = (memaddr) (RAMTOP - FRAMESIZE * 2);
    p2->p_s.pc_epc = (memaddr) test2;
    p2->orig_priority = 2;
    p2->priority = 2;
    
    /*TEST 3*/
    struct pcb_t *p3 = allocPcb();
    //p3->p_s.status = (0x1000FF00 | 0x1);
    p3->p_s.status = 0x1000FF04;
    p3->p_s.reg_sp = (memaddr) (RAMTOP - FRAMESIZE * 3);
    p3->p_s.pc_epc = (memaddr) test3;
    p3->orig_priority = 3;
    p3->priority = 3;
    
    insertProcQ(&ready_queue, p1);
    insertProcQ(&ready_queue, p2);
    insertProcQ(&ready_queue, p3);

    scheduler();
    adderrbuf("returned from the scheduler");
}
