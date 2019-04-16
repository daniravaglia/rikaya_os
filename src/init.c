#include <listx.h>
#include <const.h>
#include <pcb.h>
#include <umps/libumps.h>
#include <handler.h>
#include <scheduler.h>
/* important status register bits */
#define OFF 0x00000000
#define IEc 0x00000001
/*#define KUc 0x00000002
#define IEp 0x00000004 */
#define KUp 0x00000008
/*#define IEo 0x00000010
#define KUo 0x00000020 */
#define IM  0x0000FF00
/*#define BEV 0x00400000
#define VMc 0x01000000
#define VMp 0x02000000
#define VMo 0x04000000 */
#define TE  0x08000000
/* #define CU  0x10000000 */
#define INTERRUPTSON 0x00000004



void populate(state_t *area, memaddr addr)
{
    state_t *new_area = area;
    STST(new_area);
    new_area->pc_epc = addr;
    new_area->reg_sp = (memaddr) RAMTOP;
    new_area->status = 0; /* tutti i bit settati a 0 */
}

void init_pcb()
{

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
    struct pcb_t *test1 = allocPcb();
    /*0010 1000 0000 0000 1111 1111 0000 0001*/
    //STST(&(test1->p_s));
    STST(&(test1->p_s));
    //test1->p_s.status = (OFF | INTERRUPTSON | IM | TE);
    test1->p_s.status = 0;
    test1->p_s.reg_sp = (memaddr) RAMTOP - FRAMESIZE * 1;
    test1->p_s.pc_epc = (memaddr) test1;
    test1->orig_priority = 1;
    test1->priority = 1;


    
    /*TEST 2*/
    struct pcb_t *test2 = allocPcb();
    STST(&(test2->p_s));
    test2->p_s.status = 0x1000FF01;
    test2->p_s.reg_sp = (memaddr) RAMTOP - FRAMESIZE * 2;
    test2->p_s.pc_epc = (memaddr) test1;
    test2->orig_priority = 2;
    test2->priority = 2;
    
    /*TEST 3*/
    struct pcb_t *test3 = allocPcb();
    STST(&(test3->p_s));
    test3->p_s.status = (unsigned int) 0x1000FF01;
    test3->p_s.reg_sp = (memaddr) RAMTOP - FRAMESIZE * 3;
    test3->p_s.pc_epc = (memaddr) test1;
    test3->orig_priority = 3;
    test3->priority = 3;
    
    insertProcQ(&ready_queue, test1);
    insertProcQ(&ready_queue, test2);
    insertProcQ(&ready_queue, test3);
    
    //setTIMER(TIME_SLICE);
    
    scheduler();
    adderrbuf("returned from the scheduler");
}
