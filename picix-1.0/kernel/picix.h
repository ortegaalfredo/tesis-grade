/*  >>> this is file picix.h
        please edit it to fit your needs.
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

/*We want 2000 instructions as our time slice, or 200us @ 10MHz .  
  Timer 1 interrupts when it gets to 0xFFFF or 65535.  Therefore, 
  we set timer 1 to 65535 minus 2000 = 63535, then wait 2000 ticks 
  until rollover at 65535.  To test, use simulator to find that 
  its exactly correct*/

#define  TICKS_BETWEEN_INTERRUPTS      30000  /* 1 ms @ 40Mhz */

#define MAXPROCESS 2
#define MAX_STACK_LEVELS 22

/* The main stack declared in the linker script */
#define MAIN_STACK_LOW  0x80
#define MAIN_STACK_HIGH 0x00

#define OSVersion() "1.0"

#define OS_disable() INTCONbits.GIE = 0;INTCONbits.PEIE = 0;

#define OS_enable() INTCONbits.GIE = 1;INTCONbits.PEIE = 1;

void OS_init(void);

void setTimer(void);

void OSCreateTask(void (*proc)(void),unsigned char prio,unsigned char *SofStack,const rom unsigned char *name);
void OS_sched(void);
void OS_switch(void);
void OS_delay(unsigned char ticksL,unsigned char ticksH);
void OSRpt(void);
void OS_Replace(void (*proc)(void),unsigned char prio);
void OS_kill(unsigned char proc,unsigned char st);
void OS_renice(unsigned char proc,unsigned char prio);

void printk(unsigned char c);
void printChark(unsigned char c);
void printIntk(unsigned int i);

