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
#define SYSCALL_BREAK_NEWAREA 0x200003D4
#define PRGM_TRAP_NEWAREA     0x200002BC
#define TLB_MNGMT_NEWAREA     0x200001A4
#define INTRPT_NEWAREA        0x2000008C
#define FRAMESIZE 0x1000
#define CR 0x0a   /* carriage return as returned by the terminal */

//#define FRAMESIZE 4096

/* New areas */
/*


#define TRANSMITTED 5
#define TRANSTATUS 2
#define ACK 1
#define PRINTCHR 2
#define CHAROFFSET 8
#define STATUSMASK 0xFF
#define TERM0ADDR 0x10000250
#define DEVREGSIZE 16
#define READY 1
#define DEVREGLEN 4
#define TRANCOMMAND 3
#define BUSY 3

#define TOD_LO *((unsigned int *)0x1000001C)
#define TIME_SCALE *((unsigned int *)0x10000024)
#define RAMBASE *((unsigned int *)0x10000000)
#define RAMSIZE *((unsigned int *)0x10000004)
#define RAMTOP (RAMBASE + RAMSIZE)
#

#define SYS3 3
#define STEPS 6
#define GANTT_SIZE 20

#define TIME_SLICE 0xBB8*/





#endif
