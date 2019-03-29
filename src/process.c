#include <listx.h>
#include <const.h>
#include <pcb.h>


struct pcbFree {
    struct pcb_t process;
    struct list_head iter;
}

static struct list_head pcbFree_h;
static struct pcb_t pcbFree_table[MAXPROC];

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
