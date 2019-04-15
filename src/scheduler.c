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







void scheduler()
{
    pcb_t *proc = removeProcQ(&ready_queue);
    LDST(&(proc->p_s));
}
