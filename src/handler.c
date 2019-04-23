#include <const.h>
#include <pcb.h>
#include <scheduler.h>
#include <listx.h>
#include <umps/libumps.h>
#include <handler.h>
#include <umps/arch.h>

/*funzione per debug*/
void debug1(unsigned int value){ }

/*rimozione di tutta la progenie del processo "father" dalla ready queue*/
void kill_em_all(struct pcb_t *father)
{
    /*rimuovo il padre dalla ready queue*/
    outProcQ(&ready_queue, father);
    
    /*se non ha figli mi fermo*/
    if (!emptyChild(father))
    {
        struct pcb_t *child = container_of(father->p_child.next, struct pcb_t, p_sib);
        struct list_head *pos;
        
        /*richiamo la funzione sul primo figlio*/
        kill_em_all(child);
        
        list_for_each(pos, &(child->p_sib))
        {
            struct pcb_t *sib = container_of(pos, struct pcb_t, p_sib);
            /*richiamo la funzione per tutti i restanti figli*/
            kill_em_all(sib);
        }
    }
    else
        return;
}

void sys_break_handler()
{
    state_t *old_area = (state_t*) 0x20000348;
    
    /*prendo solo i bit riguardanti la causa dell'eccezione*/
    unsigned int exc_code = ((getCAUSE() >> 2) & 0x1F);
    
    /* se il codice riguarda una system call*/
    if (exc_code == 8)
    {
        unsigned int syscall = old_area->gpr[3];
        switch (syscall)
        {
            /*termina il processo corrente e tutta la sua progenie, rimuovendoli 
              dalla Ready Queue.*/
            case SYS3:
                kill_em_all(active_proc);
                active_proc = NULL; 
                scheduler();
        }
    }
}

void prgrm_trap_handler() 
{
    PANIC();
}


void tlb_mngmt_handler() 
{
    PANIC();
}

void intrpt_handler() 
{
   
    /*prendo solo il bit riguardante l'interval timer*/
    unsigned int device = ((getCAUSE() >> 8) & 0x4) >> 2;
    
    /*Interval Timer*/
    if (device == 0x1)
    {
        state_t *old_area = (state_t*) 0x20000000;
        /* ripristino lo stato */
        active_proc->p_s.entry_hi = old_area->entry_hi;
        active_proc->p_s.cause = old_area->cause;
        active_proc->p_s.status = old_area->status;
        active_proc->p_s.pc_epc = old_area->pc_epc;
        for (int i = 0; i<31; i++)
        {
            active_proc->p_s.gpr[i] = old_area->gpr[i];
        }
        active_proc->p_s.hi = old_area->hi;        
        active_proc->p_s.lo = old_area->lo;
        
        insertProcQ(&ready_queue, active_proc);
        scheduler();
    }
    else {
        PANIC();
    }
}
