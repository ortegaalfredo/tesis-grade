Friday, 5-5-2006

This is the first "usable" release of Picix. I give you...Picix "Vista" Edition 1.0!

1) 3+ years between minor releases....check!
2) It takes half the storage space of the system (7 Kb the OS only)....check!
3) It bogs down the cpu (process switch takes 1000 cycles aprox.) ....check!

Confirmed: I am going to be the next Microsoft.

Anyway, its free and I think it´s useful.
Should compile with microchip C compiler 3.x, and work on a plain 18F452 (Connect the Hardware UART of the PIC to the PC and fire your favorite terminal emulator to see it in action)

Fixes:

. Work with the Microchip MPLAB IDE v7.31 Emulator and Microchip MCC18 v3.0
. The timer setup has changed places and works better now
. All the Ethernet, IP and Firewall code removed (Still can be fetched on the 0.9 version)
. Removed bootloader code
. Added some comments and clarified the code

TODO:

. Learn how to program in C
. Massive optimization of everything
. Stuff

Instructions to compile:
Open the picix.mcp project file in MPLAB and hit compile. Thats all.

To make new a new Process:
Take a look how "Shell" process is setup in the main() function. It´s the main shell of the sistem, and is the only process fired in the demo.

Regards
Cachito, ortegaalfredo@gmail.com