/*%%\section{cshell.c,Implements a tiny unix-like Shell as a task in picix, with serial I/O}%%*/
/*  >>> this is file cshell.c
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
#include <string.h>
#include "kernel/picix.h"
#include "drivers/serial/console.h"
#include "drivers/serial/serial.h"
#include "drivers/nic/nic.h"
#include "fs/tfs2.h"
#include "util.h"

void ayuda(void);
void version(void);
void ps(void);
void tris(void);
void port(void);
void vi(void);
void cat(void);
void rm(void);
void mkdir(void);
void mknod(void);
void cd(void);
void dumpfs(void);
void kill(void);
void renice(void);

void saveconf(void);

//Clear Screen, default Color.
const rom unsigned char *CLRSCR_STRING="\x1bm\x1b[0m\x1b[2J";
// For stdio redirection (>,<)
extern unsigned int  openFilePointer[MAX_OPEN_FILES]; //Relative to StartBlock

typedef struct 
	{
	const rom unsigned char *Nombre;
	void (*proc)(void);
	const rom unsigned char *Ayuda;
	} TCOMMAND;

#define NCOMANDOS 8
#define HELP_MSG "Comandos disponibles:"
const rom TCOMMAND comando[] =
{ {"man",ayuda,"Imprime la ayuda"},
  {"ver",version,"Imprime la version"},	
  {"ps",ps,"Imprime los procesos"},
  {"kill",kill," <proc> <st> Modifica el estado de un proceso"},
  {"renice",renice," <proc> <prio> Modifica la prioridad de un proceso"},

  {"tris",tris,"X [val] Setea direccion I/O de puertos tri-state"},
  {"port",port,"X [val] Setea valor de puertos tri-state"},
  {"saveconf",saveconf,": Guarda la configuración actual"}
  
};



#define BUILD "720"
#define VERSION "0.9"
#define DATE "25/3/2003"

#define MAX_PARAMETER 32

unsigned char *parameter[20],*param; //List of parameters and first parameter
unsigned char commandLine[MAX_PARAMETER];

/*%%\subsection{void cursor(unsigned char q,unsigned char *s)}%%*/



/*%%\subsection{void startShell( void )}%%*/
void startShell( void ) 
{
    unsigned char i,tmp,*stdin,*stdout,cmdName[15];
    unsigned char itmp;

    reset_stdio();fputc_buffer=0;
    for (;;)	{
		for (i=0;i<32;i++) commandLine[i]=0;
		for (i=1;i<6;i++) parameter[i]=&commandLine[31];
		parameter[0]=commandLine;
		
		for (i=0;i<32;i++)
			{
			tmp = getche();
			if ((tmp == '\n') || (tmp == '\r')) {commandLine[i]=0;break;} else commandLine[i]=tmp;
			}

			
		puts_rom("");
		if (commandLine[0]!=0) 
			{
			/* split str in multiple parameters, separated by ' ' */
			itmp=0;i=0;
			do	{
				if(commandLine[i]==' ')
					{
					commandLine[i]=0;
					itmp++;
					parameter[itmp]=&commandLine[i+1];
					}
				i++;
				} while ((commandLine[i]!=0) || (i==32)) ;
			i=0;
			param = parameter[1];
			/* Checks for input/output redirections '>' and '<' */
			stdin=NULL;stdout=NULL;
			tmp=itmp;
			for (itmp=1;itmp<=tmp;itmp++)
				{
				if (parameter[itmp][0]=='>') stdout=&parameter[itmp][1];
				if (parameter[itmp][0]=='<') stdin=&parameter[itmp][1];
				}
			i=0;
			while(i<NCOMANDOS)
				{
				strcpypgm2ram(cmdName,comando[i].Nombre);
				if (strcmp(parameter[0],cmdName)==0)
					{
					if (stdout!=NULL)
						{
						set_stdout_temp(tmp);
						}
					if (stdin!=NULL)
						{
						set_stdin_temp(tmp);
						}
					break;
					}
				i++;
				}
			if (i<NCOMANDOS)
					{
					fputc_buffer=FPUTC_MAX_BUFFER;
					/* OS_Replace gives us a new stack, so we have more memory than
					calling proc() directly */
					OS_Replace(comando[i].proc,10);
					fputc_buffer=0;
					}
			else	{
					cputs(commandLine);
					puts_rom("?");
					}
			}
		}
}

extern unsigned char idlePorcent,idleCount;
/*%%\subsection{void ps(void)}%%*/
void ps(void)
{
OSRpt();
OS_Replace(startShell,10);
}

/*%%\subsection{void version(void)}%%*/
void version(void)
{
	puts_rom("                                           __");
	puts_rom("picix OS V 1.0                          \\_/(o\\ ");
	puts_rom("Copyright (C) 2003 Alfredo A. Ortega    / \\__/ ");  
	puts_rom("Universidad Nacional de la Patagonia San Juan Bosco");
	puts_rom("Facultad de Ingeniería, Dpto. de Sistemas (Tw)");
	puts_rom("http://www.unp.edu.ar");

    OS_Replace(startShell,10);
}

/*%%\subsection{void tris(void)}%%*/
void tris(void)
{
unsigned char v;
if (parameter[2][0]!=0)
	{		
	v = (hex2int(parameter[2][0])<<4)+hex2int(parameter[2][1]);	
	switch(param[0])
		{
		case 'a':
			TRISA=v;
			break;
		case 'b':
			TRISB=v;
			break;
		case 'c':
			TRISC=v;
			break;
		case 'd':
			TRISD=v;
			break;
		case 'e':
			TRISE=v;
			break;
		}
	}

switch(param[0])
	{
	case 'a':
		printChar(TRISA);
		break;
	case 'b':
		printChar(TRISB);
		break;
	case 'c':
		printChar(TRISC);
		break;
	case 'd':
		printChar(TRISD);
		break;
	case 'e':
		printChar(TRISE);
		break;
	}
putch('\n');
OS_Replace(startShell,10);
}

/*%%\subsection{void port(void)}%%*/
void port(void)
{
unsigned char v;
if (parameter[2][0]!=0)
	{		
	v = (hex2int(parameter[2][0])<<4)+hex2int(parameter[2][1]);	
	switch(param[0])
		{
		case 'a':
			PORTA=v;
			break;
		case 'b':
			PORTB=v;
			break;
		case 'c':
			PORTC=v;
			break;
		case 'd':
			PORTD=v;
			break;
		case 'e':
			PORTE=v;
			break;
		}
	}

switch(param[0])
	{
	case 'a':
		printChar(PORTA);
		break;
	case 'b':
		printChar(PORTB);
		break;
	case 'c':
		printChar(PORTC);
		break;
	case 'd':
		printChar(PORTD);
		break;
	case 'e':
		printChar(PORTE);
		break;
	}
putch('\n');
OS_Replace(startShell,10);
}

/*%%\subsection{void ayuda(void)}%%*/
void ayuda(void)
{
unsigned char q=0,punt,cmdName[15];
while(q<NCOMANDOS)
	{
	strcpypgm2ram(cmdName,comando[q].Nombre);
	punt = strcmp(param,cmdName);
	if (punt==0) {
		puts_rom(comando[q].Ayuda);
		OS_Replace(startShell,10);
		}
	q++;
	}
puts_rom(HELP_MSG);
for(q=0;q<NCOMANDOS;q++) 
	{
	cputs_rom(comando[q].Nombre);
	cputs_rom(":\t");
	puts_rom(comando[q].Ayuda);
	}
OS_Replace(startShell,10);
}


/*%%\subsection{void saveconf(void)}%%*/
void saveconf(void)
{
/* Do Stuff */
puts_rom("Ok.");
OS_Replace(startShell,10);
}

/*%%\subsection{void kill(void)}%%*/
void kill(void){
unsigned char proc;
proc = parameter[1][0]-0x30;
if ((param[0]=='-') &&
    (proc<=MAXPROCESS) )
	{
	OS_kill(proc,param[1]);
	printChar(proc);putch('-');printChar(param[1]);
	}
puts(param);putch('-');
puts(parameter[1]);putch('-');
OS_Replace(startShell,10);
}

/*%%\subsection{void kill(void)}%%*/
void renice(void){
unsigned char prio,proc;
proc = parameter[1][0]-0x30;
if ((param[0]!=0) &&
    (proc<=MAXPROCESS) )
	{
	prio = param[0]-0x30;
	if (param[1] != 0)
		{
		prio *=10;
		prio +=param[1]-0x30;
		}
	OS_renice(proc,prio);
	printChar(proc);putch('-');printChar(prio);
	}

OS_Replace(startShell,10);
}
