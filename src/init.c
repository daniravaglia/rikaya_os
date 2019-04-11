#include <listx.h>
#include <const.h>
#include <pcb.h>
#include <umps/libumps.h>
#include <handler.h>




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
    populate((state_t*) PRGM_TRAP_NEWAREA, (memaddr) prgrm_trap_handler);
    populate((state_t*) TLB_MNGMT_NEWAREA, (memaddr) tlb_mngmt_handler);
    populate((state_t*) INTRPT_NEWAREA, (memaddr) intrpt_handler);
    
    initPcbs();
    struct list_head ready_queue;
    INIT_LIST_HEAD(&ready_queue);
    
    struct pcb_t *test1 = allocPcb();
    
    /*0010 1000 0000 0000 1111 1111 0000 0001*/
    test1->p_s.status = 0x2800FF01;
    
    test1->p_s.reg_sp = 1; /* FRAMESIZE ??? */
    
    test1->p_s.pc_epc = (memaddr) test1;
    
    test1->priority = 1;
    
    struct pcb_t *test2 = allocPcb();
    struct pcb_t *test3 = allocPcb();
 
}
