/*%%\section{util.c,utility functions (Mostly conversions)}%%*/
/*  >>> this is file util.c
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
   E-Mail:    ortegaalfredo@gmail.com


============================================================================
*/

#include <p18f452.h>
#include "drivers/serial/serial.h"
#include "kernel/picix.h"

/*%%\subsection{unsigned char hex2int(unsigned char c)}%%*/
unsigned char hex2int(unsigned char c)
{
if (c<='9') return (c-'0');
if (c<='F') 
	{
	c-=55;
	return c;
	}
c-=87;
return c;
}

/*%%\subsection{unsigned char int2hex(unsigned char i)}%%*/
unsigned char int2hex(unsigned char i)
{
if (i<=9) return (i+'0');
return(i+('A'-10));
}

/*%%\subsection{void printChar(unsigned char c)}%%*/
void printChar(unsigned char c)
{
putch(int2hex(c >> 4));
putch(int2hex(c &  0x0F));
}

/*%%\subsection{void printInt(unsigned int i)}%%*/
void printInt(unsigned int i)
{
printChar(i>>8);
printChar(i & 0xFF);
}

/*
void stack(unsigned char p)
{
unsigned char pc;
printChark(FSR1H);printChark(FSR1L);
printk('-');
printk(p);
printk('\n');
printk('\r');
}
*/

/*%%\subsection{void writeEEprom(unsigned char dest,unsigned char byte)}%%*/
void writeEEprom(unsigned char dest,unsigned char byte)
{
EECON1bits.EEPGD = 0;
EECON1bits.CFGS = 0;
EEADR=dest;
EEDATA=byte;
EECON1bits.WREN=1;		// enable writes
EECON2=0x55;	// required sequence for EEPROM update
EECON2=0xAA;
EECON1bits.WR=1;
while(EECON1bits.WR)continue;
PIR2bits.EEIF=0;
EECON1bits.WREN=0;
}

/*%%\subsection{unsigned char readEEprom(unsigned char dest)}%%*/
unsigned char readEEprom(unsigned char dest)
{
EECON1bits.EEPGD = 0;
EECON1bits.CFGS = 0;
EEADR=dest;
EECON1bits.RD=1;
return EEDATA;
}
