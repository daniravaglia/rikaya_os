#include <const.h>
#include <pcb.h>
#include <scheduler.h>
#include <listx.h>
#include <umps/libumps.h>
#include <handler.h>
#include <umps/arch.h>
#include "const_rikaya.h"
#include <types_rikaya.h>
#include <const_rikaya.h>
#include <asl.h>

void cpystate(state_t *dest, state_t *src)
{
	dest->entry_hi = src->entry_hi;
	dest->cause = src->cause;
	dest->status = src->status;
	dest->pc_epc = src->pc_epc;
	for (int i = 0; i<31; i++)
	{
		dest->gpr[i] = src->gpr[i];
	}
	dest->hi = src->hi;        
	dest->lo = src->lo;
}

unsigned int UpdateTime(unsigned int start)
{
    unsigned int tot = (TOD_LO - start);
    return tot;
}

void Passeren(int *sem)
{

	*sem -= 1;

	if (*sem < 0)
	{
		/*blocco il proc in coda al sem*/
		struct pcb_t *proc = active_proc;
		if (insertBlocked(sem,proc))
		{
			/*errore, allocati tutti i sem disponibili*/
			PANIC();
		}
		cpystate(&proc->p_s, (state_t *) SYSBK_OLDAREA);
		proc->p_s.pc_epc += WORD_SIZE;
		active_proc->ker_tot += TOD_LO - active_proc->ker_start;
		active_proc = NULL;
		scheduler(0);
	}
}

void Verhogen(int *sem, int value)
{

	*sem += 1;

	if(*sem <= 0)
	{
		/*sblocco un proc in coda dal sem e lo metto nella readyq*/
		struct pcb_t *proc = removeBlocked(sem);
		if (proc != NULL)
		{
			if (value != 0)
			{
				proc->p_s.gpr[1] = value;
			}
			proc->p_semkey = NULL;
			insertProcQ(&ready_queue, proc);
		}
	}
}

int CreateProcess(state_t *statep, int priority,  void **cpid)
{
	struct pcb_t *child = allocPcb();
	if (child != NULL)
	{
		cpystate(&(child->p_s), statep);
		child->p_s.status = statep->status; //| (1<<2);
		
		child->handler[0] = NULL;
		child->handler[1] = NULL;
		child->handler[2] = NULL;
		child->old[0] = NULL;
		child->old[1] = NULL;
		child->old[2] = NULL;
		child->p_semkey = 0;
		child->isTutor = 0;
		child->priority = priority;
		child->orig_priority = priority;
		child->timer = 0;
		child->tot_time = 0;
		
		insertChild(active_proc, child);
		
		if (cpid != NULL)
		{
			*cpid = child;
		}
		insertProcQ(&ready_queue, child);
		return 0;
	}
	else {
		return -1;
	}
}

void WaitIO(unsigned int *dev_reg, unsigned int command)
{   
	if (dev_reg < (unsigned int *) TERM0ADDR)
	{
		//not a terminal
		dtpreg_t *dev = (dtpreg_t *) dev_reg;
		dev->command = command;
		/*Disk*/
		if (dev_reg < (unsigned int *) TAPE0 && dev_reg > (unsigned int *) DISK0)
		{
		    /*controllo quale degli 8 devices fa l'operazione*/
		    for (int i = 0; i < 8; i++)
		    {
		        if (dev_reg == (unsigned int *) DISK0 + i*0x80)
		        {
		            Passeren(&disk[i]);
		        }
		    }
		}
		/*Tape*/
		else if (dev_reg < (unsigned int *) NETWORK0 && dev_reg > (unsigned int *) TAPE0)
		{
		    for (int i = 0; i < 8; i++)
		    {
		        if (dev_reg == (unsigned int *) TAPE0 + i*0x80)
		        {
		            Passeren(&tape[i]);
		        }
		    }
		}
		/*Printer*/
		else if (dev_reg < (unsigned int *) TERM0ADDR && dev_reg > (unsigned int *) PRINTER0)
		{
		    for (int i = 0; i < 8; i++)
		    {
		        if (dev_reg == (unsigned int *) PRINTER0 + i*0x80)
		        {
		            Passeren(&printer[i]);
		        }
		    }
		}
		
	}
	else
	{
	    termreg_t *dterm = (termreg_t *) dev_reg;
	    if ((command & 0xFF00) << 8)
	    {
	        /*transmitter*/ 
	        dterm->transm_command = command;
	        for (int i = 0; i < 8; i++)
		    {
		        if (dev_reg == (unsigned int *) TERM0ADDR + i*0x80)
		        {
		            Passeren(&terminal_t[i]);
		        }
		    }
	    }
	    else
	    {
	        /*receiver*/
	        dterm->recv_command = command;
	        for (int i = 0; i < 8; i++)
		    {
		        if (dev_reg == (unsigned int *) TERM0ADDR + i*0x80)
		        {
		            Passeren(&terminal_r[i]);
		        }
		    }
	    }
	}	
}

void WaitClock()
{
    cpystate(&(active_proc->p_s), (state_t *) SYSBK_OLDAREA);
	active_proc->p_s.pc_epc += WORD_SIZE;
	struct pcb_t *wproc = active_proc;
	insertProcQ(&wait_queue, wproc);
	active_proc->ker_tot += TOD_LO - active_proc->ker_start;
	active_proc = NULL;
	scheduler(0);
}

void GetPid(void **pid, void **ppid)
{
    if (pid != NULL)
    {
        *pid = active_proc;
    }
    else if (ppid != NULL)
    {
        *ppid = active_proc->p_parent;
    }
}

int SpecPassup(int type, state_t *old, state_t *new)
{
    if (active_proc->handler[type] != NULL)
    {
        return -1;
    } 
    else 
    {
        active_proc->handler[type] = new;
        active_proc->old[type] = old;
        return 0;
    }
}

void SetTutor()
{
    active_proc->isTutor = 1;
}
int TerminateProcess(void **pid)
{
    struct pcb_t *new_parent;
    struct pcb_t *child;
    struct pcb_t *ancestor;
    /* se devo terminare il processo corrente */
    if (pid == 0 || pid == NULL)
    {
        new_parent = active_proc->p_parent;
        /*cerco il tutor o il primo avo */
        while (new_parent->isTutor != 1 && new_parent->p_parent != NULL)
        {
            new_parent = new_parent->p_parent;
        }
        /*controllo se sto terminando un processo root */
        if (new_parent == active_proc)
        {
            while ((child = removeChild(active_proc)) != NULL)
            {
                if (child->p_semkey != NULL)
                {
                    (*child->p_semkey)++;
                    outBlocked(child);
                    child->p_semkey = NULL;
                }
                outChild(child);
                outProcQ(&wait_queue, child);     /*può essere nella waitq o nella readyq*/
                outProcQ(&ready_queue, child);
                freePcb(child);
            }
        }
        else
        {
            /* faccio adottare i figli ad un avo/tutor */
            while ((child = removeChild(active_proc)) != NULL)
            {
                insertChild(new_parent, child);
            }
        }
        /* controllo se il processo era in attesa su qualche semaforo ed/o eventuali code */
        if (active_proc->p_semkey != NULL)
        {
            (*active_proc->p_semkey)++;
            outBlocked(active_proc);
            active_proc->p_semkey = NULL;
        }
        outChild(active_proc);
        freePcb(active_proc);
        active_proc = NULL;
        scheduler(0);
        return 1;
    }
    else 
    {
        /*struct pcb_t *tokill = (pcb_t *) *pid;*/
        struct pcb_t *tokill = *((pcb_t **)pid);
        ancestor = tokill->p_parent;
        /*verifico che il processo da terminare sia discendente del processo corrente */
        while (ancestor != active_proc && ancestor->p_parent != NULL)
        {
            ancestor = ancestor->p_parent;
        }
        if (ancestor == active_proc)
        {
            new_parent = tokill->p_parent;
            /*cerco il tutor o il primo avo */
            while (new_parent->isTutor != 1 && new_parent->p_parent != NULL)
            {
                new_parent = new_parent->p_parent;
            }
            /* faccio adottare i figli ad un avo/tutor */
            while ((child = removeChild(tokill)) != NULL)
            {
                insertChild(new_parent, child);
            }
            /* controllo se il processo era in attesa su qualche semaforo ed/o eventuali code */
            if (tokill->p_semkey != NULL)
            {       
                (*tokill->p_semkey)++;
                outBlocked(tokill);
                tokill->p_semkey = NULL;
            }
            outProcQ(&wait_queue, tokill);     /*può essere nella waitq o nella readyq*/
            outProcQ(&ready_queue, tokill);
            outChild(tokill);
            freePcb(tokill);
            tokill = NULL;
            return 0;  
        }
        else
            return -1;
    }    
}

void GetCpuTime(unsigned int *user, unsigned int *kernel, unsigned int *wallclock)
{

    if (wallclock != NULL)
    {
        *wallclock = (TOD_LO - active_proc->tot_time);
    }
    if (kernel != NULL)
    {
        active_proc->ker_tot += UpdateTime(active_proc->ker_start);
        *kernel = active_proc->ker_tot;
    }
    if (user != NULL)
    {
        *user = active_proc->usr_tot;
    }
}
/********************************************************SYS_BRK HANDLER********************************************************/
void sys_break_handler()
{
    /*time*/
    active_proc->usr_tot += UpdateTime(active_proc->usr_start);
    active_proc->ker_start = TOD_LO;
    
    state_t *old_area = (state_t*) SYSBK_OLDAREA;
    int ret_val = -2;
    /*prendo solo i bit riguardanti la causa dell'eccezione*/
    unsigned int exc_code = ((getCAUSE() >> 2) & 0x1F);
	
    /* se il codice riguarda una system call*/
    if (exc_code == 8)
    {
        unsigned int syscall = old_area->gpr[3];
        switch (syscall)
        {
            case TERMINATEPROCESS:
                ret_val = TerminateProcess((void **) old_area->gpr[4]);
                break;
                
            case VERHOGEN: 
				Verhogen((int *) old_area->gpr[4], 0);
				
				break;
				
			case PASSEREN:
            	Passeren((int *) old_area->gpr[4]);
				break;
				
			case GETCPUTIME:
                GetCpuTime((unsigned int *) old_area->gpr[4], (unsigned int *) old_area->gpr[5], (unsigned int *) old_area->gpr[6]);
				break;
				
			case CREATEPROCESS:
				ret_val = CreateProcess((state_t *) old_area->gpr[4], (int) old_area->gpr[5], (void **) old_area->gpr[6]);
				break;
				
			case WAITCLOCK:
				WaitClock();
				break;
			case WAITIO:
			    WaitIO((unsigned int *) old_area->gpr[5], (unsigned int) old_area->gpr[4]);				
				break;
				
			case SETTUTOR:
			    SetTutor();
				break;
			case SPECPASSUP:
				ret_val = SpecPassup((int) old_area->gpr[4], (state_t *) old_area->gpr[5], (state_t *) old_area->gpr[6]);
				break;
			case GETPID:
			    GetPid((void **)old_area->gpr[4], (void **) old_area->gpr[5]);
			    break;
        }
        if (syscall > 10)
        {
            if (active_proc->handler[0] != NULL)
            {
                cpystate((state_t *) active_proc->old[0], (state_t *) SYSBK_OLDAREA);  
                active_proc->old[0]->pc_epc += WORD_SIZE;  
                active_proc->ker_tot += TOD_LO - active_proc->ker_start;
                LDST(active_proc->handler[0]);
            }
            else 
            {
                TerminateProcess(0);
            }
        }	
        /* ripristino lo stato */

		cpystate(&active_proc->p_s, (state_t *) SYSBK_OLDAREA);
		active_proc->p_s.pc_epc += WORD_SIZE;
		if (ret_val != -2)
		    active_proc->p_s.gpr[1] = ret_val;
		
		active_proc->ker_tot += UpdateTime(active_proc->ker_start);
		LDST(&(active_proc->p_s));
    }
    else
    {
        if (active_proc->handler[0] != NULL && active_proc->old[0] != NULL)
        {
            cpystate((state_t *) active_proc->old[0], (state_t *) SYSBK_OLDAREA);
            active_proc->old[0]->pc_epc += WORD_SIZE; 
            active_proc->ker_tot += UpdateTime(active_proc->ker_start); 
            
            LDST(active_proc->handler[0]);
        }
        else
        {
            TerminateProcess(0);
        }
    }
}
/*****************************************************TRAP HANDLER***********************************************************************/
void prgrm_trap_handler() 
{
    active_proc->usr_tot += UpdateTime(active_proc->usr_start);
    active_proc->ker_start = TOD_LO;
    
    if (active_proc->handler[2] != NULL && active_proc->old[2] != NULL)
    {
        cpystate((state_t *) active_proc->old[2], (state_t *) PGMTRAP_OLDAREA);
        active_proc->old[2]->pc_epc += WORD_SIZE; 
        
        active_proc->ker_tot += UpdateTime(active_proc->ker_start); 
         
        LDST(active_proc->handler[2]);
    }
    else
    {
        TerminateProcess(0);
    }
    PANIC();
}

/********************************************************TLB HANDLER***********************************************************/
void tlb_mngmt_handler() 
{
    active_proc->usr_tot += UpdateTime(active_proc->usr_start);
    active_proc->ker_start = TOD_LO;
    
    if (active_proc->handler[1] != NULL && active_proc->old[1] != NULL)
    {
        cpystate((state_t *) active_proc->old[1], (state_t *) TLB_OLDAREA);  
        active_proc->old[1]->pc_epc += WORD_SIZE;  
        
        active_proc->ker_tot += UpdateTime(active_proc->ker_start); 
        LDST(active_proc->handler[1]);
    }
    else
    {
        TerminateProcess(0);
    }
    PANIC();
}
/*********************************************************INTERRUPT HANDLER***************************************************************/
void Terminal()
{
    unsigned int ret_st;
	unsigned int *pendingdev = INTR_CURRENT_BITMAP(7);
	/*ciclo for sulla bitmap dei pending dev, per ogni bit acceso 
	  faccio le op di cui hanno bisogno*/
	unsigned int zmask = 0;
	unsigned int bitmask;
	unsigned int isPending;
	for (int i = 0; i < 8; i++)
	{
		/*controllo se l' i-esimo bit è acceso*/
		bitmask = (zmask | 1) << i;
		isPending = (*pendingdev & bitmask) >> i;
		
		if (isPending)
		{	
			/*calcolo l'indirizzo del devreg che ha lanciato l'eccezione*/
			termreg_t *dterm =(termreg_t *) (0x10000050 + ((7 - 3) * 0x80) + (i * 0x10));
			if ((dterm->transm_status & 0xFF) == 5)
			{	
				ret_st = dterm->transm_status;
	   			dterm->transm_command = DEV_C_ACK;
	   			Verhogen(&terminal_t[i], ret_st);
	   		}
	   		else if ((dterm->recv_status & 0xFF) == 5)
			{
	   			dterm->recv_command = DEV_C_ACK;
	   		}
		}
			
	}
    /* ripristino lo stato */
    state_t *old_area = (state_t*) 0x20000000;
    active_proc->p_s.entry_hi = old_area->entry_hi;
    active_proc->p_s.cause = old_area->cause;
    active_proc->p_s.status = old_area->status;
    active_proc->p_s.pc_epc = old_area->pc_epc ;
    for (int i = 0; i<31; i++)
    {
        active_proc->p_s.gpr[i] = old_area->gpr[i];
    }
    active_proc->p_s.hi = old_area->hi;        
    active_proc->p_s.lo = old_area->lo;
    active_proc->ker_tot += UpdateTime(active_proc->ker_start); 
    LDST(&(active_proc->p_s)); 
}

void IntervalTimer(state_t *old_area)
{
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
    active_proc->ker_tot += UpdateTime(active_proc->ker_start); 
    /* controllo se qualcuno ha fatto una "WAITCLOCK" */
    if (!emptyProcQ(&wait_queue))
    {
        struct pcb_t *iter;
        struct list_head tmpQ;
        INIT_LIST_HEAD(&tmpQ);
        
        while ((iter = removeProcQ(&wait_queue)) != NULL)
        {
            iter->timer += SCHED_TIME_SLICE;
		    if (iter->timer >= 100000)
		    {
		        insertProcQ(&ready_queue, iter);
		        iter->timer = 0;
		    }
		    else
		    {
		        insertProcQ(&tmpQ, iter);
		    }
        }
        while ((iter =  removeProcQ(&tmpQ)) != NULL)
        {
		    insertProcQ(&wait_queue, iter);
        }
    }
    scheduler(0);
}

void PrintTapeDisk(int line)
{
	unsigned int *pendingdev = INTR_CURRENT_BITMAP(line);
	/*ciclo for sulla bitmap dei pending dev, per ogni bit acceso 
	  faccio le op di cui hanno bisogno*/
	unsigned int zmask = 0;
	unsigned int bitmask;
	unsigned int isPending;
	for (int i = 0; i < 8; i++)
	{
		/*controllo se l' i-esimo bit è acceso*/
		bitmask = (zmask | 1) << i;
		isPending = (*pendingdev & bitmask) >> i;
		
		if (isPending)
		{	
			/*calcolo l'indirizzo del devreg che ha lanciato l'eccezione*/
			dtpreg_t *dev =(dtpreg_t *) (0x10000050 + ((line - 3) * 0x80) + (i * 0x10));
			/*device ready */
		    if (dev->status == 1)
		    {
		        dev->command = DEV_C_ACK;
		        switch (line)
		        {
		            /*Disk*/
		            case 3:
		                Verhogen(&disk[i], 0);
		                break;
		            /*Tape*/
		            case 4:
		                Verhogen(&tape[i], 0);
		                break;
		            /*Printer*/
		            case 6:
		                Verhogen(&printer[i], 0);
		                break;
		        }
		    }
		}	
	}
    /* ripristino lo stato */
    state_t *old_area = (state_t*) 0x20000000;
    active_proc->p_s.entry_hi = old_area->entry_hi;
    active_proc->p_s.cause = old_area->cause;
    active_proc->p_s.status = old_area->status;
    active_proc->p_s.pc_epc = old_area->pc_epc ;
    for (int i = 0; i<31; i++)
    {
        active_proc->p_s.gpr[i] = old_area->gpr[i];
    }
    active_proc->p_s.hi = old_area->hi;        
    active_proc->p_s.lo = old_area->lo;
    active_proc->ker_tot += UpdateTime(active_proc->ker_start); 
    LDST(&(active_proc->p_s)); 
}

void intrpt_handler() 
{
    /*time*/
    active_proc->usr_tot += UpdateTime(active_proc->usr_start);
    active_proc->ker_start = TOD_LO;

    unsigned int cause = getCAUSE() >> 8;

    /*Inter-processor interrupts*/
    if ((cause & 0x1) == 0x1)
    {
    	/* ... */
    }
    /*Processor Local Timer*/
    else if (((cause & 0x2) >> 1) == 0x1)
    {
        /*Non abilitato*/
    }
    /*Disk*/
    else if (((cause & 0x8) >> 3) == 0x1)
    {
        PrintTapeDisk(3);
    }
    /*Interval Timer*/
    else if (((cause & 0x4) >> 2) == 0x1)
    {
        IntervalTimer((state_t *) INT_OLDAREA);
    }
    /*Tape*/
    else if (((cause & 0x10) >> 4) == 0x1)
    {
        PrintTapeDisk(4);
    }
    /*Network Devices*/
    else if (((cause & 0x20) >> 5) == 0x1)
    {
        /* ... */
    }
    /*Printer*/
    else if (((cause & 0x40) >> 6) == 0x1) 
    {
        PrintTapeDisk(6);
    }
    /*Terminal Devices*/
    else if (((cause & 0x80) >> 7) == 0x1)
    {
        Terminal();
    }
    else {
        PANIC();
    }
}


