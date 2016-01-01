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

#include "Particle.h"
#include "Globals.h"
#include "GameController.h"

const int LIFESPAN = 500;

extern int delta_time;

Particle::Particle()
{
	setupCommon();

	translation = new float[3]
	{
		0, 0, 0
	};

	velocity = new float[3]
	{
		0, 0, 0
	};
}

Particle::Particle(float x, float y, float z, float r, float g, float b)
{
	setupCommon();

	// Starting pos (in front of ship).
	translation = new float[3]
	{
		x, y, z
	};

	// Apply random velocity.
	velocity = new float[3]
	{
		GameController::getRandomBetween(-.1f, .1f), GameController::getRandomBetween(-.1f, .1f), 0
	};
}

void Particle::setupCommon()
{
	orientation = new float[16]
	{
		1, 0, 0, 0,
		0, 1, 0, 0,
		0, 0, 1, 0,
		0, 0, 0, 1
	};

	scaling = new float[3]
	{
		1, 1, 1
	};

	material = new float[4]
	{
		50.0f / RGB_MAX, 250.0f / RGB_MAX, 50.0f / RGB_MAX, 1
	};
}

void Particle::update(void)
{
	timer += delta_time;
	if (timer > LIFESPAN)
	{
		isExpired = true;
	}

	// Apply bullet velocity.
	translate(velocity[0], velocity[1], velocity[2]);
}

void Particle::draw(void)
{
	// Model view (since we're drawing something specific)
	glMatrixMode(GL_MODELVIEW);
	// Save glLookAt transformation (I*cam)
	glPushMatrix();

	// Apply current translation to the bullet.
	glTranslatef(translation[0], translation[1], translation[2]);
	// Apply current orientation to the bullet.
	glMultMatrixf(orientation);
	// Scale the bullet.
	glScalef(scaling[0], scaling[1], scaling[2]);

	// Set material.
	glMaterialfv(GL_FRONT, GL_AMBIENT_AND_DIFFUSE, material);

	// The particle is just a tiny sphere.
	glPushMatrix();
	{
		glutSolidSphere(.05f, 10, 5);
	}
	glPopMatrix();

	// Clean up after ourselves.
	glPopMatrix();

	// Clear garbage.
	glFlush();
}