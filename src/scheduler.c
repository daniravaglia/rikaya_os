#include "const.h"
#include "listx.h"
#include <umps/libumps.h>
#include <pcb.h>
#include <scheduler.h>
#include <const_rikaya.h>
#include <handler.h>

struct devregarea *reg_area = (devregarea_t*) 0x10000000;


void idle(){

	while(1){
        ;;
	}
}
void scheduler(int waiting)
{
    
    if ((active_proc = removeProcQ(&ready_queue)) != NULL)
    {
        /* ripristino la priorità originale del active_proc */
        active_proc->priority = active_proc->orig_priority;
        
        /* incremento la priority di tutti i processi rimasti in coda */
        struct list_head* iter;
	    list_for_each(iter,&ready_queue) {
		    pcb_t *item=container_of(iter,struct pcb_t,p_next);	
		    if (item->priority != 0)
		    	item->priority += 1; 	
	    }
	    /*se è la prima volta che viene eseguito mi segno il tempo di inizio */
        if (active_proc->tot_time == 0)
        {
            active_proc->tot_time = TOD_LO;
        }
        /* time management */
        active_proc->usr_start = TOD_LO;
        
	    reg_area->intervaltimer = SCHED_TIME_SLICE;
        LDST(&(active_proc->p_s));
    }
    else {
        /*non ci sono processi nella ready queue*/
        HALT();
    }
}











