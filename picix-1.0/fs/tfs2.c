/*%%\section{tfs2.c,Implements a tiny file system}%%*/
/*  >>> this is file tfs2.c
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

Implements a tiny file system
History: 14/1/2003 Works!
		 17/1/2003 Implemented stdin/out redirection
TODO: buffer writes to files

*/
#include <p18f452.h>
#include <string.h>
#include "../kernel/picix.h"
#include "../drivers/serial/console.h"
#include "../drivers/serial/serial.h"
#include "../util.h"
#include "tfs2.h"
#include "tfs2_ll.h"

extern unsigned char *param,*parameter[];
extern unsigned char commandLine[];

unsigned int  openFilePointer[MAX_OPEN_FILES]; //Relative to StartBlock
unsigned int  openFileStartBlock[MAX_OPEN_FILES];
unsigned int  openFileCurrentBlock[MAX_OPEN_FILES];

extern void startShell( void );

/*---------------------------------------------------------------------*/
/*%%\subsection{unsigned char getFreeDescriptor(void)}%%*/
unsigned char getFreeDescriptor(void)
{
unsigned char fd;
/* Find a free file descriptor */
for (fd=0;fd<MAX_OPEN_FILES;fd++)
	if (openFileStartBlock[fd]==0x0) break;
return fd;
}

/*---------------------------------------------------------------------*/
/*%%\subsection{unsigned char shr13(unsigned int val)}%%*/
/* Helper function, shift right 13 bits. It saves about 150 bytes of code */
unsigned char shr13(unsigned int val)
{
return (val>>13);
}

/*---------------------------------------------------------------------*/
unsigned int stdin,stdout;
/*%%\subsection{void init_FS()}%%*/
void init_FS() /* Clear file descriptors. To be called at main() */
{
unsigned char i;
for (i=0;i<MAX_OPEN_FILES;i++)	
	openFileStartBlock[i]=0x0;
set_default_stdio_serial();
reset_stdio();
}
/*---------------------------------------------------------------------*/
/*%%\subsection{void reset_stdio(void)}%%*/
/* Set the standard files at stdin and stdout */ 
//20-1-2003
void reset_stdio(void)
{
    openFileStartBlock[STDIN]=stdin; //STDIN
    openFileStartBlock[STDOUT]=stdout; //STDOUT
    openFileCurrentBlock[STDIN]=stdin; //STDIN
    openFileCurrentBlock[STDOUT]=stdout; //STDOUT    
}

/*%%\subsection{void set_stdout_temp(unsigned char fd)}%%*/
void set_stdout_temp(unsigned char fd)
{
openFileStartBlock[STDOUT] = openFileStartBlock[fd];
openFileCurrentBlock[STDOUT] = openFileStartBlock[fd];
close(fd);
}

/*%%\subsection{void set_stdin_temp(unsigned char fd)}%%*/
void set_stdin_temp(unsigned char fd)
{
openFileStartBlock[STDIN] = openFileStartBlock[fd];
openFileCurrentBlock[STDIN] = openFileStartBlock[fd];
close(fd);
}

/*%%\subsection{void set_default_stdio_serial(void)}%%*/
void set_default_stdio_serial(void)
{
stdout=stdin=0x8000;
}

/*%%\subsection{void set_default_stdio_file(unsigned char fd)}%%*/
void set_default_stdio_file(unsigned char fd)
{
stdout=stdin=openFileStartBlock[fd];
}


/*---------------------------------------------------------------------*/
/*%%\subsection{unsigned char close(unsigned char fd)}%%*/
unsigned char close(unsigned char fd)
{
if (openFileStartBlock[fd]==0x0) return CLOSE_ERROR_NOT_FOUND;
openFileStartBlock[fd] = 0x0;
openFilePointer[fd] = 0;
return CLOSE_OK;
}
/*---------------------------------------------------------------------*/
/*%%\subsection{unsigned char read(unsigned char fd,unsigned char *data,unsigned char qty)}%%*/
/* Common read function. Warning: max qty = 253 */
unsigned char read(unsigned char fd,unsigned char *data,unsigned char qty)
{
unsigned int filePunt;
unsigned char q;
if ((fd>MAX_OPEN_FILES) || (openFileStartBlock[fd]==0x0)) return READ_WRONG_FD;
filePunt = openFileCurrentBlock[fd];
if ((filePunt & 0x8000) == 0) // Block Device 
	{
	} else  {// Character device
	q=filePunt>>13;
	LL_read(data,filePunt,qty,q);
	}
return qty;
}

/*---------------------------------------------------------------------*/
/*%%\subsection{unsigned char write(unsigned char fd,const unsigned char *data,unsigned char qty)}%%*/
/* Common write function. Warning: max qty = 64 */
unsigned char write(unsigned char fd,const unsigned char *data,unsigned char qty)
{
unsigned int filePunt;
unsigned char q;
if (openFileStartBlock[fd]==0x0) return READ_WRONG_FD;
filePunt = openFileCurrentBlock[fd];
if ((filePunt & 0x8000) == 0) // Block Device 
	{
	} else { // is a character device, write directly to the low level driver
	q=filePunt>>13;
	LL_write(data,filePunt,qty,q);
	}
return qty;
}
