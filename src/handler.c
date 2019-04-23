#include <const.h>
#include <pcb.h>
#include <scheduler.h>
#include <listx.h>
#include <umps/libumps.h>
#include <handler.h>
#include <umps/arch.h>



void debug1(unsigned int value){ }

void sys_break_handler()
{   
   state_t *oldState = (state_t *) SYSOLDAREA;

   unsigned int sysCallNumber = oldState->reg_a0;
   


   switch (sysCallNumber)
   {
      case SYS3:
         terminate_process();
         break;
   
   }
} 

//SYSCALL 3

void terminate_process()
{
  outProcQ(&ready_queue, active_proc);
  scheduler();
}


void prgrm_trap_handler() 
{
    //PANIC();
}


void tlb_mngmt_handler() 
{
    //PANIC();
}

void intrpt_handler() 
{
    /*leggo la causa*/
    
    /*prendo solo i bit riguardanti Cause.ExcCode*/
    //unsigned int cause = ((area->cause >> 2) & 0x1F);
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

        //active_proc->p_s.pc_epc += WORD_SIZE;
        
        insertProcQ(&ready_queue, active_proc);
        scheduler();
    }
    else {
        PANIC();
    }
}
