#include "const.h"
#include "listx.h"
#include <umps/libumps.h>
#include <pcb.h>
#include <scheduler.h>
#include <const_rikaya.h>
#include <handler.h>

struct devregarea *reg_area = (devregarea_t*) 0x10000000;

/*funzione per fare debug*/
void log_priority(unsigned int value) {}
void logsem(int a){}
void idle(){

	while(1){
		logsem(terminal_t[0]);
	}
}
void scheduler(int waiting)
{
    
    if ((active_proc = removeProcQ(&ready_queue)) != NULL)
    {
        /* ripristino la priorità originale del active_proc */
        active_proc->priority = active_proc->orig_priority;

        /*log_process_order(active_proc->orig_priority);*/
        
        /* incremento la priority di tutti i processi rimasti in coda */
        struct list_head* iter;
	    list_for_each(iter,&ready_queue) {
		    pcb_t *item=container_of(iter,struct pcb_t,p_next);	
		    if (item->priority != 0)
		    	item->priority += 1; 	
		    log_priority(item->priority);
	    }

	    reg_area->intervaltimer = SCHED_TIME_SLICE;
	    /*setTIMER(SCHED_TIME_SLICE);*/
        LDST(&(active_proc->p_s));
    }
    else {
        /*non ci sono processi nella ready queue*/
        HALT();
    }
}











