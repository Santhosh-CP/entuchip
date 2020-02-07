#include<iostream>
#include "chip8.h"
//#include "freeglut/freeglut.h"

using namespace std;

/*
This is my attempt at writing a Chip-8 Emulator

Chip 8 Information:-

Basic Details:-
35 opcodes
opcode = 2 bytes long
4K Memory
15 one byte General purpose registers (v0 to vE). 
vF is used for carry flag
Index Register I
Program Counter PC
Display screen size is 64 x 32 (2048 pixels)
Two timers - Delay Timer & Sound Timer
These two timers count at 80Hz. When they are set to a value above 0, they count down to zero
A Stack is needed to handle jumps. This stack has 16 levels
A Hexbased keypad
*/

void main() {
	cout << "This is Entu Chip!";

	unsigned short temp = 0x0A00;
	temp = temp & 0x0F00;
	temp = temp >> 8;
	cout << temp;

}