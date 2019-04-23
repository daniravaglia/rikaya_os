#include <const.h>
#include <pcb.h>
#include <scheduler.h>
#include <listx.h>
#include <umps/libumps.h>
#include <handler.h>
#include <umps/arch.h>

/*funzione per debug*/
void debug1(unsigned int value){ }

// Per questa fase va controlalta solo una syscall, ovvero quella che termina
// il processo. Ecco perche viene chiamata alla fine del test. "Per gestirla 
// dovete popolare la new area relativa a syscalle  brakpoint con handler
// e implementare questa syscall che termina il process( rumuoverlo dalla lis
// ta dei processi che devono essere eseguiti)"




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
                if (emptyChild(active_proc))
                {
                    active_proc = NULL;
                }
                else {
                    while (!(emptyChild(active_proc)))
                    {
                        struct pcb_t *child = removeChild(active_proc);
                        outProcQ(&ready_queue, child);
                    }
                    active_proc = NULL;
               }
               scheduler();
        }
    }
}


//SYSCALL 3

void kill_process(pcb_t* proc)
{
   //fare il loop che cerca nell'albero il processo da uccidere con removeChild
   // (il loop ricorsivo ovviamente)
   


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
    /*leggo la causa*/
    
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
