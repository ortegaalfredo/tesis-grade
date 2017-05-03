/*  >>> this is file tfs.h
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
Tiny file system 2:
Implements a tiny unix-like file system:

Each inode: 64 bytes

The first Inode is a "directory Inode" (The / directory)

Directory Inode structure:
[13 (*char) name][1 (unsigned int) file start inode]
[13 (*char) name][1 (unsigned int) file start inode]
[13 (*char) name][1 (unsigned int) file start inode]
[13 (*char) name][1 (unsigned int) file start inode]
[2 (char) Not used] // To fill 64 bytes
[1 (unsigned int) ] next Inode or 00 (End)

Data Inode structure:
[62 (*char) data][1 (unsigned int) next inode or 0x00 (end)]

Sub-Directory: We signal that a file is a subdirectory like this:
['$' 13 (*char) name][1 (unsigned char directory start block]

Inode address: 16 bits:
[bit 14 - 15 : FS type: 00: Flash, 01: EEprom, and so on]
[bit 16 : Read-Write type: 
            0(Block mode): the FS must be formatted and accessed
	                   via the common INODE structure.
			   Used in Flash and external devices.
        1(Character mode): The FS can be read and write one character
	                   at time, and cant be formatted.
			   Used in peripherals like Serial I/O, i2c,
			   ADC, etc.
Max: 8 types,so a file in the same directory can point to flash 
or eeprom or etc.

Free space:
There are a "$Free:" file with all the space of the device linked.
So we only have to make linked-list operations to maintain the FS
coherence.
*/

void mkfs(void);
void ls(void);
void init_FS(void);
void reset_stdio(void);
void set_stdout_temp(unsigned char fd);
void set_stdin_temp(unsigned char fd);
void set_default_stdio_serial(void);
void set_default_stdio_file(unsigned char fd);
unsigned char open(const unsigned char *path,unsigned char mode);
unsigned char close(unsigned char fd);
unsigned char read(unsigned char fd,unsigned char *buf,unsigned char qty);
unsigned char write(unsigned char fd,const unsigned char *data,unsigned char qty);
unsigned char seek(unsigned char fd,int delta);
extern unsigned int stdin,stdout; //default

/* File types */
enum {
 FILE_NORMAL, FILE_EXE, FILE_FREE
};

/* File open modes */
enum {
 OPEN_R, OPEN_RWC
};

/* File Open error codes */
enum {
 OPEN_OK = 0,OPEN_ERROR_MAX_REACHED = 0xfe, OPEN_ERROR_NOT_FOUND = 0xff
};

/* File Read error codes */
enum {
 READ_EOF = 255,READ_WRONG_FD = 254
};

/* File Close error codes */
enum {
 CLOSE_OK = 0,CLOSE_ERROR_NOT_FOUND
};

#define MAX_FILENAME_SIZE 13
#define MAX_OPEN_FILES 7

#define STDIN  0
#define STDOUT 1

