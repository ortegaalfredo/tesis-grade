/*  >>> this is file picixKernel.h
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

/* Timer-related Defines */
#define CONTEXT_SWITCH_OVERHEAD            1000
#define TMR1RESET (0xFFFF-(TICKS_BETWEEN_INTERRUPTS+CONTEXT_SWITCH_OVERHEAD))
#define TMR1RESET_HIGH TMR1RESET >> 8
#define TMR1RESET_LOW TMR1RESET & 0xFF

#define PROCESS_EMPTY  '-'
#define PROCESS_ELIG   'E'
#define PROCESS_DELAY  'D'
#define PROCESS_PAUSED 'P'

/* Basic structures for the Context switch: The registers, and the Stack */
unsigned char processPriority[MAXPROCESS];
unsigned char processCount[MAXPROCESS];
unsigned char processStatus[MAXPROCESS];
unsigned char actualProcess;

unsigned char Hstkptr[MAXPROCESS];
unsigned char HstackL[MAXPROCESS][MAX_STACK_LEVELS];
unsigned char HstackH[MAXPROCESS][MAX_STACK_LEVELS];

unsigned char Sstackl[MAXPROCESS];
unsigned char Sstackh[MAXPROCESS];

typedef struct {
    unsigned char wreg,
    			  bsr,
    			  status,
    			  prodl,
    			  prodh,
    			  fsr0h,
    			  fsr0l,
    			  tblptru,
    			  tblptrh,
    			  tblptrl,
    			  fsr1h,
    			  fsr1l,
    			  fsr2h,
    			  fsr2l,
    			  tmp0,
    			  tmp1,
    			  pclath;
   			  } sfrContext;

sfrContext sfr[MAXPROCESS];

/* Statistics */

unsigned char maxSstack[MAXPROCESS];
unsigned char maxHstack[MAXPROCESS];
unsigned char cpuTime[MAXPROCESS];
unsigned char cpuTime_temp[MAXPROCESS];
rom unsigned char *processName[MAXPROCESS];

unsigned char status_temp;
unsigned char prodl_temp;
unsigned char prodh_temp;
unsigned char bsr_temp;
unsigned char wreg_temp;
unsigned char fsr0l_temp;
unsigned char fsr0h_temp;
unsigned char tmp0_temp;
unsigned char tmp1_temp;
