#include <listx.h>
#include <const.h>
#include <pcb.h>


struct pcbFree {
    struct pcb_t process; // provo a fare che ha un puntatore a pcb, perche lasciando come era prima creavo un altro pcb_t che però sta gia nell'array quindi si ripete boh
    struct list_head iter;
};

struct list_head pcbFree_h; // per forza

struct pcbFree pcbFree_table[MAXPROC]; // sicuro


void initPcbs(void)
{
    INIT_LIST_HEAD(&pcbFree_h);
    

    int i;
    for(i=0; i<MAXPROC;i++)
	{
		INIT_LIST_HEAD(&pcbFree_table[i].iter); //per sicurezza purpose
        list_add(&pcbFree_table[i].iter,&pcbFree_h); 
	}
    
}

void freePcb(pcb_t *p)
{
	//  Questa la lascio per il futuro, non so perche come faccio a ritornare un qualcosa di errore dato che non torna niente, forse il check lo fa prima btw: Check if oldproc is deletable (no children and no threads)
   /*
   if (!(list_empty(&p->p_child) && list_empty(&p->p_sib)))
      return -1;
   

		*/


   // remove oldproc from sibling list 
   //list_del(&(p->p_next));

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
		INIT_LIST_HEAD(&newProc->process.p_next);
	  	INIT_LIST_HEAD(&newProc->process.p_child);
	  	INIT_LIST_HEAD(&newProc->process.p_sib);

	  	return &newProc->process;	

    }

}

// inizializza la lista dei pcb inizializzando l'elemnento sentinella
void mkEmptyProcQ(struct list_head *head)
{
	INIT_LIST_HEAD(&head); //boh cosi semplice?
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

void insertProcQ(struct list_head *head, pcb_t *p)
{
	//qui ce sta da lavoracce serio, e' tipo da mettere in ordine di importanza i process
}

pcb_t *headProcQ(struct list_head *head)
{

}

pcb_t *removeProcQ(struct list_head *head)
{

}

pcb_t *outProcQ(struct list_head *head, pcb_t *p)
{

}


/* Tree view functions */
int emptyChild(pcb_t *this)
{

}

void insertChild(pcb_t *prnt, pcb_t *p)
{

}

pcb_t *removeChild(pcb_t *p)
{

}

pcb_t *outChild(pcb_t *p)
{

}
//--------------------------------FUNZIONI FILE ASL.H -----------semd_t* getSemd(int *key);
void initASL(){}
int insertBlocked(int *key,pcb_t* p){}
pcb_t* removeBlocked(int *key){}
pcb_t* outBlocked(pcb_t *p){}
pcb_t* headBlocked(int *key){}
void outChildBlocked(pcb_t *p){}




