#ifdef _M_IX86
#include <windows.h>
#else
#include <stream.h>
#endif

#include <GL/gl.h>
#include <GL/glu.h>
#include <GL/freeglut.h>
#include <math.h>
#include "GameObject.h"
#include "Globals.h"

void GameObject::translate(float xDelta, float yDelta, float zDelta)
{
	translation[0] += xDelta;
	translation[1] += yDelta;
	translation[2] += zDelta;

	// Keep within room X/Y.
	if (translation[0] > ROOM_SIZE) translation[0] = -ROOM_SIZE;
	else if (translation[0] < -ROOM_SIZE) translation[0] = ROOM_SIZE;
	
	if (translation[1] > ROOM_SIZE) translation[1] = -ROOM_SIZE;
	else if (translation[1] < -ROOM_SIZE) translation[1] = ROOM_SIZE;
}

void GameObject::rotate(float xDeg, float yDeg, float zDeg)
{
	float  mat[16] =
	{
		1, 0, 0, 0,
		0, 1, 0, 0,
		0, 0, 1, 0,
		0, 0, 0, 1
	};

	if (xDeg != 0)
	{
		mat[5] = cos(xDeg);
		mat[6] = -sin(xDeg);
		mat[9] = sin(xDeg);
		mat[10] = cos(xDeg);
	}

	if (yDeg != 0)
	{
		mat[0] = cos(yDeg);
		mat[2] = sin(yDeg);
		mat[8] = -sin(yDeg);
		mat[10] = cos(yDeg);
	}

	if (zDeg != 0)
	{
		mat[0] = cos(zDeg);
		mat[1] = -sin(zDeg);
		mat[4] = sin(zDeg);
		mat[5] = cos(zDeg);
	}

	// Apply the new orientation using the stack, removing and storing the updated matrix afterwards.
	glMatrixMode(GL_MODELVIEW);
	glPushMatrix();
	glLoadMatrixf(orientation);
	glMultMatrixf(mat);
	glGetFloatv(GL_MODELVIEW_MATRIX, orientation);
	glPopMatrix();
}

void GameObject::scale(float xScale, float yScale, float zScale)
{
	scaling[0] *= xScale;
	scaling[1] *= yScale;
	scaling[2] *= zScale;
}