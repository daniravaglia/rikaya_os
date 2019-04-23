Struttura directory:

    rykaya_os
    |
    |---src/    => .c files 
    |
    |---inc/    => .h files
    |
    |---obj/    => .o files
    |
    |---Makefile

Per compilare eseguire il comando "make" all'interno della directory "rikaya_os".
I files per umps2 verranno generati all'interno della stessa. 

Il codice è stato diviso in 4 files: init.c, scheduler.c, handler.c e pcb.c (esclusi i file per i test).

init.c:
    File che contiene il main; qui vengono inizializzate le new areas per la gestione delle eccezioni
    e i 3 pcb che gestiranno i 3 test.
    Una volta completata l'inizializzazione viene chiamato lo scheduler.
    
scheduler.c:
    Lo scheduler si occupa di inizializzare l'interval timer e caricare lo stato di un nuovo processo (estratto dalla ready_queue).

handler.c:
    Contiene tutte le funzioni per gestire syscall, trap e interrupt.
    Nello specifico in questa fase gestisce solo l'interrupt relativo all'intervall timer e la system call per la terminazione di un processo.
    In caso di altre eccezioni sarà invocata la funzione di PANIC. 

pcb.c:
    Qui sono contenute tutte le funzioni relative ai pcb, implementate durante fase 1. 
    
    
