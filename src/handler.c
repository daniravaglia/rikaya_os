#include <const.h>
#include <pcb.h>
#include <scheduler.h>
#include <listx.h>
#include <umps/libumps.h>
#include <handler.h>
#include <umps/arch.h>

<<<<<<< HEAD
void achtung1(unsigned int tmp){ }
void achtung2(){ }
=======
// Per questa fase va controlalta solo una syscall, ovvero quella che termina
// il processo. Ecco perche viene chiamata alla fine del test. "Per gestirla 
// dovete popolare la new area relativa a syscalle  brakpoint con handler
// e implementare questa syscall che termina il process( rumuoverlo dalla lis
// ta dei processi che devono essere eseguiti)"



>>>>>>> c076d07021e20791e9b079a221ddedeb910cd4e5
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

<<<<<<< HEAD
void prgrm_trap_handler() 
{
    PANIC();
}
=======



void prgrm_trap_handler() {}
>>>>>>> c076d07021e20791e9b079a221ddedeb910cd4e5

void tlb_mngmt_handler() 
{
    PANIC();
}

void intrpt_handler() 
{
    /*leggo la causa*/
    
    state_t *area = (state_t*) INTRPT_NEWAREA;
    /*prendo solo i bit riguardanti Cause.ExcCode*/
    //unsigned int cause = ((area->cause >> 2) & 0x1F);
    unsigned int device = ((getCAUSE() >> 8) & 0x4) >> 2;
    achtung1(device);
    
    /*Interval Timer*/
    if (device == 0x1)
    {
        state_t *old_area = (state_t*) 0x20000000;
        old_area->pc_epc += WORD_SIZE;
        //TODO capire come re-inserire il pcb nella ready queue
        struct pcb_t *proc = container_of(old_area, struct pcb_t, p_s);
        //insertProcQ(&ready_queue, proc);
        achtung2();
        scheduler();
    }
    else {
        PANIC();
    }
    /*
    switch (device)
    {
        case 0x0:
            //interrupt caused by a device
            //unsigned int device = ((area->cause >> 2) & 0x1F);
            adderrbuf("0");
            
        case 0x1:
            adderrbuf("1");
        case 0x2:
            adderrbuf("2");
        case 0x3:
            adderrbuf("3");
        case 0x4:
            adderrbuf("4");
        case 0x5:
            adderrbuf("5");
        case 0x6:
            adderrbuf("6");
        case 0x7:
            adderrbuf("7");
        case 0x8:
            adderrbuf("8");
        case 0x9:
            adderrbuf("9");
        case 0xA:
            adderrbuf("a");
        case 0xB:
            adderrbuf("b");
        case 0xC:
            adderrbuf("c");
        case 0xD:
            adderrbuf("d");
        case 0xE:
            adderrbuf("e");
        case 0xF:
            adderrbuf("f");
        default:
            adderrbuf("fuck");
    }*/
    /*
    switch (cause)
    {
        case 0x0:
            //interrupt caused by a device
            unsigned int device = ((area->cause >> 2) & 0x1F);
            
        case 0x1:
        
        case 0x2:
        
        case 0x3:
        
        case 0x4:
        
        case 0x5:
        
        case 0x7:
        
        case 0x8:
        
        case 0x9:
        
        case 0xA:
        
        case 0xB:
        
        case 0xC:
        
        case 0xD:
        
        case 0xE:
        
        case 0xF:
        
        default:
            achtung2();
    }*/
    
}
