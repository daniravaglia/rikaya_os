#include <listx.h>
#include <const.h>
#include <asl.h>
#include <pcb.h>


/*DESCRIZIONE: Inizializza la lista dei semdFree in
modo da contenere tutti gli elementi della
semdTable. Questo metodo viene invocato una
volta sola durante l’inizializzazione della struttura
dati.*/
struct semdFree {
    struct semd_t semd; 
    struct list_head iter;
};

struct list_head semdFree_h; 
struct semdFree semd_table[MAXPROC]; 
struct list_head semd_h;

void initASL()
{
    INIT_LIST_HEAD(&semdFree_h);
    INIT_LIST_HEAD(&semd_h);
    
    for (int i = 0; i<MAXPROC; i++)
    {
        INIT_LIST_HEAD(&semd_table[i].iter); 
        INIT_LIST_HEAD(&(semd_table[i].semd.s_procQ));
        INIT_LIST_HEAD(&(semd_table[i].semd.s_next));
        list_add(&semd_table[i].iter,&semdFree_h);    
    }
}

/*DESCRIZIONE: restituisce il puntatore al SEMD nella ASL
la cui chiave è pari a key. Se non esiste un elemento nella
ASL con chiave eguale a key, viene restituito NULL.*/
semd_t* getSemd(int *key)
{
    struct list_head *testa = semd_h.next;        
    while (testa != &semd_h)
    {    
        struct semdFree *tmp = container_of(testa, struct semdFree, iter);
        struct semd_t *tmp2 = &(tmp->semd);

        if ((tmp2->s_key) == key)
        {   
            return tmp2;
        }
        testa = testa->next;
    }
    return NULL;
    
}
/*DESCRIZIONE: Viene inserito il PCB puntato da p nella
coda dei processi bloccati associata al SEMD con chiave
key. Se il semaforo corrispondente non è presente nella
ASL, alloca un nuovo SEMD dalla lista di quelli liberi
(semdFree) e lo inserisce nella ASL, settando I campi in
maniera opportuna (i.e. key e s_procQ). Se non è possibile
allocare un nuovo SEMD perché la lista di quelli liberi è
vuota, restituisce TRUE. In tutti gli altri casi, restituisce
FALSE.*/
int insertBlocked(int *key,pcb_t* p)
{

    struct semd_t *semdKey;
    //non trovo il semaforo, ne devo prendere uno dalla free list
    if ((semdKey = getSemd(key))== NULL)
    {
        //controllo se la free list è vuota
        if (list_empty(&semdFree_h))
            return 1;
        
        struct list_head *new_semd = semdFree_h.next;
        list_del(semdFree_h.next);
        list_add(new_semd, &semd_h);
        struct semdFree *semdFree = container_of(new_semd,struct semdFree, iter);
        struct semd_t *semd = &(semdFree->semd);
        semd->s_key = key;
        p->p_semkey = key;       
        
        insertProcQ(&(semd->s_procQ), p);
     }
     return 0;
}
/*DESCRIZIONE: Ritorna il primo PCB dalla
coda dei processi bloccati (s_ProcQ)
associata al SEMD della ASL con chiave key.
Se tale descrittore non esiste nella ASL,
restituisce NULL. Altrimenti, restituisce
l’elemento rimosso. Se la coda dei processi
bloccati per il semaforo diventa vuota,
rimuove il descrittore corrispondente dalla
ASL e lo inserisce nella coda dei descrittori
liberi (semdFree).*/
pcb_t* removeBlocked(int *key)
{
    struct semd_t *tmp;    
    if ((tmp = getSemd(key))== NULL)
    {   
        return NULL;
    }
    struct pcb_t *proc = removeProcQ(&(tmp->s_procQ));
    //caso in cui il processo tolto dalla coda del semaforo è l'ultimo
    if (list_empty(&(tmp->s_procQ)))
    {
        struct semdFree *master = container_of(tmp, struct semdFree, semd);
        INIT_LIST_HEAD(&(tmp->s_procQ));  
        list_del(&(master->iter));
        list_add(&(master->iter), &semdFree_h);
    }
    return proc;
}

/*DESCRIZIONE: Rimuove il PCB puntato da p dalla
coda del semaforo su cui è bloccato (indicato da p->p_semKey). 
Se il PCB non compare in tale coda,
allora restituisce NULL (condizione di errore).
Altrimenti, restituisce p.*/
pcb_t* outBlocked(pcb_t *p)
{
    struct semd_t *semd;    
    if ((semd = getSemd(p->p_semkey))== NULL)
        return NULL;    
    return outProcQ(&(semd->s_procQ), p);    
}

/*DESCRIZIONE: Restituisce (senza rimuovere) il
puntatore al PCB che si trova in testa alla coda dei
processi associata al SEMD con chiave key. Ritorna
NULL se il SEMD non compare nella ASL oppure
se compare ma la sua coda dei processi è vuota.*/
pcb_t* headBlocked(int *key)
{
    struct semd_t *semd;    
    if ((semd = getSemd(key))== NULL)
        return NULL;
    
    if (list_empty(&(semd->s_procQ)))
        return NULL;
        
    struct pcb_t *p = container_of(semd->s_procQ.next, struct pcb_t, p_next);
    return p;
}

/*DESCRIZIONE: Rimuove il PCB puntato da p dalla
coda del semaforo su cui è bloccato (indicato da p->p_semKey). 
Inoltre, elimina tutti i processi
dell’albero radicato in p (ossia tutti i processi che
hanno come avo p) dalle eventuali code dei
semafori su cui sono bloccati.*/
void outChildBlocked(pcb_t *p)
{
    struct semd_t *semd;    
    if ((semd = getSemd(p->p_semkey))== NULL)
        return;
    
    outProcQ(&(semd->s_procQ), p);
    if (!emptyChild(p))
    {
        struct pcb_t *child = container_of(p->p_child.next, struct pcb_t, p_sib);
        struct list_head *pos;
        
        outChildBlocked(child);
        
        list_for_each(pos, &(child->p_sib))
        {
            struct pcb_t *sib = container_of(pos, struct pcb_t, p_sib);
            outChildBlocked(sib);
        }
    }
    else
        return;
}
