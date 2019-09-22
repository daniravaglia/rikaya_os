#include <listx.h>
#include <const.h>
#include <pcb.h>


struct pcbFree {
    struct pcb_t process; 
    struct list_head iter;
};

struct list_head pcbFree_h; // per forza

struct pcbFree pcbFree_table[MAXPROC]; // sicuro


void initPcbs(void)
{
    INIT_LIST_HEAD(&pcbFree_h);
    for(int i=0; i<MAXPROC;i++)
	{
		INIT_LIST_HEAD(&pcbFree_table[i].iter); //per sicurezza purpose
        list_add(&pcbFree_table[i].iter,&pcbFree_h); 
	}
}

void freePcb(pcb_t *p)
{
   struct pcbFree* tmp;
   tmp = container_of(p,struct pcbFree,process);
   list_add(&tmp->iter,&pcbFree_h);
}

pcb_t *allocPcb(void)
{
	 //controllo se la lista libera ha ancora processi disponibili
	  if(list_empty(&pcbFree_h))
	      return NULL;
	  else 
	  {
		//tolgo un elemento dalla lista libera
		struct list_head *tmp = pcbFree_h.next;
		list_del(tmp);
		//creo nuovo processo
		struct pcbFree* newProc;
		newProc = container_of(tmp,struct pcbFree,iter);
		//inizializzo i campi, per adesso solo questi poi vedo
		INIT_LIST_HEAD(&(newProc->process.p_next));
	  	INIT_LIST_HEAD(&(newProc->process.p_child));
	  	INIT_LIST_HEAD(&(newProc->process.p_sib));
	  	newProc->process.priority = 0;

	  	return &(newProc->process);	
    }
}

// inizializza la lista dei pcb inizializzando l'elemnento sentinella
void mkEmptyProcQ(struct list_head *head)
{
	INIT_LIST_HEAD(head); //boh cosi semplice?
}

// restituisce true se la lista puntata da head e' vuota, false altrimenti
int emptyProcQ(struct list_head *head)
{
	if (list_empty(head)) //non sono sicurissimpo, perche io metterei &head ma non funziona TODO da rivedere
	{
		return 1;
	}
	return 0;
}
/*
DESCRIZIONE: inserisce l’elemento puntato da p
nella coda dei processi puntata da head.
L’inserimento deve avvenire tenendo conto della
priorita’ di ciascun pcb (campo p->priority). La coda
dei processi deve essere ordinata in base alla priorita’
dei PCB, in ordine decrescente (i.e. l’elemento di
testa è l’elemento con la priorita’ più alta).
*/
void insertProcQ(struct list_head *head, pcb_t *p)
{
	if(list_empty(head))
 	{
  		list_add(&(p->p_next),head);
 	}
	else
	{
		struct list_head *testa = head->next;
		struct pcb_t* current_pcb = container_of(testa,struct pcb_t,p_next);
	    while (testa != head)
	    {
	        if (current_pcb->priority < p->priority)
	        {   
	            list_add(&(p->p_next), testa->prev);
	            break;
	        }
	        
	        if (testa->next == head)
	        {
	            list_add(&(p->p_next), testa);
	            break;
	        }
	        testa = testa->next;
	        current_pcb = container_of(testa,struct pcb_t,p_next);
	    }
    }
}
//DESCRIZIONE: Restituisce l’elemento di testa della
//coda dei processi da head, SENZA RIMUOVERLO.
//Ritorna NULL se la coda non ha elementi.

pcb_t *headProcQ(struct list_head *head)
{
	if(list_empty(head))
	{
		return NULL;
	}
	return container_of(head->next,struct pcb_t, p_next);	
}

/*DESCRIZIONE: rimuove il primo elemento dalla
coda dei processi puntata da head. Ritorna NULL
se la coda è vuota. Altrimenti ritorna il puntatore
all’elemento rimosso dalla lista*/

pcb_t *removeProcQ(struct list_head *head)
{
	if(list_empty(head))
	{
		return NULL;
	}
	struct pcb_t *tmp = container_of(head->next,struct pcb_t, p_next);
	list_del(head->next);
	return tmp;
}
/*
DESCRIZIONE: Rimuove il PCB puntato da p dalla
coda dei processi puntata da head. Se p non è
presente nella coda, restituisce NULL. (NOTA: p
può trovarsi in una posizione arbitraria della coda).
*/
pcb_t *outProcQ(struct list_head *head, pcb_t *p)
{
	struct pcb_t *tmp = container_of(head->next,struct pcb_t, p_next);
	struct list_head *testa = head->next;
	while(testa != head)
	{
		if(tmp == p)
		{
			list_del(testa);
			return p;
		}
		testa = testa->next;
		tmp = container_of(testa,struct pcb_t, p_next);		
	}
	return NULL;
}


/* Tree view functions */

/*DESCRIZIONE: restituisce TRUE se il PCB
puntato da p non ha figli, restituisce FALSE
altrimenti.*/
int emptyChild(pcb_t *this)
{
	if(list_empty(&(this->p_child)))
		return 1;
	
	return 0;
}

/*DESCRIZIONE: Inserisce il PCB puntato da p
come figlio del PCB puntato da prnt.*/
void insertChild(pcb_t *prnt, pcb_t *p)
{
	if(list_empty(&(prnt->p_child)))
	{
		prnt->p_child.next = &(p->p_sib);
		prnt->p_child.prev = &(p->p_sib);
	}

	else
	{
		list_add_tail(&(p->p_sib),(prnt->p_child.next));
    }
	p->p_parent = prnt;
}

/*DESCRIZIONE. Rimuove il primo figlio del PCB
puntato da p. Se p non ha figli, restituisce NULL.*/
pcb_t *removeChild(pcb_t *p)
{

	if(list_empty(&(p->p_child)))
		return NULL;
		
    pcb_t *tmp = container_of(p->p_child.next, struct pcb_t, p_sib);
    
    if (list_empty(&(tmp->p_sib)))
    {
        INIT_LIST_HEAD(&(p->p_child));
    }
    else 
    {
        p->p_child.next = tmp->p_sib.next;
        p->p_child.prev = tmp->p_sib.next;
        list_del(&(tmp->p_sib));
    }
	
	INIT_LIST_HEAD(&(tmp->p_sib));
	tmp->p_parent = NULL;
	return tmp;
}


/*DESCRIZIONE: Rimuove il PCB puntato
da p dalla lista dei figli del padre. Se il
PCB puntato da p non ha un padre,
restituisce NULL. Altrimenti restituisce
l’elemento rimosso (cioè p). A
differenza della removeChild, p può
trovarsi in una posizione arbitraria
(ossia non è necessariamente il primo
figlio del padre).*/
pcb_t *outChild(pcb_t *p)
{
	if(p->p_parent == NULL)
		return NULL;
		
	pcb_t *parent = p->p_parent;
	p->p_parent = NULL;
    
    parent->p_child.next = p->p_sib.next;
    parent->p_child.prev = p->p_sib.next;
    
	list_del(&(p->p_sib));
	
	INIT_LIST_HEAD(&(p->p_sib));
	return p;
}




