#include "const.h"
#include "listx.h"
#include <umps/libumps.h>
#include <pcb.h>
#include <scheduler.h>

// APPUNTI AUDIO
// 
// Lo scheduler è una funzione che viene chiamata ogni tot secondi per fare
// context switch (3 millisecodni)
//
//ogni volta che scadono i 3 millisec viene invocato lo scheduler
// 
// Mette da parte il processo in esecuzione e attiva il prossimo nella coda
// dei processi pronti
//
// TODO aggiungere un processo di aging grazie al origin priority: ogni volta 
// che viene fatto context switch si aumenta di 1 il campo prioity di ogni 
// processo, altrimenti viene chiamato sempre il processo 3
//
// TODO resettare quindi al processo che viene chiamato la priority sennò stai
// da capo
//
//
//
//
// Inoltre: ogni volta che facciamo la context switch va chaimata la funziione
// di log che sta nel test, passandogli come campo la original priority che 
// fuzniona come una sorta di ID del processo.
// Questo serve per debuggare, è una funzione che viene chiamata dall'ultimo 
// processo e che dovrebbe stampare "3 2 3 1 2 3 1 2 3 1 2 3" ecc ecc

#define IT 0x10000020




struct devregarea *reg_area = 0x10000000;


void scheduler()
{
    
    pcb_t *proc = removeProcQ(&ready_queue);
    /* ripristino la priorità originale del proc */
    proc->priority = proc->orig_priority;
    log_process_order(proc->orig_priority);
    /* incremento la priority di tutti i processi rimasti in coda */
    struct list_head* iter;
	list_for_each(iter,&ready_queue) {
		pcb_t *item=container_of(iter,struct pcb_t,p_next);
		//TODO loggare priority
		item->priority += 1; 	
	}
	
	reg_area->intervaltimer = TIME_SLICE;
    setTIMER(TIME_SLICE);
    //IT = TIME_SLICE;
    LDST(&(proc->p_s));

}











