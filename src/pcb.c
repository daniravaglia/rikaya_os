#include <listx.h>
#include <const.h>
#include <pcb.h>


struct pcbFree {
    struct pcb_t process;
    struct list_head iter;
};

struct list_head pcbFree_h;
struct pcb_t pcbFree_table[MAXPROC];

void initPcbs(void)
{
    /*
    INIT_LIST_HEAD(&pcbFree_h);
    
    int i:
    for(i=0; i<MAXPROC;i++)
	{
        list_add(&pcbFree_table[i].p_next,&pcbFree_h); 
	}
	*/
    
}

void freePcb(pcb_t *p)
{

}

pcb_t *allocPcb(void)
{

}

void mkEmptyProcQ(struct list_head *head)
{

}

int emptyProcQ(struct list_head *head)
{

}

void insertProcQ(struct list_head *head, pcb_t *p)
{

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




