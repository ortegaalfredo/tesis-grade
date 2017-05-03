/*%%\section{tfs2_ll.c,Low Level drivers for tfs2}%%*/
/*  >>> this is file tfs2_ll.c
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
#include <string.h>
#include "../kernel/picix.h"
#include "../ip/ip.h"
#include "tfs2.h"
#include "tfs2_ll.h"

void flashWrite(const  unsigned char *data,unsigned int dst,unsigned char len);
void flashRead(unsigned char *data,unsigned int dst,unsigned char len);
void flashInfo (unsigned int *max_blocks,unsigned char *about);

void eepromWrite(const  unsigned char *data,unsigned int dst,unsigned char len);
void eepromRead(unsigned char *data,unsigned int dst,unsigned char len);
void eepromInfo (unsigned int *max_blocks,unsigned char *about);

void extWrite(const  unsigned char *data,unsigned int dst,unsigned char len);
void extRead(unsigned char *data,unsigned int dst,unsigned char len);
void extInfo (unsigned int *max_blocks,unsigned char *about);

void dummyWrite(const  unsigned char *data,unsigned int dst,unsigned char len);
void dummyRead(unsigned char *data,unsigned int dst,unsigned char len);
void dummyInfo (unsigned int *max_blocks,unsigned char *about);

void serialWrite(const  unsigned char *data,unsigned int dst,unsigned char len);
void serialRead(unsigned char *data,unsigned int dst,unsigned char len);
void serialInfo (unsigned int *max_blocks,unsigned char *about);

/* This structure holds the pointers to the read and write operations for all File systems 
   kind of a low-level driver */
typedef struct
	{
	void (*read) (unsigned char *buf,unsigned int dst,unsigned char len);
	void (*write)(const  unsigned char *data,unsigned int dst,unsigned char len);
	void (*info) (unsigned int *max_blocks,unsigned char *about);
	} FS_OP;


/*%%\subsection{Device drivers structure}%%*/
/* this configuration:
Flash  address: 0x0xxx
Eeprom address: 0x2xxx
Null   address: 0x4xxx or 0x6xxx
Serial address: 0x8xxx
(You can use those address with 'mknod')
*/
const rom FS_OP fileOp[] =  // File system low-level drivers
{
// 0-3: Block Devices
	{ flashRead ,flashWrite ,flashInfo }, // type 0: Flash operations
	{ eepromRead,eepromWrite ,eepromInfo }, // type 1: Eeprom operations
	{ extRead,extWrite ,extInfo }, // type 2: Nothing yet!
	{ dummyRead,dummyWrite ,dummyInfo }, // type 3: Nothing yet!
// 4-7: Character Devices
	{ serialRead,serialWrite ,serialInfo }, // type 4: Serial I/O
//	{ TCPRead,TCPWrite ,TCPInfo }  // type 5: Telnet I/O
};

/* The next code set the low-level file system driver, it
   takes a lot of space (why??), so we do it here and call from everywere */

// these are the pointers


/*%%\subsection{void LL_read  (unsigned char *buf,unsigned int dst,unsigned char len,unsigned char fsType)}%%*/
/* We never call the pointers directly, it takes a LOT of code to do so */
void LL_read  (unsigned char *buf,unsigned int dst,unsigned char len,unsigned char fsType)
{
void (*read) (unsigned char *buf,unsigned int dst,unsigned char len);
if (!((fsType == 5) || (fsType == 4)))
	{
	fsType=3;
	printk('!');
	}
TBLPTRU=0;PCLATU=0;
fileOp[fsType].read(buf,dst,len);
}
	
/*%%\subsection{void LL_write (const  unsigned char *data,unsigned int dst,unsigned char len,unsigned char fsType)}%%*/
void LL_write (const  unsigned char *data,unsigned int dst,unsigned char len,unsigned char fsType)
{ 
if (!((fsType == 5) || (fsType == 4)))
	{
	fsType=3;
	printk('!');
	}
TBLPTRU=0;PCLATU=0;
fileOp[fsType].write(data,dst,len); 
}

/*%%\subsection{void LL_info  (unsigned int *max_blocks,unsigned char *about,unsigned char fsType)}%%*/
void LL_info  (unsigned int *max_blocks,unsigned char *about,unsigned char fsType)
{ 
if (fsType!=5)
	{
	fsType=3;
	printk('!');
	}
TBLPTRU=0;PCLATU=0;
fileOp[fsType].info(max_blocks,about); 
}

/*%%\subsection{Eeprom driver}%%*/
/***********************************
Low level Driver for on-board eeprom 
************************************/
/*%%\subsubsection{void eepromWrite(const  unsigned char *data,unsigned int dst,unsigned char len)}%%*/
void eepromWrite(const  unsigned char *data,unsigned int dst,unsigned char len)
{
/*
unsigned char addr,i;	// in PIC18Fxx2 devices, the data needs
INTCONbits.GIE = 0;
INTCONbits.PEIE = 0;
dst+=EEPROM_START;
addr = dst & 0xff;

    EECON1bits.EEPGD = 0;
    EECON1bits.CFGS = 0;

for (i=0;i<len;i++)
	{
	EEADR=addr++;
	EEDATA=data[i];
	EECON1bits.WREN=1;		// enable writes
	EECON2=0x55;	// required sequence for EEPROM update
	EECON2=0xAA;
	EECON1bits.WR=1;
	while(EECON1bits.WR)continue;
	PIR2bits.EEIF=0;
	EECON1bits.WREN=0;
	}
INTCONbits.GIE = 1;
INTCONbits.PEIE = 1;
*/
}

/*%%\subsubsection{void eepromRead(unsigned char *buf,unsigned int dst,unsigned char len)}%%*/
/* eeprom read */
void eepromRead(unsigned char *buf,unsigned int dst,unsigned char len)
{
/*
unsigned char addr,i;	// in PIC18Fxx2 devices, the data needs
INTCONbits.GIE = 0;
INTCONbits.PEIE = 0;

dst+=EEPROM_START;
addr = dst & 0xff;
EECON1bits.EEPGD = 0;
EECON1bits.CFGS = 0;
for (i=0;i<len;i++)
	{
	EEADR=addr++;
	EECON1bits.RD=1;
	buf[i]=EEDATA;
    }

INTCONbits.GIE = 1;
INTCONbits.PEIE = 1;
*/
}

/*%%\subsubsection{void eepromInfo (unsigned int *fs_size_bytes,unsigned char *about)}%%*/
void eepromInfo (unsigned int *fs_size_bytes,unsigned char *about)
{
strcpypgm2ram(about,EEPROM_DEVICE);
*fs_size_bytes = EEPROM_SIZE;
}

/*%%\subsection{Flash driver}%%*/
/***********************************
Low level Driver for Flash memory
************************************/

/*%%\subsubsection{void flashRead(unsigned char *buf,unsigned int dst,unsigned char len)}%%*/
void flashRead(unsigned char *buf,unsigned int dst,unsigned char len)
{
/*
unsigned char i;
//INTCONbits.GIE = 0;
//INTCONbits.PEIE = 0;
dst+=FLASH_START;
TBLPTRL=((dst)&0xFF);
TBLPTRH=(((dst)>>8)&0xFF);
TBLPTRU=0;	// TBLPTRU=(((addr)>>8)>>8);
for (i=0;i<len;i++)
	{
	_asm
	tblrdpostinc
	_endasm
	buf[i]=TABLAT;
	}
//INTCONbits.GIE = 1;
//INTCONbits.PEIE = 1;
*/
}

/*%%\subsubsection{void flashWrite(const  unsigned char *data,unsigned int dst,unsigned char len)}%%*/
/* Flash erase+write WARNING: This erase the 64 bytes block containing dst*/
void flashWrite(const  unsigned char *data,unsigned int dst,unsigned char len)
{
/*
unsigned char index,offset,*p;
dst+=FLASH_START;
p=data;

//INTCONbits.GIE = 0;
//INTCONbits.PEIE = 0;

//FLASH_ERASE
TBLPTRL=((dst)&0xFF);
TBLPTRH=(((dst)>>8)&0xFF);
TBLPTRU=0;	// TBLPTRU=(((addr)>>8)>>8);
EECON1|=0x94;
EECON1&=0xBF;
EECON2=0x55;
EECON2=0xAA;
EECON1bits.WR=1;		// enable writes
_asm
NOP
_endasm
//FLASH_WRITE
dst--;
TBLPTRL=((dst)&0xFF);
TBLPTRH=(((dst)>>8)&0xFF);
TBLPTRU=0;	// TBLPTRU=(((addr)>>8)>>8);
		
		offset=0;
		while(len--)
		{
			
			TABLAT=*p++;	// copy the data buffer to the
			_asm 
			tblwtpreinc
			_endasm
			if(++offset==8)	// after every 8th byte, the
			{			// the write buffer is written to flash.
				EECON1bits.EEPGD=1;
				EECON1bits.WREN=1;		// this is the required sequence
				EECON2=0x55;
				EECON2=0xAA;
				EECON1bits.WR=1;
				_asm
				NOP
				NOP
				NOP
				NOP
				_endasm
				EECON1bits.WREN=0;
				offset=0;
			}
		}
//INTCONbits.GIE = 1;
//INTCONbits.PEIE = 1;
*/
}

/*%%\subsubsection{void flashInfo (unsigned int *fs_size_bytes,unsigned char *about)}%%*/
void flashInfo (unsigned int *fs_size_bytes,unsigned char *about)
{
strcpypgm2ram(about,FLASH_DEVICE);
*fs_size_bytes = FLASH_SIZE;
}

/***********************************
Dummy driver (do nothing)
************************************/


/*%%\subsection{Dummy driver (Ej. /dev/null)}%%*/
void dummyRead(unsigned char *buf,unsigned int dst,unsigned char len){}
void dummyWrite(const  unsigned char *data,unsigned int dst,unsigned char len){}
void dummyInfo (unsigned int *fs_size_bytes,unsigned char *about){*fs_size_bytes=0;strcpypgm2ram(about,DUMMY_DEVICE);}


/*%%\subsection{External EEPROM driver}%%*/
/***********************************
Low level Driver for External EEPROM
************************************/

//unsigned char EEPageWrite2( unsigned char control, unsigned char addressh,unsigned char addressl, unsigned char *wrptr );
//unsigned char EESequentialRead2( unsigned char control, unsigned char addressh,unsigned char addressl, unsigned char *rdptr, unsigned char length );

void extRead(unsigned char *buf,unsigned int dst,unsigned char len)
{
/*
unsigned char ah,al;
ah = (dst>>8) & 0xff;
al = dst & 0xff;
EESequentialRead2( 0xA0, ah, al, buf, len);
*/
}

/* Eeprom write WARNING: This works in 64 bytes blocks! (ignore len)*/
void extWrite(const  unsigned char *data,unsigned int dst,unsigned char len)
{
/*
unsigned char ah,al;
ah = (dst>>8) & 0xff;
al = dst & 0xff;
EEPageWrite2( 0xA0, ah,al, data );
*/

}

void extInfo (unsigned int *fs_size_bytes,unsigned char *about)
	{
	*fs_size_bytes=EXT_SIZE;
	strcpypgm2ram(about,EXT_DEVICE);
	}

/*%%\subsection{Serial driver}%%*/
/***********************************
Low level Driver for Serial I/O
************************************/

/*%%\subsubsection{void serialWrite(const  unsigned char *data,unsigned int dst,unsigned char len)}%%*/
/* serial write: Outputs characters to the circular output buffer */
void serialWrite(const  unsigned char *data,unsigned int dst,unsigned char len)
{
unsigned char i;
unsigned int stdoutO;
stdoutO=stdout;
for (i=0;i<len;i++)
	{
	 while((!(TXSTA & 0x02)) && (stdoutO==stdout)) 
	 		OS_delay(1,0);
        TXREG = *data;
     data++;
    }
}

/*%%\subsubsection{void serialRead(unsigned char *buf,unsigned int dst,unsigned char len)}%%*/
void serialRead(unsigned char *buf,unsigned int dst,unsigned char len)
{
unsigned char i;
unsigned int stdinO;
stdinO=stdin;
for (i=0;i<len;i++)
	{
	while( (PIR1bits.RCIF == 0) && (stdinO == stdin) )
	 		OS_delay(1,0);
	if (RCSTA & 0x06)
		{
		RCSTAbits.CREN=0;
		RCSTAbits.CREN=1;
		i--;
		continue;
		}
	buf[i] = RCREG;
	}
}

/*%%\subsubsection{void serialInfo (unsigned int *fs_size_bytes,unsigned char *about)}%%*/
void serialInfo (unsigned int *fs_size_bytes,unsigned char *about)
{
strcpypgm2ram(about,SERIAL_DEVICE);
*fs_size_bytes = 1;
}

