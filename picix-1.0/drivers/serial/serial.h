#ifndef _SERIAL_H_
#define _SERIAL_H_

#define BAUD 9600    
#define FOSC 40000000L
#define NINE 0     /* Use 9bit communication? FALSE=8bit */
#define OUTPUT 0
#define INPUT 1

#define DIVIDER ((int)(FOSC/(16UL * BAUD) -1))
#define HIGH_SPEED 0

#if NINE == 1
#define NINE_BITS 0x40
#else
#define NINE_BITS 0
#endif

#if HIGH_SPEED == 1
#define SPEED 0x4
#else
#define SPEED 0
#endif

void init_comms(void);
void putch(unsigned char);

#define FPUTC_MAX_BUFFER 20
extern unsigned char fputc_buffer; // if 0: not buffered, FPUTC_MAX_BUFFER is the max value
void fputc(unsigned char fd,unsigned char byte);
unsigned char fgetc(unsigned char fd);

unsigned char getch(void);
unsigned char getche(void);
unsigned char Kbhit(void);
void fputs_rom(unsigned char fd,const rom unsigned char * s);
void fputs(unsigned char fd,const unsigned char * s);
void flush(void);
#endif
