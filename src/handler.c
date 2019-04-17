#include "const.h"
#include "listx.h"
#include <umps/libumps.h>
#include <handler.h>

// Per questa fase va controlalta solo una syscall, ovvero quella che termina
// il processo. Ecco perche viene chiamata alla fine del test. "Per gestirla 
// dovete popolare la new area relativa a syscalle  brakpoint con handler
// e implementare questa syscall che termina il process( rumuoverlo dalla lis
// ta dei processi che devono essere eseguiti)"



void sys_break_handler()
{
   //ci sono da fare ovviamente cose prima ma ad una certa c'è da fare lo switch
   //delle syscall
   
   state_t* oldState;

   pcb_t* process;
  
   //inseirire l'indirizzo della oldarea

   //questo pezzo l'ho preso da coso poi lo si vede bene
   // per adesso serve come bozza
   int sysCallNumber = oldState->s_a0;

   switch (sysCallNumber)
   {
      case 3:
         kill_process();
         break;

   }
}


//SYSCALL 3

void kill_process(pcb_t* proc)
{
   //fare il loop che cerca nell'albero il processo da uccidere con removeChild
   // (il loop ricorsivo ovviamente)
   


}




void prgrm_trap_handler() {}

void tlb_mngmt_handler() {}

void intrpt_handler() {}
