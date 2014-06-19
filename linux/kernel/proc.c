
/*++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
                               proc.c
++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
                                                    Forrest Yu, 2005
++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/

#include "type.h"
#include "const.h"
#include "protect.h"
#include "proto.h"
#include "string.h"
#include "proc.h"
#include "global.h"

/*======================================================================*
                              schedule
 *======================================================================*/
PUBLIC void schedule()
{
	PROCESS* p;
/*	int	 greatest_ticks = 0;

	while (!greatest_ticks) {
		for (p = proc_table; p < proc_table+NR_TASKS; p++) {// TODO TO NEXT,find first; and if is 0, ignore it
			if (p->ticks > greatest_ticks) {// find the lagest ticks
				greatest_ticks = p->ticks;
				p_proc_ready = p;
			}
		}

		if (!greatest_ticks) {		// reset the delay ticks 
			for (p = proc_table; p < proc_table+NR_TASKS; p++) {
				p->ticks = p->priority;
			}
		}

	}
*/


	for (p = proc_table; p < proc_table+NR_TASKS; p++) {// TODO TO NEXT
		if(p->isWait){
			continue;
		}
		if(p->ticks){
			p->ticks--;
		}
	}

	p_proc_ready++;
	while(1){
		if(p_proc_ready->isWait){
			p_proc_ready++;
		}
		if (p_proc_ready >= proc_table + NR_TASKS) {
			p_proc_ready = proc_table;
		}
		if(p_proc_ready->ticks){
			p_proc_ready++;
		} else {
			break;
		}
	}

}

/*======================================================================*
                           sys_get_ticks
 *======================================================================*/
PUBLIC int sys_get_ticks()
{
	return ticks;
}

/*======================================================================*
                           sys_process_sleep
 *======================================================================*/
PUBLIC void sys_process_sleep(int i)
{
	p_proc_ready->ticks = i*HZ/1000;
}

/*======================================================================*
                           sys_disp_str
 *======================================================================*/
PUBLIC void sys_disp_str(char* str)
{
	disp_str(str);
}
/*======================================================================*
                           sys_sem_p
 *======================================================================*/
PUBLIC int sys_sem_p(int ID)
{
	if(ID == MUTEX){
		return --mutex;
	} else if(ID == FULL){
		return --full;
	} else if(ID == EMPTY){
		return --empty;
	} else{
		return -1;
	}
}
/*======================================================================*
                           sys_sem_v
 *======================================================================*/
PUBLIC int sys_sem_v(int ID)
{
	if(ID == MUTEX){
		return ++mutex;
	} else if(ID == FULL){
		return ++full;
	} else if(ID == EMPTY){
		return ++empty;
	} else{
		return -1;
	}
}
