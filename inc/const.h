#ifndef _CONST_H
#define _CONST_H

/**************************************************************************** 
 *
 * This header file contains the global constant & macro definitions.
 * 
 ****************************************************************************/

/* Maxi number of overall (eg, system, daemons, user) concurrent processes */
#define MAXPROC 20

#define UPROCMAX 3  /* number of usermode processes (not including master proc
		       and system daemons */

#define	HIDDEN static
#define	TRUE 	1
#define	FALSE	0
#define ON 	1
#define OFF 	0
#define EOS '\0'

#define DEV_PER_INT 8 /* Maximum number of devices per interrupt line */

#define NULL ((void *)0)

#define CR 0x0a   /* carriage return as returned by the terminal */

/* New areas */
#define SYSCALL_BREAK_NEWAREA 0x200003D4
#define PRGM_TRAP_NEWAREA     0x200002BC
#define TLB_MNGMT_NEWAREA     0x200001A4
#define INTRPT_NEWAREA        0x2000008C

/* RAM */
#define RAM_BASE 0x10000000
#define RAM_SIZE 0x10000004 
#define RAMTOP RAM_SIZE + RAM_BASE





#endif
