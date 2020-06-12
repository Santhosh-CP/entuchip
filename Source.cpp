#include<iostream>
#include<string>
#include<filesystem>
#include "entu.h"
#include "GL/freeglut.h"

using namespace std;

/*
				ENTUCHIP
This is my attempt at writing a Chip-8 Emulator

Chip-8 Basic Details:-
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
A Hex-based keypad
*/

/*
STATUS
------
Program working!


POSSIBLE FUTURE IMPROVEMENTS
----------------------------
Show a keymap on the console (Which key on the keyboard corresponds to which C8 key)
Add a program icon
Extensive testing!
Implement beep based sound?
*/

//Original Chip-8 Screen dimensions
#define SCREEN_WIDTH 64
#define SCREEN_HEIGHT 32
#define MULTIPLIER 12

//Display Window dimensions
#define WINDOW_WIDTH SCREEN_WIDTH * MULTIPLIER
#define WINDOW_HEIGHT SCREEN_HEIGHT * MULTIPLIER
#define FPS 1000

Entu entuchip;


//Function Prototypes
void display();
void reshape(int, int);
void timer(int);
void keyboardPressed(unsigned char, int, int);
void keyboardUnpressed(unsigned char, int, int);

//Screen Graphics Generation
void loadGame();
void drawScreenPixel(int, int);
void updateQuads(Entu);


int main(int argc, char** argv) {
	loadGame();

	//Display related code
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA);
	glutInitWindowSize(WINDOW_WIDTH, WINDOW_HEIGHT);
	glutInitWindowPosition(512, 512);
	glutCreateWindow("EntuChip by Santhosh C P");

	glutDisplayFunc(display);
	glutReshapeFunc(reshape);
	glutTimerFunc(0, timer, 0);
	glutKeyboardFunc(keyboardPressed);
	glutKeyboardUpFunc(keyboardUnpressed);

	glutMainLoop();

	return 0;
}


void display() {
	entuchip.cycle();

	if (entuchip.drawFlag) {
		glClear(GL_COLOR_BUFFER_BIT); //Clears the color buffer(background)

		updateQuads(entuchip);

		glutSwapBuffers();
		entuchip.drawFlag = false;
	}
}

void reshape(int w, int h) {
	glClearColor(0.0f, 0.0f, 0.5f, 0.0f);

	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluOrtho2D(0, SCREEN_WIDTH, SCREEN_HEIGHT, 0);
	glMatrixMode(GL_MODELVIEW);

	glViewport(0, 0, w, h);
}

void timer(int) {
	glutPostRedisplay();
	glutTimerFunc(1000 / FPS, timer, 0);
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


// Screen Graphics Generation
void drawScreenPixel(int x, int y) {
	//Converting to GLfloat values
	GLfloat X = x + 0.0f;
	GLfloat Y = y + 0.0f;

	glBegin(GL_QUADS);
	glVertex3f(X, Y, 0);
	glVertex3f(X, Y + 1, 0);
	glVertex3f(X + 1, Y + 1, 0);
	glVertex3f(X + 1, Y, 0);
}

void updateQuads(const Entu entu) {

	for (int y = 0; y < SCREEN_HEIGHT; y++) {
		for (int x = 0; x < SCREEN_WIDTH; x++) {
			if (entu.Graphics[(y * 64) + x] == 0) {
				glColor3f(0.0f, 0.0f, 0.0f);
			}
			else {
				glColor3f(1.0f, 1.0f, 1.0f);
			}

			drawScreenPixel(x, y);
		}
	}
}

void loadGame() {
	string ROM;

	cout << "Entu Chip by Santhosh C P" << endl;
	cout << "Enter ROM filename (if it is in the same directory) or its absolute path\n";

	while (true) {
		cin >> ROM;
		if (filesystem::exists(ROM)) {
			break;
		}
		else {
			cout << "File does not exist. Please recheck the path/filename entered\n";
		}
	}

	entuchip.load(ROM);
}