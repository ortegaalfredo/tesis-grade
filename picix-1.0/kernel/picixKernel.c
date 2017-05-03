/*%%\section{picixKernel.c,Basic scheduler of the picix OS}%%*/
/*  >>> this is file picixKernel.C
        compile it with Microchip C18 3.x
============================================================================
   Picix v1.0 (C) 2002-2006 Alfredo Ortega (ortegaalfredo@gmail.com)

   This program is free software; you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation; either version 2 of the License, or
   (at your option) any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program; if not, write to the Free Software
   Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.

----------------------------------------------------------------------------
   Comments and bug reports are welcome and may be sent to:
   E-Mail:    cachito@ortega.net.ar

============================================================================

   This source contains the basic scheduler of the picix OS. Is very simple.
   V 0.1: (20-12-2002)
   	-Works!
	-Interrupt fixed at TMR1 (for now)
	-Configurable proccess and stack depth
	-HI-TIDE can´t simulate it. Why? (Answer (23-1-2003): Because it sucks, just like MPSIM)
   V 0.5: (5-2-2003)
   	-Really Works!
	-OS_delay works
	-Statistics works
	-Preemptive and cooperative OS
	-Compile it with Microchip C18 v2.10.02
	-Won´t work with HI-TECH PICC18, it uses a compiled stack!
   V 0.6: (1-3-2003)
   	-Assembly optimizations
   	-Context switch Reduced to ~700 Clocks.
   V 0.9: (1-4-2003)
    -A lot of more registers saved
	-Idle function added
*/

#include <p18f452.h>
#include <delays.h>
#include "../drivers/serial/serial.h"
#include "../drivers/serial/console.h"
#include "../util.h"
#include "picix.h"
#include "picixkernel.h"

/* Work variables */
unsigned char *pc1,*pc2,pc,switchs;


/*%%\subsection{void OS_init(void)}%%*/
/*---------------------------------------------------------------------*/
/* Function OS_init:
 	Initializes some variables and the initial thread
   Last modified: 5-2-2003
*/
void OS_init(void)
{
   unsigned char i;
   switchs=0;
   for (i=0;i<MAXPROCESS;i++)
       processStatus[i] = PROCESS_EMPTY;
       /* Here we setup the main() thread */
   actualProcess = 0;
   processPriority[0]=5;
   processCount[0] = 5;
   processStatus[0] = PROCESS_ELIG;
   processName[0] = "main()";
   Sstackl[0]=MAIN_STACK_LOW;
   Sstackh[0]=MAIN_STACK_HIGH;
   sfr[0].tblptrh=sfr[0].tblptrl=sfr[0].tblptru=0;
}

/*%%\subsection{void printk(unsigned char c)}%%*/
/*---------------------------------------------------------------------*/
/* For debugging: Low latency serial output 
   Last modified: 3-1-2003
   */
void printk(unsigned char c)
	{
	 while(!(TXSTA & 0x02)) continue;
        TXREG = c;
	}
	
/*%%\subsection{void printChark(unsigned char c)}%%*/
void printChark(unsigned char c)
{
printk(int2hex(c >> 4));
printk(int2hex(c &  0x0F));
}
void printIntk(unsigned int i)
{
printChark(i>>8);
printChark(i & 0xFF);
}


/*%%\subsection{void save_SFR(void)}%%*/
/*---------------------------------------------------------------------*/
/* Assembly functions to save and restore special function registers.
   Those are useful because C18 don´t optimize functions containing assembler.
   Last modified: 1-3-2003
   */
void save_SFR(void)
{
      // Save Special function registers
    _asm
    movff WREG, wreg_temp
    movff BSR, bsr_temp
    movff STATUS, status_temp
    movff FSR0H, fsr0h_temp
    movff FSR0L, fsr0l_temp
    movff PRODL, prodl_temp
    movff PRODH, prodh_temp
    movff   0  , tmp0_temp
    movff   1  , tmp1_temp
    _endasm
}

/*%%\subsection{void restore_SFR(void)}%%*/
void restore_SFR(void)
{
      // Restore Special function registers
    _asm
    movff  wreg_temp, WREG
    movff   bsr_temp, BSR
    movff status_temp,STATUS
    movff fsr0h_temp,FSR0H
    movff fsr0l_temp,FSR0L
    movff  prodl_temp, PRODL
    movff  prodh_temp, PRODH
    movff  tmp0_temp,0
    movff  tmp1_temp,1
    _endasm
}

/*%%\subsection{void OS_switch(void)}%%*/
/*---------------------------------------------------------------------*/
/* Funcion OS_switch:
	Causes a complete Context switch between processes.
		Pseudocode:
			1.Deactivate interrupts
			2.Save special function registers
			3.Collect statistics about the current process
			4.Save the hardware stack
			5.Calls OS_sched to load the next process
	Generally you will call this function every time when you want a context switch,
	If you put it in a periodic ISR, you get a preemptive OS, but if you call it from 
	a user-space function, you will get a cooperative OS that way.
   Last modified: 1-3-2003
   */
void OS_switch(void)
{
	OS_disable();
    save_SFR();
    pc = actualProcess * sizeof(sfrContext);
    _asm
    lfsr 0 , sfr
    movff pc,WREG
    addwf FSR0L,1,0
    clrf WREG,0
    addwfc FSR0H,1,0
    _endasm
    //Save special registers
    POSTINC0 = wreg_temp;
    POSTINC0 = bsr_temp;
    POSTINC0 = status_temp;
    POSTINC0 = prodl_temp;
    POSTINC0 = prodh_temp;
    POSTINC0 = fsr0h_temp;
    POSTINC0 = fsr0l_temp;

    POSTINC0 = TBLPTRU;
    POSTINC0 = TBLPTRH;
    POSTINC0 = TBLPTRL;
    POSTINC0 = FSR1H;
    POSTINC0 = FSR1L;
    POSTINC0 = FSR2H;
    POSTINC0 = FSR2L;
    POSTINC0 = tmp0_temp;
    POSTINC0 = tmp1_temp;
    POSTINC0 = PCLATH;

	// Collect statistics

	if (STKPTR>maxHstack[actualProcess]) maxHstack[actualProcess]=STKPTR;
	pc =(unsigned int)(((unsigned int)FSR1H)*256+((unsigned int)FSR1L))-
	    (unsigned int)((unsigned int)Sstackh[actualProcess]*256+(unsigned int)Sstackl[actualProcess]);
	if (pc>maxSstack[actualProcess]) maxSstack[actualProcess]=pc;
	if (switchs++==100) //Calcs cpu time per process
		{
		switchs=0;
		for(pc=0;pc<MAXPROCESS;pc++)
			{
			cpuTime[pc]=cpuTime_temp[pc];
			cpuTime_temp[pc]=0;
			}
		}
	
    //Save Hardware Stack
    Hstkptr[actualProcess] = STKPTR;
	pc1 = &HstackH[actualProcess][0];
	pc2 = &HstackL[actualProcess][0];
    if (STKPTR == 0) processStatus[actualProcess] = PROCESS_EMPTY; // The process has finished
    else    while (STKPTR)
    	{
	    pc = STKPTR-1;
		pc1[pc] = TOSH;
		pc2[pc] = TOSL;
	    _asm
	    POP
	    _endasm
		}
OS_sched();
}

/*%%\subsection{void selectNextProcess(void)}%%*/
/*---------------------------------------------------------------------*/
/* Funcion selectNextProcess:
	Chooses and load the next process ready to run on actualProcess.
   Last modified: 16-3-2003
   Fixed "all process in delay" problem.
   */
void selectNextProcess(void)
{
unsigned char i,q;
q=255;
 //Chooses the next process to run
 do	{
	for (i=0;i<MAXPROCESS;i++)
		{
		if ((processCount[i]==0) &&  (processStatus[i] == PROCESS_DELAY)) processStatus[i] = PROCESS_ELIG;
		if ((processCount[i]<=q) && (processStatus[i] == PROCESS_ELIG)) {actualProcess=i;q=processCount[i];}
		if (processCount[i]) processCount[i]--;
		}
	processCount[actualProcess] = processPriority[actualProcess];
	cpuTime_temp[actualProcess]++;
	
	if (processStatus[actualProcess] != PROCESS_ELIG)
		Delay1KTCYx(TICKS_BETWEEN_INTERRUPTS/1000);
		
	} while (processStatus[actualProcess] == PROCESS_DELAY);
}

/*%%\subsection{void OS_sched(void)}%%*/
/*---------------------------------------------------------------------*/
/* Funcion OS_sched:
	Load the context of the next process and returns.
		Pseudocode:
			1.Select the next process to run.
			2.Restore the hardware stack
			3.Restores the  special function registers
			4.Activate interrupts.
			5.returns to the next process
	If OS_sched is called from a user-space function (Like main()) causes the 
	ejecution flow to switch to the next process whitout saving the context!
	This is useful sometimes but generally is bad.
   Last modified: 1-3-2003
   */
void OS_sched(void)
{
static unsigned char i,q;
  selectNextProcess();
  // Restore Hardware stack
  STKPTR=0;
	pc1 = &HstackH[actualProcess][0];
	pc2 = &HstackL[actualProcess][0];
	q=Hstkptr[actualProcess];
  for (i=0;i<q;i++)
    	{
		STKPTR++;
	    pc = STKPTR-1;
		TOSL = pc2[pc];
		TOSH = pc1[pc];
		TOSU = 0;
		}

    // Restore context
    pc = actualProcess * sizeof(sfrContext);
    _asm
    lfsr 0 , sfr
    movff pc,WREG
    addwf FSR0L,1,0
    clrf WREG,0
    addwfc FSR0H,1,0
    _endasm
    //Restore special registers
    wreg_temp = POSTINC0;
    bsr_temp = POSTINC0;
    status_temp = POSTINC0;
    prodl_temp = POSTINC0;
    prodh_temp = POSTINC0;
    fsr0h_temp = POSTINC0;
    fsr0l_temp = POSTINC0;

    TBLPTRU = POSTINC0;
    TBLPTRH = POSTINC0;
    TBLPTRL = POSTINC0;
    FSR1H = POSTINC0;
    FSR1L = POSTINC0;
    FSR2H = POSTINC0;
    FSR2L = POSTINC0;
    tmp0_temp = POSTINC0;
    tmp1_temp = POSTINC0;
    PCLATH = POSTINC0;
    PCLATU = 0;

 	restore_SFR();
	OS_enable();
setTimer();
}	
/*%%\subsection{void OSRpt(void)}%%*/
/*---------------------------------------------------------------------*/
/* Funcion OS_Rpt:
	Prints statistics about the Os.
	It needs a working putch(), and the home-made printChar and printInt
	on serial.c
	So we avoid using printf() that is very expensive at the code.
   Last modified: 5-2-2003
	*/

void OSRpt(void)
{
unsigned char i,q;
unsigned int n;

puts_rom("proc\tname\tprio\tstatus\tHstack\taddr\tSstack\ttime");
for(i=0;i<MAXPROCESS;i++)
	{
	printChar(i);
	putch('\t');
	cputs_rom(processName[i]);
	putch('\t');
	printChar(processPriority[i]);
	putch('\t');
	putch(processStatus[i]);
	if (i==actualProcess) putch('*');
	putch('\t');  //Hstack
	printChar(maxHstack[i]);
	putch('\t');
	n=HstackH[i][q]*256; //addr
	n+=HstackL[i][q];
	printInt(n);
	putch('\t'); //Sstack
	printChar(maxSstack[i]);
	putch('\t'); //Time
	printChar(cpuTime[i]);
	puts_rom("");
	maxHstack[i]=0;
	maxSstack[i]=0;
	}
OS_disable();
for(i=0;i<MAXPROCESS;i++)
	{
	printk(i+0x30);
	printk(':');
	n = Hstkptr[i]+1;
	for(q=0;q<n;q++)
		{
		printChark(HstackH[i][q]);
		printChark(HstackL[i][q]);
		printk(';');
		}
	printk('\n');
	printk('\r');
	}
OS_enable();
}

/*%%\subsection{void OSCreateTask(void (*proc)(void),unsigned char prio,unsigned char *SofStack,const rom unsigned char *name)}%%*/
/*---------------------------------------------------------------------*/
/* Funcion OS_CreateTask:
	Creates a task and fills some info about it.
	Parameters:
		proc: A function pointer to the process
		prio: Priority of the process. 1(max) to 255 (min)
		SofStack: Software Stack for the process. 128 bytes should be enough to everyone :)
		name: Process name, to be used in OS_rpt()
   Last modified: 18-2-2003
	*/
void OSCreateTask(void (*proc)(void),unsigned char prio,unsigned char *SofStack,const rom unsigned char *name)
{
unsigned char *p,i;
p = (unsigned char *) &proc;
for (i=0;i<MAXPROCESS;i++)
	if (processStatus[i] == PROCESS_EMPTY) break;
processPriority[i]=prio;
processCount[i] = prio;
processName[i] = name;
Hstkptr[i] = 1;
HstackL[i][0] = p[0];
HstackH[i][0] = p[1];
processStatus[i] = PROCESS_ELIG;
p = (unsigned char *) &SofStack;
Sstackl[i]=sfr[i].fsr2l=sfr[i].fsr1l=p[0];
Sstackh[i]=sfr[i].fsr2h=sfr[i].fsr1h=p[1];
sfr[i].tblptrh=sfr[i].tblptrl=sfr[i].tblptru=0;
}

/*%%\subsection{void OS_delay(unsigned char ticksL,unsigned char ticksH)}%%*/
/*---------------------------------------------------------------------*/
/* Function OS_delay:
	Delays the calling task the specified 16-bit number of ticks
	you define the clocks per tick on 'picix.h'.
	It really enforces the delay, so weird things can happen when you
	use short time delays.
   Last modified: 5-2-2003
	*/
void OS_delay(unsigned char ticksL,unsigned char ticksH)
{
processStatus[actualProcess] = PROCESS_DELAY;
processCount[actualProcess]=ticksL;
OS_enable();
OS_switch();
while (ticksH--) {
	processCount[actualProcess]=255;
	OS_switch();
	};
}

/*---------------------------------------------------------------------*/
/* Function OS_Replace:
	Replaces the current process with another one, and with another priority.
	It may be useful to use this instead a normal function call, to reuse
	the software stack.
   Last modified: 23-2-2003
	*/

/*%%\subsection{void OS_Replace(void (*proc)(void),unsigned char prio)}%%*/
void OS_Replace(void (*proc)(void),unsigned char prio)
{
processPriority[actualProcess]=prio;
processCount[actualProcess] = prio;
pc1 = (unsigned char *) &proc;
STKPTR = 1;
TOSL = pc1[0];
TOSH = pc1[1];
TOSU = 0;
TBLPTRL=TBLPTRH=TBLPTRU=0;
//sfr[actualProcess].tblptrh=sfr[actualProcess].tblptrl=sfr[actualProcess].tblptru=0;
pc=Sstackl[actualProcess];
//sfr[actualProcess].fsr2l=sfr[actualProcess].fsr1l=pc;
_asm
    movff pc,FSR1L
    movff pc,FSR2L
_endasm
pc=Sstackh[actualProcess];
//sfr[actualProcess].fsr2h=sfr[actualProcess].fsr1h=pc;
_asm
    movff pc,FSR1H
    movff pc,FSR2H
    return 0
_endasm
}

/*%%\subsection{void OS_kill(unsigned char proc,unsigned char st)}%%*/
void OS_kill(unsigned char proc,unsigned char st)
{
processStatus[proc] = st;
}

/*%%\subsection{void OS_renice(unsigned char proc,unsigned char prio)}%%*/
void OS_renice(unsigned char proc,unsigned char prio)
{
processPriority[proc]=prio;
processCount[proc] = prio;
}

/*%%\subsection{void setTimer(void)}%%*/
void setTimer(void)
{
static unsigned char wtmp;
	_asm movff WREG, wtmp _endasm
   TMR1H=TMR1RESET_HIGH;
   TMR1L=TMR1RESET_LOW;
   T1CONbits.TMR1ON=1;      
   PIR1bits.TMR1IF=0;
   PIE1bits.TMR1IE=1;
	_asm movff wtmp, WREG _endasm
}
