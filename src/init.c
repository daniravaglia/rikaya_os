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

int main(void)
{
    populate((state_t*) SYSCALL_BREAK_NEWAREA, (memaddr) sys_break_handler);
    populate((state_t*) PRGM_TRAP_NEWAREA, (memaddr) prgrm_trap_handler);
    populate((state_t*) TLB_MNGMT_NEWAREA, (memaddr) tlb_mngmt_handler);
    populate((state_t*) INTRPT_NEWAREA, (memaddr) intrpt_handler);
    
    initPcbs();
    struct list_head ready_queue;
    //LIST_HEAD(&ready_queue);
    
    //struct pcb_t *root = allocPcb();
}
