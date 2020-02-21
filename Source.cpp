#include<iostream>
#include "Entu.h"
#include "GL/freeglut.h"

using namespace std;

//Window dimensions
#define WINDOW_HEIGHT 512
#define WINDOW_WIDTH 1024

//Original Chip-8 Screen dimensions
#define SCREEN_HEIGHT 32
#define SCREEN_WIDTH 64
unsigned char ScreenGraphics[SCREEN_WIDTH][SCREEN_WIDTH][3]; //For RGB

Entu entuchip;



/*
TASKS
-----
chip8 display and key functions need to be implemented
Load ROM function needs to be implemented
The rom to be loaded should be selected by File -> Load ROM in the GUI


STATUS
------
freeglut libraries have been added to the project. The project complied successfully!
*/


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

void keyboardPressed(unsigned char key, int x, int y);
void keyboardUnpressed(unsigned char key, int x, int y);

void createTextures() {

	//Clear the screen
	for (int i = 0; i < SCREEN_HEIGHT; i++) {
		for (int j = 0; j < SCREEN_WIDTH; j++) {
			ScreenGraphics[i][j][0] = 0;
			ScreenGraphics[i][j][1] = 0;
			ScreenGraphics[i][j][2] = 0;
		}
	}

	//Creating a texture
	glTexImage2D(GL_TEXTURE_2D, 0, 3, SCREEN_WIDTH, SCREEN_HEIGHT, 0, GL_RGB, GL_UNSIGNED_BYTE, (GLvoid*)ScreenGraphics);

	//Setting it up
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);

	//Enabling the texture
	glEnable(GL_TEXTURE_2D);
}

void updateTextures(const Entu entu) {
	int pixel;

	//Updating the screen
	for (int i = 0; i < SCREEN_HEIGHT; i++) {
		for (int j = 0; j < SCREEN_WIDTH; j++) {
			pixel = i * 64 + j;

			if (entu.Graphics[pixel] == 0) {
				ScreenGraphics[i][j][0] = 0;
				ScreenGraphics[i][j][1] = 0;
				ScreenGraphics[i][j][2] = 0;
			}
			else {
				ScreenGraphics[i][j][0] = 255;
				ScreenGraphics[i][j][1] = 255;
				ScreenGraphics[i][j][2] = 255;
			}
		}
	}

	//Updating the textures
	glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, SCREEN_WIDTH, SCREEN_HEIGHT, GL_RGB, GL_UNSIGNED_BYTE, (GLvoid*)ScreenGraphics);


	//Why?
	glBegin(GL_QUADS);

	glTexCoord2d(0.0, 0.0);
	glVertex2d(0.0, 0.0);

	glTexCoord2d(1.0, 0.0);
	glVertex2d(WINDOW_WIDTH, 0.0);

	glTexCoord2d(1.0, 1.0);
	glVertex2d(WINDOW_WIDTH, WINDOW_HEIGHT);

	glTexCoord2d(0.0, 1.0);
	glVertex2d(0.0, WINDOW_HEIGHT);

	glBegin(GL_QUADS);
}

void myDisplay() {
	entuchip.cycle();

	if (entuchip.drawFlag) {
		glClear(GL_COLOR_BUFFER_BIT); //Clears the color buffer(background)
		updateTextures(entuchip);
		glutSwapBuffers();
		entuchip.drawFlag = false;
	}
}

int main(int argc, char** argv) {
	cout << "This is Entu Chip!" << endl;

	if (!entuchip.load(argv[1]))
		return 1;

	//Display related code
	glutInit(&argc, argv);
	glutInitWindowSize(WINDOW_WIDTH, WINDOW_HEIGHT);
	glutCreateWindow("Entu Chip by Santhosh C P");
	glutDisplayFunc(myDisplay);
	glutIdleFunc(myDisplay);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, 64, 32, 0, GL_RGBA, GL_UNSIGNED_BYTE, entuchip.Graphics);
	glutMainLoop();
	
	glutKeyboardFunc(keyboardPressed);
	glutKeyboardFunc(keyboardUnpressed);
	//TODO - Load ROM here


	while (true) {
		entuchip.cycle();
	}

	return 0;
}

void keyboardPressed(unsigned char key, int x, int y) {
	// We will be using the first 4 keys on the 4 rows of the keyboard

	//1 2 3 4 Row
	if (key == '1')
		entuchip.Key[0x1] = 1;
	else if (key == '2')
		entuchip.Key[0x2] = 1;
	else if (key == '3')
		entuchip.Key[0x3] = 1;
	else if (key == '4')
		entuchip.Key[0xC] = 1;

	//Q W E R Row
	else if (key == 'q')
		entuchip.Key[0x4] = 1;
	else if (key == 'w')
		entuchip.Key[0x5] = 1;
	else if (key == 'e')
		entuchip.Key[0x6] = 1;
	else if (key == 'r')
		entuchip.Key[0xD] = 1;

	//A S D F Row
	else if (key == 'a')
		entuchip.Key[0x7] = 1;
	else if (key == 's')
		entuchip.Key[0x8] = 1;
	else if (key == 'd')
		entuchip.Key[0x9] = 1;
	else if (key == 'f')
		entuchip.Key[0xE] = 1;

	//Z X C V Row
	else if (key == 'z')
		entuchip.Key[0xA] = 1;
	else if (key == 'x')
		entuchip.Key[0x0] = 1;
	else if (key == 'c')
		entuchip.Key[0xB] = 1;
	else if (key == 'v')
		entuchip.Key[0xF] = 1;
}

void keyboardUnpressed(unsigned char key, int x, int y) {
	//1 2 3 4 Row
	if (key == '1')
		entuchip.Key[0x1] = 0;
	else if (key == '2')
		entuchip.Key[0x2] = 0;
	else if (key == '3')
		entuchip.Key[0x3] = 0;
	else if (key == '4')
		entuchip.Key[0xC] = 0;

	//Q W E R Row
	else if (key == 'q')
		entuchip.Key[0x4] = 0;
	else if (key == 'w')
		entuchip.Key[0x5] = 0;
	else if (key == 'e')
		entuchip.Key[0x6] = 0;
	else if (key == 'r')
		entuchip.Key[0xD] = 0;

	//A S D F Row
	else if (key == 'a')
		entuchip.Key[0x7] = 0;
	else if (key == 's')
		entuchip.Key[0x8] = 0;
	else if (key == 'd')
		entuchip.Key[0x9] = 0;
	else if (key == 'f')
		entuchip.Key[0xE] = 0;

	//Z X C V Row
	else if (key == 'z')
		entuchip.Key[0xA] = 0;
	else if (key == 'x')
		entuchip.Key[0x0] = 0;
	else if (key == 'c')
		entuchip.Key[0xB] = 0;
	else if (key == 'v')
		entuchip.Key[0xF] = 0;
}