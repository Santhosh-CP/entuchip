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
	unsigned short I; // Index Register
	unsigned short PC; // Program Counter
	unsigned char Graphics[64 * 32]; // Graphics
	unsigned char DelayTimer;
	unsigned char SoundTimer;
	unsigned short Stack[16]; // Stack with 16 Levels
	unsigned short StackPointer;
	unsigned char Key[16]; // 0 to F
	unsigned char FontSet[80] = 
	{
	  0xF0, 0x90, 0x90, 0x90, 0xF0, // 0
	  0x20, 0x60, 0x20, 0x20, 0x70, // 1
	  0xF0, 0x10, 0xF0, 0x80, 0xF0, // 2
	  0xF0, 0x10, 0xF0, 0x10, 0xF0, // 3
	  0x90, 0x90, 0xF0, 0x10, 0x10, // 4
	  0xF0, 0x80, 0xF0, 0x10, 0xF0, // 5
	  0xF0, 0x80, 0xF0, 0x90, 0xF0, // 6
	  0xF0, 0x10, 0x20, 0x40, 0x40, // 7
	  0xF0, 0x90, 0xF0, 0x90, 0xF0, // 8
	  0xF0, 0x90, 0xF0, 0x10, 0xF0, // 9
	  0xF0, 0x90, 0xF0, 0x90, 0x90, // A
	  0xE0, 0x90, 0xE0, 0x90, 0xE0, // B
	  0xF0, 0x80, 0x80, 0x80, 0xF0, // C
	  0xE0, 0x90, 0x90, 0x90, 0xE0, // D
	  0xF0, 0x80, 0xF0, 0x80, 0xF0, // E
	  0xF0, 0x80, 0xF0, 0x80, 0x80  // F
	};
	bool drawFlag;

	//functions
	void initialize(); // Initializes the System
	void cycle();
	unsigned short getOpcode(unsigned char, unsigned char);
	unsigned char getX();
	unsigned char getY();
	unsigned char getKK();
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
	switch (Opcode & 0xF000) {
		/*
		Reason for the switch value "Opcode & 0xF000"

		Chip 8 Opcodes can be divided into 16 subdivisions based on the value of the first 4 bits (0xN000)
		Futher subdivision is done for certain Opcodes which have multiple Opcodes having the same 4 first bits
		So when we perform the Bitwise AND operation on the Opcode, we eliminate the last 12 bits and keep only the first 4 bits
		The comparison is done based on the first 4 bits
		*/
	case 0x0000: {
		unsigned short last;

		last = Opcode & 0x00FF;
		if (last == 0x00E0) {
			//00E0 - cls
			//Clears the screen
			fill(begin(Graphics), end(Graphics), 0);
			fill(begin(Graphics), end(Graphics), 0);
			PC = PC + 2;
		}
		else if (last == 0x00EE) {
			//00EE - RET
			//Return from a subroutine
			PC = Stack[StackPointer];
			StackPointer--;
		}
		else {
			cout << "\nUnsupported 0x0000 series Opcode\n";
		}
		break;
	}

	case 0x1000: {
		//1nnn - JP addr
		//Jump to location nnn.
		PC = Opcode & 0x0FFF;
		break;
	}

	case 0x2000: {
		//2nnn - CALL addr
		//Call subroutine at nnn.
		StackPointer++;
		Stack[StackPointer] = PC;
		PC = Opcode & 0x0FFF;
		break;
	}

			   // Might be wrong
	case 0x3000: {
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
	}

			   //Might be wrong
	case 0x4000: {
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
	}

			   //Might be wrong
	case 0x5000: {
		//5xy0 - SE Vx, Vy
		//Skip next instruction if Vx = Vy.
		unsigned short x, y;
		x = getX();
		y = getY();
		if (v[x] == v[y]) {
			PC = PC + 2;
		}
		break;
	}

	case 0x6000: {
		//6xkk - LD Vx, byte
		//Set Vx = kk.
		unsigned char x, kk;
		x = getX();
		kk = getKK();
		v[x] = kk;
		PC = PC + 2;
		break;
	}

	case 0x7000: {
		//7xkk - ADD Vx, byte
		//Set Vx = Vx + kk.
		unsigned short x, kk;
		x = getX();
		kk = getKK();
		v[x] = v[x] + kk;
		PC = PC + 2;
		break;
	}

	case 0x8000: {
		//There are multiple opcodes which start with 8
		//They are differentiated based on the last 4 bits
		//All commands of this type have X & Y values
		unsigned short x, y, last;
		last = Opcode & 0x000F;
		x = getX();
		y = getY();
		switch (last) {
			//8xy0 - LD Vx, Vy
			//Set Vx = Vy.
		case 0: {
			v[x] = v[y];
			break;
		}

			  //8xy1 - OR Vx, Vy
			  //Set Vx = Vx OR Vy.
		case 1: {
			v[x] = v[x] | v[y];
			break;
		}

			  //8xy2 - AND Vx, Vy
			  //Set Vx = Vx AND Vy.
		case 2: {
			v[x] = v[x] & v[y];
			break;
		}

			  //8xy3 - XOR Vx, Vy
			  //Set Vx = Vx XOR Vy.
		case 3: {
			v[x] = v[x] ^ v[y];
			break;
		}

			  //8xy4 - ADD Vx, Vy
			  //Set Vx = Vx + Vy, set VF = carry.
		case 4: {
			unsigned short sum;
			sum = v[x] + v[y];
			if (sum > 255) {
				v[15] = 1;
				sum = sum & 0x00FF;
				v[x] = sum;
			}
			break;
		}

			  //8xy5 - SUB Vx, Vy
				//Set Vx = Vx - Vy, set VF = NOT borrow.
		case 5: {
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
			  //8xy6 - SHR Vx {, Vy}
			  //Set Vx = Vx SHR 1.
		case 6: {
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

			  //8xy7 - SUBN Vx, Vy
			  //Set Vx = Vy - Vx, set VF = NOT borrow.
		case 7: {
			if (v[y] > v[x]) {
				v[15] = 1;
			}
			else {
				v[15] = 0;
			}
			v[x] = v[y] - v[x];
			break;
		}

			  //8xyE - SHL Vx {, Vy}
			  //Set Vx = Vx SHL 1.
		case 0xE: {
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

		default: {
			cout << "\nUnsupported 0x8000 series Opcode\n";
			break;
		}
		}
		PC = PC + 2;
		break;
	}

	case 0x9000: {
		//9xy0 - SNE Vx, Vy
		//Skip next instruction if Vx != Vy.
		unsigned short x, y;
		x = getX();
		y = getY();
		if (v[x] != v[y]) {
			PC = PC + 2;
		}
		break;
	}

	case 0xA000: {
		//Annn - LD I, addr
		//Set I = nnn.
		I = Opcode & 0x0FFF;
		PC = PC + 2;
		break;
	}

	case 0xB000: {
		//Bnnn - JP V0, addr
		//Jump to location nnn + V0.
		PC = Opcode & 0x0FFF;
		PC = PC + v[0];
		break;
	}

	case 0xC000: {
		//Cxkk - RND Vx, byte
		//Set Vx = random byte AND kk.
		unsigned char max, rand_byte;

		max = 256;
		rand_byte = rand() % max; //Generates a random number between 0 to 255
		break;
	}


	case 0xD000: {
		//Dxyn - DRW Vx, Vy, nibble
		//Display n - byte sprite starting at memory location I at(Vx, Vy), set VF = collision.
		unsigned char x, y;
		unsigned short height;
		unsigned char pixel;

		x = getX();
		y = getY();
		height = Opcode & 0x000F;
		v[0xF] = 0; //0xF = Vf
		for (int yaxis = 0; yaxis < height; yaxis++) {
			pixel = Memory[I + yaxis];
			for (int xaxis = 0; xaxis < 8; xaxis++) {
				//xaxis < 8 because a byte has 8 bits

				if ((pixel & (0x80 >> xaxis)) != 0) {
					if (Graphics[(x + xaxis + (y + yaxis) * 64)] == 1) {
						//Checking if the current pixel is set or not. If it is set, the collision is registered to VF
						v[0xF] = 1;
					}
					Graphics[(x + xaxis + (y + yaxis * 64))] = Graphics[(x + xaxis + (y * yaxis * 64))] ^ 1;
				}
			}
		}
		drawFlag = true;
		PC = PC + 2;

		break;
	}


	case 0xE000: {
		unsigned short last;
		unsigned char x;
		last = Opcode & 0x00FF;
		x = getX();

		if (last == 0x009E) {
			//Ex9E - SKP Vx
			//Skip next instruction if key with the value of Vx is pressed.

			if (Key[v[x]] != 0) {
				PC = PC + 4;
			}
			else {
				PC = PC + 2;
			}
		}
		else if (last == 0x00A1) {
			//ExA1 - SKNP Vx
			//Skip next instruction if key with the value of Vx is not pressed.

			if (Key[v[x]] == 0) {
				PC = PC + 4;
			}
			else {
				PC = PC + 2;
			}
		}
		else {
			cout << "\nUnsupported 0xE000 series Opcode\n";
		}
		break;
	}

			   //NOT FULLY IMPLEMENTED
	case 0xF000: {
		unsigned short last;
		unsigned char x;
		last = Opcode & 0x00FF;
		x = getX();

		switch (last) {
		case 0x0007: {
			//Fx07 - LD Vx, DT
			//Set Vx = delay timer value.
			v[x] = DelayTimer;
			break;
		}

				   //NOT IMPLEMENTED. NEEDS keypress check function
		case 0x000A: {
			//Fx0A - LD Vx, K
			//Wait for a key press, store the value of the key in Vx.

		}

		case 0x0015: {
			//Fx15 - LD DT, Vx
			//Set delay timer = Vx.
			DelayTimer = v[x];
			break;
		}

		case 0x0018: {
			//Fx18 - LD ST, Vx
			//Set sound timer = Vx.
			SoundTimer = v[x];
			break;
		}

		case 0x001E: {
			//Fx1E - ADD I, Vx
			//Set I = I + Vx.
			I = I + v[x];
			break;
		}
				   //NOT IMPLEMENTED. NEEDS sprite loading
		case 0x0029: {
			//Fx29 - LD F, Vx
			//Set I = location of sprite for digit Vx.
			break;
		}

		case 0x0033: {
			//Fx33 - LD B, Vx
			//Store BCD representation of Vx in memory locations I, I + 1, and I + 2.

			unsigned char digit_0s, digit_10s, digit_100s;
			unsigned short value;

			value = v[x];
			//Obtaining the digits
			digit_0s = value % 10;
			value = value / 10;
			digit_10s = value % 10;
			value = value / 10;
			digit_100s = value % 10;

			//Storing in memory
			Memory[I] = digit_100s;
			Memory[I + 1] = digit_10s;
			Memory[I + 2] = digit_0s;

			//Shouldn't I be incremented? hmmm
			break;
		}

		case 0x0055: {
			//Fx55 - LD [I], Vx
			//Store registers V0 through Vx in memory starting at location I.
			for (int i = 0; i <= x; i++) {
				Memory[I + i] = v[i];
			}
			break;
		}

		case 0x0065: {
			//Fx65 - LD Vx, [I]
			//Read registers V0 through Vx from memory starting at location I.
			for (int i = 0; i <= x; i++) {
				v[i] = Memory[I + i];
			}
			break;
		}
		}
		PC = PC + 2;
	}
	}
}
unsigned char Chip8::getX() {
	/*
	In all Chip 8 Opcode, if X exists, it is in bits 5-8
	That is, 0x0X00
	This function obtains the X value and returns it.
	*/
	unsigned short x;
	unsigned char ret_x;

	x = Opcode & 0x0F00;
	x = x >> 8;
	ret_x = x;
	return ret_x;
}
unsigned char Chip8::getY() {
	/*
	In all Chip 8 Opcode, if X exists, it is in bits 9-12
	That is, 0x00Y0
	This function obtains the Y value and returns it.
	*/
	unsigned short y;
	unsigned char ret_y;

	y = Opcode & 0x00F0;
	y = y >> 4;
	ret_y = y;
	return ret_y;
}
unsigned char Chip8::getKK() {
	/*
	In all Chip 8 Opcode, if KK exists, it is in the second byte
	That is, 0x00KK
	This function obtains the K value and returns it.
	*/
	unsigned short kk;
	unsigned char ret_kk;

	kk = Opcode & 0x00FF;
	ret_kk = kk;
	return ret_kk;
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