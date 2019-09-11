#include <const.h>
#include <pcb.h>
#include <scheduler.h>
#include <listx.h>
#include <umps/libumps.h>
#include <handler.h>
#include <umps/arch.h>
#include "const_rikaya.h"

int *sem;
semdev *sem_dev;

/*funzione per debug*/
void debug1(unsigned int value){ }
void debug2(unsigned int value){ }
void debug3(unsigned int value){ }
void debug4(unsigned int value){ }
void debug5(unsigned int value){ }
void debug6(unsigned int value){ }
void debug7(unsigned int value){ }
void debug8(unsigned int value){ }
void debug9(unsigned int value){ }
void debug10(unsigned int value1, unsigned int value2, unsigned int value3, unsigned int value4){ }

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
            case TERMINATEPROCESS:
                kill_em_all(active_proc);
                active_proc = NULL; 
                scheduler();
                
            case VERHOGEN: 
            	debug1(0);
            	sem = old_area->gpr[4];
				*sem = *sem + 1;
				/* ripristino lo stato */
				active_proc->p_s.entry_hi = old_area->entry_hi;
				active_proc->p_s.cause = old_area->cause;
				active_proc->p_s.status = old_area->status;
				active_proc->p_s.pc_epc = old_area->pc_epc + WORD_SIZE;
				for (int i = 0; i<31; i++)
				{
				    active_proc->p_s.gpr[i] = old_area->gpr[i];
				}
				active_proc->p_s.hi = old_area->hi;        
				active_proc->p_s.lo = old_area->lo;
				
				insertProcQ(&ready_queue, active_proc);
				scheduler();
				
			case PASSEREN:
				debug2(0);
            	sem = old_area->gpr[4];
				*sem = *sem - 1;
				/* ripristino lo stato */
				active_proc->p_s.entry_hi = old_area->entry_hi;
				active_proc->p_s.cause = old_area->cause;
				active_proc->p_s.status = old_area->status;
				active_proc->p_s.pc_epc = old_area->pc_epc + WORD_SIZE;
				for (int i = 0; i<31; i++)
				{
				    active_proc->p_s.gpr[i] = old_area->gpr[i];
				}
				active_proc->p_s.hi = old_area->hi;        
				active_proc->p_s.lo = old_area->lo;
				
				insertProcQ(&ready_queue, active_proc);
				scheduler();
				
			case GETCPUTIME:
				debug3(0);		
			case CREATEPROCESS:
				debug4(0);
			case WAITCLOCK:
				debug5(0);
				
			case WAITIO:
				debug6(0);
				/*---parte semaforo---*/
				/*sem_dev->*/
				termreg_t *dev_reg = old_area->gpr[5];
				unsigned int command = old_area->gpr[4];
				dev_reg->transm_command = command;
				sem_dev->terminalT[0].
				/* ripristino lo stato */
				
				active_proc->p_s.entry_hi = old_area->entry_hi;
				active_proc->p_s.cause = old_area->cause;
				active_proc->p_s.status = old_area->status;
				active_proc->p_s.pc_epc = old_area->pc_epc + WORD_SIZE;
				for (int i = 0; i<31; i++)
				{
				    active_proc->p_s.gpr[i] = old_area->gpr[i];
				}
				active_proc->p_s.hi = old_area->hi;        
				active_proc->p_s.lo = old_area->lo;
				
				insertProcQ(&ready_queue, active_proc);
				scheduler();
				
			case SETTUTOR:
				debug7(0);
			case SPECPASSUP:
				debug8(0);
			case GETPID:
				debug9(0);
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
    /*unsigned int device = ((getCAUSE() >> 8) & 0x4) >> 2;*/
    /*1111 1111
      1000 0000 */
    unsigned int cause = getCAUSE() >> 8;
    
    /*Inter-processor interrupts*/
    if ((cause & 0x1) == 0x1)
    {
    
    }
    /*Processor Local Timer*/
    else if (((cause & 0x2) >> 1) == 0x1)
    {
    
    }
    /*Interval Timer*/
    else if (((cause & 0x4) >> 2) == 0x1)
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
    /*Terminal Devices*/
    else if (((cause & 0x80) >> 7) == 0x1)
    {
    	unsigned int *pendingdev = (memaddr) PENDING_BITMAP_START + (WORD_SIZE * (7 - 3));
    	termreg_t *dterm = (0x10000050 + ((7 - 3) * 0x80) + ((*pendingdev - 1) * 0x10));
    	/*termreg_t *dterm = 0x10000250;*/
    	debug10((dterm->transm_status & 0xFF), dterm->recv_status, (0x10000050 + ((7 - 3) * 0x80) + ((*pendingdev - 1) * 0x10)), dterm->recv_command);
    	if ((dterm->transm_status & 0xFF) == 5)
    	{
   			dterm->transm_command = DEV_C_ACK;
   		}
   		else if ((dterm->recv_status & 0xFF) == 5)
    	{
   			dterm->recv_command = DEV_C_ACK;
   		}
   		
   		
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
