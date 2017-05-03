/*%%\section{main.c,Main source file, OS setup and ISR}%%*/
/*  >>> this is file main.c
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

*/
#include <p18f452.h>
#include <i2c.h>
#include "kernel/picix.h"
#include "ip/picix-uip.h"
#include "drivers/serial/console.h"
#include "drivers/serial/serial.h"
#include "fs/tfs2.h"
#include "fs/tfs2_ll.h"
#include "cshell.h"
#include "firewall.h"
#include "util.h"

/* Timer-related Defines */
#define INTERRUPT_OVERHEAD            19 /* 19?? */
#define TMR1RESET (0xFFFF-(TICKS_BETWEEN_INTERRUPTS-INTERRUPT_OVERHEAD))
#define TMR1RESET_HIGH TMR1RESET >> 8
#define TMR1RESET_LOW TMR1RESET & 0xFF


/*%%\subsection{Device configuration}%%*/
#pragma romdata CONFIG
rom const unsigned int config_word1 = 0xFEFF;
rom const unsigned int config_word2 = 0xFEFF;
rom const unsigned int config_word3 = 0xFFFF;
rom const unsigned int config_word4 = 0xFFFF;
rom const unsigned int config_word5 = 0xFFFF;
rom const unsigned int config_word6 = 0xFFFF;
rom const unsigned int config_word7 = 0xFFFF;


//---------------------------------------------------------------------
// Interrupt Code
//---------------------------------------------------------------------

void ISR(void);

// Locate ISR handler code at interrupt vector

//#pragma code isrcode=0x0208 // Sera por Jolt?
#pragma code isrcode=0x08

/*%%\subsection{void isrhandler(void)}%%*/
void isrhandler(void)        // This function directs execution to the
{                            //   actual interrupt code
_asm
goto ISR
_endasm
}

#pragma code
void printk(unsigned char c);
/*%%\subsection{void ISR(void)}%%*/
void ISR(void)
{
  if (PIR1bits.TMR1IF)
    {                                   //check for TMR0 overflow
    PIR1bits.TMR1IF = 0;                //clear interrupt flag
   	ClrWdt()
   	OS_switch();
    }
}

/*%%\subsection{void Init(void)}%%*/
void Init(void)
	{
	TRISA = 0xff;
	TRISB = 0xff;
	TRISC = 0xff;
	TRISD = 0xff;
	ADCON1=0x6;
   	init_comms();
	init_FS();
    OS_init();
	}

extern void version(void);
extern const rom unsigned char *CLRSCR_STRING;

/*%%\subsection{void main(void)}%%*/
//void nic(void);
unsigned char proc1Stack[100];
//unsigned char proc2Stack[140];
extern unsigned char putc_mutex,getc_mutex,buf_mutex;
void main( void )
{
  unsigned char i;
  long lCont;
  Init();
//  writeEEprom(0xff,0x25); //For the Jolt bootloader
  OSCreateTask(version,10,proc1Stack,"shell()");
  T1CONbits.TMR1CS=0;
  T1CONbits.T1CKPS0=0;
  T1CONbits.T1CKPS1=0;
  setTimer();
  putc_mutex = getc_mutex = 0;
  OS_enable();


  lCont=0;
  while (1)
   	{ 
	// This is the main thread :
	if (lCont++==100000) 
		lCont=0;
	};
}
