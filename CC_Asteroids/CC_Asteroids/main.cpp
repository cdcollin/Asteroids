#ifdef _M_IX86
#include <windows.h>
#else
#include <stream.h>
#endif

#include <iostream>
#include <GL/gl.h>
#include <GL/glu.h>
#include <GL/freeglut.h>
#include <math.h>

#include "Globals.h"
#include "GameController.h"

/*
Asteroids!

Author: Chris Collins
*/

// Function prototypes.
void setupGLUTStuff(void);
void setupCamera(void);
void setupGLStuff(void);
void setupLighting(void);
void initGame(void);
void update(int);
void draw(void);
void handle_menu(int);
void handle_keyboard(unsigned char, int, int);
void handle_special(int, int, int);
void handle_keyboard_up(unsigned char, int, int);
void handle_special_up(int, int, int);

// Constants.
const int TIMER = 10;

// "Engine" Stuff.
int elapsed_old = 0; // in milliseconds
int elapsed_new = 0; // millis
int delta_time = 0; // ms

// Game objects.
GameController* controller;

void main(int argc, char *argv[])
{
	glutInit(&argc, argv);
	setupGLUTStuff();
	setupCamera();
	setupGLStuff();
	setupLighting();

	initGame();

	// Main game loop.
	glutMainLoop();
}

/*
GLUT initialization and callbacks.
*/
void setupGLUTStuff()
{
	// Initialize GLUT
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);
	glutInitWindowSize(1280, 960);
	glutCreateWindow("Asteroids - Now with more Thrust!");

	// Setup GLUT callbacks (input, game loop, etc)
	glutTimerFunc(TIMER, update, 0);
	glutDisplayFunc(draw);
	glutKeyboardFunc(handle_keyboard);
	glutKeyboardUpFunc(handle_keyboard_up);
	glutSpecialFunc(handle_special);
	glutSpecialUpFunc(handle_special_up);

	// Setup GLUT popup menu (exits the game)
	glutCreateMenu(handle_menu);
	glutAddMenuEntry("New Game", 0);
	glutAddMenuEntry("Quit", 1);
	glutAttachMenu(GLUT_RIGHT_BUTTON);
}

/*
Setup perspective view and push it onto the modelview stack.
*/
void setupCamera()
{
	// Setup perspective projection (camera)
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	glOrtho(-ROOM_SIZE, ROOM_SIZE, -ROOM_SIZE, ROOM_SIZE, 0, 50);
	gluLookAt(0, 0, 10, 0, 0, -1, 0, 1, 0);

	// Setup model transformations (push identity onto stack and aim the camera)
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
}

/*
Shade mode, background color, and closing functionality.
*/
void setupGLStuff()
{
	glShadeModel(GL_FLAT);

	// Setup background colour
	glClearDepth(1.0);			
	glClearColor(21.0f / RGB_MAX, 24.0f / RGB_MAX, 43.0f / RGB_MAX, 1);
	glEnable(GL_DEPTH_TEST);

	glutSetOption(GLUT_ACTION_ON_WINDOW_CLOSE, GLUT_ACTION_GLUTMAINLOOP_RETURNS);
}

/*
Create, position, and turn on our lights.
*/
void setupLighting()
{
	float lt_dif0[] = { .8f, .8f, .8f, 1 };
	float lt_pos0[] = { 0, 0, 1, 0 };
	float lt_spc0[] = { 0, 0, 0, 1 };
	float lt_col0[] = { .2f, .2f, .2f, 1 };

	float lt_dif1[] = { 0, 0, 0, 0 };
	float lt_pos1[] = { 15, 15, 1, 0 };
	float lt_spc1[] = { .2f, .2f, .2f, 1 };
	float lt_col1[] = { .8f, .8f, .8f, 1 };

	glLightfv(GL_LIGHT0, GL_POSITION, lt_pos0);
	glLightfv(GL_LIGHT0, GL_AMBIENT, lt_col0);
	glLightfv(GL_LIGHT0, GL_DIFFUSE, lt_dif0);
	glLightfv(GL_LIGHT0, GL_SPECULAR, lt_spc0);
	glEnable(GL_LIGHT0);

	glLightfv(GL_LIGHT1, GL_POSITION, lt_pos1);
	glLightfv(GL_LIGHT1, GL_COLOR, lt_col1);
	glLightfv(GL_LIGHT1, GL_DIFFUSE, lt_dif1);
	glLightfv(GL_LIGHT1, GL_SPECULAR, lt_spc1);
	glEnable(GL_LIGHT1);

	float amb[] = { 0, 0, 0, 1 };
	glLightModelfv(GL_LIGHT_MODEL_AMBIENT, amb);

	glEnable(GL_LIGHTING);

	glShadeModel(GL_FLAT);
	glEnable(GL_DEPTH);
	glEnable(GL_NORMALIZE);
}

/*
Create game objects and sets up initial game timer/state.
*/
void initGame()
{
	// Start elapsed timer.
	elapsed_new = glutGet(GLUT_ELAPSED_TIME);
	
	// Create Game Controller.
	controller = new GameController();
	controller->setup();
}

/*
Main update loop callback.
*/
void update(int t)
{
	// Update delta_time.
	elapsed_old = elapsed_new;
	elapsed_new = glutGet(GLUT_ELAPSED_TIME);
	delta_time = elapsed_new - elapsed_old; // time since last update (in ms).

	// Update game.
	controller->update();

	// Set up the next update.
	glutTimerFunc(TIMER, update, 0);
}

/*
Display callback.
*/
void draw(void)
{
	// Clear previous draw.
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	// Draw game.
	controller->draw();

	// Display back buffer and set up the next draw.
	glutSwapBuffers();
	glutPostRedisplay();
}

void handle_keyboard(unsigned char key, int x, int y)
{
	controller->handle_keyboard(key, x, y);
}

void handle_special(int key, int x, int y)
{
	controller->handle_special(key, x, y);
}

void handle_keyboard_up(unsigned char key, int x, int y)
{
	controller->handle_keyboard_up(key, x, y);
}

void handle_special_up(int key, int x, int y)
{
	controller->handle_special_up(key, x, y);
}

void handle_menu(int x)
{
	controller->handle_menu(x);
}
