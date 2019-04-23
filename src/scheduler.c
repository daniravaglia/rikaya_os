#include "const.h"
#include "listx.h"
#include <umps/libumps.h>
#include <pcb.h>
#include <scheduler.h>


struct devregarea *reg_area = (devregarea_t*) 0x10000000;

/*funzione per fare debug*/
void log_priority(unsigned int value) {}

void scheduler()
{
    
    if ((active_proc = removeProcQ(&ready_queue)) != NULL)
    {
        /* ripristino la priorità originale del active_proc */
        active_proc->priority = active_proc->orig_priority;

        log_process_order(active_proc->orig_priority);
        
       /* incremento la priority di tutti i processi rimasti in coda */
        struct list_head* iter;
	    list_for_each(iter,&ready_queue) {
		    pcb_t *item=container_of(iter,struct pcb_t,p_next);		
		    item->priority += 1; 	
	    }
	    
	    reg_area->intervaltimer = TIME_SLICE;
        LDST(&(active_proc->p_s));
    }
    else {
        /*non ci sono processi nella ready queue*/
        HALT();
    }

}











