#pragma once
#ifndef CHIP8
#define CHIP8

/*
Task to be implemented

fontset - where to get it?
*/

using namespace std;

class Chip8 {
	unsigned short Opcode; // Stores the current opcode
	unsigned char Memory[4096]; // Stores the 4K memory
	unsigned char v[16]; // General purpose registers v0 to vE
	unsigned char I; // Index Register
	unsigned char PC; // Program Counter
	unsigned char Graphics[64 * 32]; // Graphics
	unsigned char DelayTimer;
	unsigned char SoundTimer;
	unsigned char Stack[16]; // Stack with 16 Levels
	unsigned char StackPointer;
	unsigned char Key[16]; // 0 to F
	unsigned char FontSet[80];

	//functions
	void initialize(); // Initializes the System
	void cycle();
	unsigned short getOpcode(unsigned char, unsigned char);
	unsigned short getX();
	unsigned short getY();
	unsigned short getKK();
};

void Chip8::initialize() {
	PC = 0x200; //PC starts at 0x200

	//resetting the registers
	Opcode = 0;
	I = 0;
	StackPointer = 0;

	//Filling all the arrays with 0 in all memory locations. Essentially "reset"ting them
	fill(begin(Memory), end(Memory), 0); 
	fill(begin(v), end(v), 0);
	fill(begin(Graphics), end(Graphics), 0);
	fill(begin(Stack), end(Stack), 0);

	//resetting the timers
	DelayTimer = 0;
	SoundTimer = 0;

	//Loading the fontset
	//The fontset is stored from the memory address 0x50
	for (int i = 0; i < 80; i++) {
		Memory[i + 0x50] = FontSet[i];
	}
}

void Chip8::cycle() {
	//Obtaining the opcode
	Opcode = getOpcode(Memory[PC], Memory[PC + 1]);

	//Decoding the opcode
	switch (Opcode) { //WRONG
		

		case 0xA000: 
			/*
			ANNN
			sets I = NNN
			we do Opcode (0xANNN) & 0x0FFF is to preserve the NNN values while eliminating the 'A' value
			*/
			I = Opcode & 0x0FFF;
			PC = PC + 2;
			break;

		case 0x00E0:
			//00E0 - cls
			//Clears the screen
			fill(begin(Graphics), end(Graphics), 0);
			PC = PC + 2;
			break;

		case 0x00EE:
			//00EE - RET
			//Return from a subroutine
			PC = Stack[StackPointer];
			StackPointer--;
			break;

		case 0x1000:
			//1nnn - JP addr
			//Jump to location nnn.
			PC = Opcode & 0x0FFF;
			break;

		case 0x2000:
			//2nnn - CALL addr
			//Call subroutine at nnn.
			StackPointer++;
			Stack[StackPointer] = PC;
			PC = Opcode & 0x0FFF;
			break;

		// Might be wrong
		case 0x3000:
			//3xkk - SE Vx, byte
			//Skip next instruction if Vx = kk.
			unsigned short x, kk;
			x = getX();
			kk = getKK();
			if (v[x] == kk) {
				PC = PC + 2; 
			}
			//Shouldn't here be a pc+2?
			break;

		//Might be wrong
		case 0x4000:
			//4xkk - SNE Vx, byte
			//Skip next instruction if Vx != kk.
			unsigned short x, kk;
			x = getX();
			kk = getKK();
			if (v[x] != kk) {
				PC = PC + 2;
			}
			//Shouldn't here be a pc+2?
			break;

		//Might be wrong
		case 0x5000:
			//5xy0 - SE Vx, Vy
			//Skip next instruction if Vx = Vy.
			unsigned short x, y;
			x = getX();
			y = getY();
			if (v[x] == v[y]) {
				PC = PC + 2;
			}
			break;

		case 0x6000:
			//6xkk - LD Vx, byte
			//Set Vx = kk.
			unsigned short x, kk;
			x = getX();
			kk = getKK();
			v[x] = kk;
			PC = PC + 2;
			break;

		case 0x7000:
			//7xkk - ADD Vx, byte
			//Set Vx = Vx + kk.
			unsigned short x, kk;
			x = getX();
			kk = getKK();
			v[x] = v[x] + kk;
			PC = PC + 2;
			break;

		case 0x8000: {
			//There are multiple opcodes which start with 8
			//They are differentiated based on the last 4 bits
			//All commands of this type have X & Y values
			unsigned short x, y, last;
			last = Opcode & 0x000F;
			x = getX();
			y = getY();
			switch (last) {
			case 0: {
				//8xy0 - LD Vx, Vy
				//Set Vx = Vy.
				v[x] = v[y];
				break;
			}
			case 1: {
				//8xy1 - OR Vx, Vy
				//Set Vx = Vx OR Vy.
				v[x] = v[x] | v[y];
				break;
			}
			case 2:
				//8xy2 - AND Vx, Vy
				//Set Vx = Vx AND Vy.
				v[x] = v[x] & v[y];
				break;

			case 3: {
				//8xy3 - XOR Vx, Vy
				//Set Vx = Vx XOR Vy.
				v[x] = v[x] ^ v[y];
				break;
			}
			case 4: {
				//8xy4 - ADD Vx, Vy
				//Set Vx = Vx + Vy, set VF = carry.
				unsigned short sum;
				sum = v[x] + v[y];
				if (sum > 255) {
					v[15] = 1;
					sum = sum & 0x00FF;
					v[x] = sum;
				}
				break;
			}
			case 5: {
				//8xy5 - SUB Vx, Vy
				//Set Vx = Vx - Vy, set VF = NOT borrow.
				if (v[x] > v[y]) {
					v[15] = 1;
				}
				else {
					v[15] = 0;
				}
				v[x] = v[x] - v[y];
				break;
			}

				//TEST
			case 6: {
				//8xy6 - SHR Vx {, Vy}
				//Set Vx = Vx SHR 1.
				unsigned char end;
				end = v[x] & 0x0F;
				end = end & 0b0001;
				if (end == 1) {
					v[15] = 1;
				}
				else {
					v[15] = 0;
				}
				v[x] = v[x] >> 1;
				break;
			}

			case 7: {
				//8xy7 - SUBN Vx, Vy
				//Set Vx = Vy - Vx, set VF = NOT borrow.
				if (v[y] > v[x]) {
					v[15] = 1;
				}
				else {
					v[15] = 0;
				}
				v[x] = v[y] - v[x]; 
				break;
			}
			case 0xE: {
				//8xyE - SHL Vx {, Vy}
				//Set Vx = Vx SHL 1.
				unsigned char start;
				start = v[x] & 0xF0;
				start = start >> 7; // Only the MSB is remaining
				if (start == 1) {
					v[15] = 1;
				}
				else {
					v[15] = 0;
				}
				v[x] = v[x] << 1;
				break;

			}
			PC = PC + 2;
			break;
			}
			}

	}
}
unsigned short Chip8::getX() {
	/*
	In all Chip 8 Opcode, if X exists, it is in bits 5-8
	That is, 0x0X00
	This function obtains the X value and returns it.
	*/
	unsigned short x;
	x = Opcode & 0x0F00;
	x = x >> 8;
	return x;
}

unsigned short Chip8::getY() {
	/*
	In all Chip 8 Opcode, if X exists, it is in bits 9-12
	That is, 0x00Y0
	This function obtains the Y value and returns it.
	*/
	unsigned short y;
	y = Opcode & 0x00F0;
	y = y >> 4;
	return y;
}
unsigned short Chip8::getY() {
	/*
	In all Chip 8 Opcode, if KK exists, it is in the second byte
	That is, 0x00KK
	This function obtains the K value and returns it.
	*/
	unsigned short kk;
	kk = Opcode & 0x00FF;
	return kk;
}

unsigned short Chip8::getOpcode(unsigned char a, unsigned char b) {
	/*
	Memory in Chip 8 is one byte
	But opcodes are two bytes
	In order to read one opcode, two memory bytes have to be read
	This function combines the two memory bytes and returns a single 2 byte opcode

	2 Bytes = 16 bits
	The first memory byte is left-shifted by 8 bits. This adds 8 Zeroes to its left
	A bitwise OR operation is then performed between the left-shifted memory byte and the second memory byte
	The resultant provides us the actual opcode
	*/
	unsigned short code;

	code = a << 8; // First Memory byte is left shifted 8 times
	code = code | b; // Bitwise OR operation with the Second Memory byte
	return code;
}
#endif