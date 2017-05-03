/*  >>> this is file tfs2_ll.h
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
Low-level drivers headers

*/

void LL_read (unsigned char *buf,unsigned int dst,unsigned char len,unsigned char fsType);
void LL_write(const unsigned char *data,unsigned int,unsigned char,unsigned char fsType);
void LL_info (unsigned int *fs_size_bytes,unsigned char *about,unsigned char fsType);

//void set_LL_Driver(unsigned char fsType);

/* FS types */
enum 
{ 
  FLASH_FS = 0,EEPROM_FS = 1,EXT_FS = 2
};

#define ROOT_DIR 0x003f /* The address of the '/' dir (in flash)*/ 
//#define ROOT_DIR 0x2003 /* The address of the '/' dir (in eeprom)*/

/* FS data */
#define FLASH_SIZE 4096
#define FLASH_MAX 32704
#define FLASH_START FLASH_MAX-FLASH_SIZE
#define FLASH_DEVICE "flash" 

#define EEPROM_SIZE 256
#define EEPROM_MAX 256
#define EEPROM_START EEPROM_MAX-EEPROM_SIZE
#define EEPROM_DEVICE "eeprom" 

#define EXT_SIZE 32768
#define EXT_MAX 32768
#define EXT_START EXT_MAX-EXT_SIZE
#define EXT_DEVICE "ext" 

#define INODE_SIZE 64

#define DUMMY_DEVICE "Null" 
#define SERIAL_DEVICE "ttyS0" 


//******************************* Serial I/O Definitions */

#define MAX_SERIALI_BUFFER 2 //We can't type very fast...
#define MAX_SERIALO_BUFFER 8


