/*%%\section{serial.c,Serial utility functions}%%*/
/*  >>> this is file serial.c
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
#include "../../fs/tfs2.h"
#include "../../fs/tfs2_ll.h" //Here we have the serial buffers
#include "serial.h"

#include "../../kernel/picix.h"
/* Serial initialization */ 

/*%%\subsection{void init_comms(void)}%%*/
void init_comms(void)
{
	SPBRG = 128;//DIVIDER;     	
	TXSTA = (SPEED|0x20);
	RCSTA = (0x90);
//	TXSTA = (SPEED|NINE_BITS|0x20);
//	RCSTA = (NINE_BITS|0x90);
	PIE1bits.RCIE=0;
	RCSTAbits.CREN=1;
	
}

/*%%\subsection{unsigned char Kbhit()}%%*/
unsigned char Kbhit()
{
return(PIR1bits.RCIF); // number of chars in buffer
}

/* Buffered fputc */
//21-1-2003
unsigned char fputc_buffer = 0;
unsigned char putc_mutex = 0,getc_mutex = 0;

/*%%\subsection{void fputc(unsigned char fd,unsigned char byte)}%%*/
void fputc(unsigned char fd,unsigned char byte)
{
//while(putc_mutex!=0) continue;
//putc_mutex = 1;
write(fd,&byte,1);
//putc_mutex = 0;
}

/*%%\subsection{unsigned char fgetc(unsigned char fd)}%%*/
unsigned char fgetc(unsigned char fd)
{
unsigned char byte;
read(fd,&byte,1);
return byte;
}


/*%%\subsection{void putch(unsigned char byte)}%%*/
void putch(unsigned char byte) 
{
fputc(0,byte);

	/* output one byte */
/*
while(!(TXSTA & 0x02) ) 
 		OS_delay(1,0);
TXREG = byte;
*/
}

/*%%\subsection{unsigned char getch(void)}%%*/
unsigned char getch(void) {
	/* retrieve one byte */
//return fgetc(STDIN);

while (!PIR1bits.RCIF)
 	 	OS_delay(1,0);
if (RCSTA & 0x06)
	{
	RCSTAbits.CREN=0;
	RCSTAbits.CREN=1;
	return ' ';
	}
if (PIR1bits.RCIF)
	return RCREG;
}

/*%%\subsection{unsigned char getche(void)}%%*/
unsigned char getche(void)
{
	unsigned char c;
	c = getch();
	if (c>31) putch(c);
	return c;
}

/*%%\subsection{void fputs_rom(unsigned char fd,const rom unsigned char * s)}%%*/
void fputs_rom(unsigned char fd,const rom unsigned char * s)
{
	while(*s) 
		fputc(fd,*s++);
}

/*%%\subsection{void fputs(unsigned char fd,const unsigned char * s)}%%*/
void fputs(unsigned char fd,const unsigned char * s)
{
	while(*s) 
		fputc(fd,*s++);
}
