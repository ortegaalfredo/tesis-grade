/*%%\section{console.c,Serial console utility functions}%%*/
/*  >>> this is file console.c
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

#include "serial.h"
#include "../../fs/tfs2.h"

/*%%\subsection{unsigned char *cgets(unsigned char *s)}%%*/
unsigned char *
cgets(unsigned char *s)
{
unsigned char i;
for (i=0;(s[i]=getche()) != '\n';i++);
s[i]=0;
return s;
}

/*%%\subsection{void cputs(const unsigned char *s)}%%*/
void cputs(const unsigned char * s)
{
	while(*s)
		putch(*s++);
}

/*%%\subsection{void puts(const unsigned char *s)}%%*/
void puts(const unsigned char * s)
{
	cputs(s);
	putch('\n');
	putch('\r');
}

/*%%\subsection{void cputs_rom(const rom unsigned char * s)}%%*/
void cputs_rom(const rom unsigned char * s)
{
/*unsigned char byte;
	while(*s)
		{
		byte = *s++;
		write(STDOUT,&byte,1);
		}*/
	while(*s)
		putch(*s++);
}

/*%%\subsection{void puts_rom(const rom unsigned char * s)}%%*/
void puts_rom(const rom unsigned char * s)
{
unsigned char byte;
/*	while(*s)
		{
		byte = *s++;
		write(STDOUT,&byte,1);
		}
byte = '\n';
write(STDOUT,&byte,1);
byte = '\r';
write(STDOUT,&byte,1);
*/
	cputs_rom(s);
	putch('\n');
	putch('\r');
}
