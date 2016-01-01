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
#include <time.h>

#include "Asteroid.h"
#include "Globals.h"
#include "GameController.h"
#include "Particle.h"
#include "Bullet.h"
#include "Player.h"

const int NUM_VERTICES = 12;
const int NUM_DIMENSIONS = 3;
const float MIN_SHIFT = -1.5f;
const float MAX_SHIFT = 1.5f;

const float LARGE = 1;
const float MEDIUM = .5f;
const float SMALL = .25f;
const int SPLIT = 2;
const int MIN_PARTICLES = 8;
const int MAX_PARTICLES = 12;

Asteroid::Asteroid()
{
	setupCommon();

	translation = new float[3]
	{
		0, 0, 0
	};

	// Pick an edge of the screen to appear on.
	float edge = GameController::getRandomBetween(0.0f, 1.0f);
	if (edge < .25f)
	{
		// Left x, random y.
		translation[0] = -ROOM_SIZE;
		translation[1] = GameController::getRandomBetween(-ROOM_SIZE, ROOM_SIZE);
	}
	else if (edge < .5f)
	{
		// Right x, random y.
		translation[0] = ROOM_SIZE;
		translation[1] = GameController::getRandomBetween(-ROOM_SIZE, ROOM_SIZE);
	}
	else if (edge < .75f)
	{
		// Bottom y, random x.
		translation[1] = -ROOM_SIZE;
		translation[0] = GameController::getRandomBetween(-ROOM_SIZE, ROOM_SIZE);
	}
	else
	{
		// Top y, random x.
		translation[1] = ROOM_SIZE;
		translation[0] = GameController::getRandomBetween(-ROOM_SIZE, ROOM_SIZE);
	}
}

Asteroid::Asteroid(float x, float y, float z)
{
	setupCommon();

	translation = new float[3]
	{
		x, y, z
	};
}

void Asteroid::setupCommon()
{
	generateVertices();

	material = new float[4]
	{
		160.0f / RGB_MAX, 110.0f / RGB_MAX, 43.0f / RGB_MAX, 1
	};

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

	// Asteroids start off with a constant velocity and rotation.
	velocity = new float[3]
	{
		GameController::getRandomBetween(-.1f, .1f), GameController::getRandomBetween(-.1f, .1f), 0
	};

	xSpin = GameController::getRandomBetween(-.01f, .01f);
	ySpin = GameController::getRandomBetween(-.01f, .01f);
	zSpin = GameController::getRandomBetween(-.01f, .01f);
}

float Asteroid::getRadius()
{
	// Any scaling factor should work; it's always the same value as the radius.
	return scaling[0] * ((1 + MAX_SHIFT) * 2); // Use the maximum possible radius given our possible icosahedron shifts.
}

int Asteroid::getPoints()
{
	if (wasShot == true)
	{
		if (scaling[0] >= LARGE)
		{
			return VAL_ASTEROID_LARGE;
		}
		else if (scaling[0] >= MEDIUM)
		{
			return VAL_ASTEROID_MEDIUM;
		}
		else
		{
			return VAL_ASTEROID_SMALL;
		}
	}

	return 0;
}

void Asteroid::generateVertices()
{
	asteroidVertices = new float*[NUM_VERTICES];
	int i;
	for (i = 0; i < NUM_VERTICES; i++)
	{
		asteroidVertices[i] = new float[NUM_DIMENSIONS];
	}

	float randomShift = GameController::getRandomBetween(MIN_SHIFT, MAX_SHIFT);
	float t = (1.0f + sqrt(5.0f)) / 2.0f;

	// Vertex positions, ordered counterclockwise.
	asteroidVertices[0][0] = -1 + randomShift;
	asteroidVertices[0][1] = t + randomShift;
	asteroidVertices[0][2] = 0;

	randomShift = GameController::getRandomBetween(MIN_SHIFT, MAX_SHIFT);
	asteroidVertices[1][0] = 1 + randomShift;
	asteroidVertices[1][1] = t + randomShift;
	asteroidVertices[1][2] = 0;

	randomShift = GameController::getRandomBetween(MIN_SHIFT, MAX_SHIFT);
	asteroidVertices[2][0] = -1 + randomShift;
	asteroidVertices[2][1] = -t + randomShift;
	asteroidVertices[2][2] = 0;

	randomShift = GameController::getRandomBetween(MIN_SHIFT, MAX_SHIFT);
	asteroidVertices[3][0] = 1 + randomShift;
	asteroidVertices[3][1] = -t + randomShift;
	asteroidVertices[3][2] = 0;
	
	// -----

	randomShift = GameController::getRandomBetween(MIN_SHIFT, MAX_SHIFT);
	asteroidVertices[4][0] = 0;
	asteroidVertices[4][1] = -1 + randomShift;
	asteroidVertices[4][2] = t + randomShift;

	randomShift = GameController::getRandomBetween(MIN_SHIFT, MAX_SHIFT);
	asteroidVertices[5][0] = 0;
	asteroidVertices[5][1] = 1 + randomShift;
	asteroidVertices[5][2] = t + randomShift;

	randomShift = GameController::getRandomBetween(MIN_SHIFT, MAX_SHIFT);
	asteroidVertices[6][0] = 0;
	asteroidVertices[6][1] = -1 + randomShift;
	asteroidVertices[6][2] = -t + randomShift;

	randomShift = GameController::getRandomBetween(MIN_SHIFT, MAX_SHIFT);
	asteroidVertices[7][0] = 0;
	asteroidVertices[7][1] = 1 + randomShift;
	asteroidVertices[7][2] = -t + randomShift;

	// -----

	randomShift = GameController::getRandomBetween(MIN_SHIFT, MAX_SHIFT);
	asteroidVertices[8][0] = t + randomShift;
	asteroidVertices[8][1] = 0;
	asteroidVertices[8][2] = -1 + randomShift;

	randomShift = GameController::getRandomBetween(MIN_SHIFT, MAX_SHIFT);
	asteroidVertices[9][0] = t + randomShift;
	asteroidVertices[9][1] = 0;
	asteroidVertices[9][2] = 1 + randomShift;

	randomShift = GameController::getRandomBetween(MIN_SHIFT, MAX_SHIFT);
	asteroidVertices[10][0] = -t + randomShift;
	asteroidVertices[10][1] = 0;
	asteroidVertices[10][2] = -1 + randomShift;

	randomShift = GameController::getRandomBetween(MIN_SHIFT, MAX_SHIFT);
	asteroidVertices[11][0] = -t + randomShift;
	asteroidVertices[11][1] = 0;
	asteroidVertices[11][2] = 1 + randomShift;
}

void Asteroid::getNormal(float n[], float v0[], float v1[], float v2[])
{
	// Distance between points on the plane.
	float d0[3] = { v1[0] - v0[0], v1[1] - v0[1], v1[2] - v0[2] };
	float d1[3] = { v2[0] - v1[0], v2[1] - v1[1], v2[2] - v1[2] };

	// Cross product.
	float cross[3] =
	{
		d0[1] * d1[2] - d0[2] * d1[1],
		d0[2] * d1[0] - d0[0] * d1[2],
		d0[0] * d1[1] - d0[1] * d1[0]
	};

	// Magnitude of cross product vector.
	float cross_mag = sqrtf((cross[0] * cross[0]) + (cross[1] * cross[1]) + (cross[2] * cross[2]));

	// Normal to the plane of v0, v1, v2.
	n[0] = cross[0] / cross_mag;
	n[1] = cross[1] / cross_mag;
	n[2] = cross[2] / cross_mag;
}

bool Asteroid::collidesWith(GameObject* other)
{
	if (Bullet* b = dynamic_cast<Bullet*>(other))
	{
		return GameController::collided(this, b);
	}
	else if (Enemy* e = dynamic_cast <Enemy*>(other))
	{
		return GameController::collided(this, e);
	}
	else if (Player* p = dynamic_cast<Player*>(other))
	{
		if (p->isInvulnerable() == false)
		{
			return GameController::collided(p, this);
		}
	}

	return false;
}

void Asteroid::handleCollision(GameObject* other)
{
	if (dynamic_cast<Bullet*>(other) || dynamic_cast<Enemy*>(other) || dynamic_cast<Player*>(other))
	{
		if (Bullet* b = dynamic_cast<Bullet*>(other))
		{
			if (b->shotByPlayer() == true)
			{
				wasShot = true;
			}
		}

		// Could check x, y, or z; they should be the same.
		if (scaling[0] > SMALL)
		{
			// Split into two half-sized asteroids.
			for (int i = 0; i < SPLIT; i++)
			{
				Asteroid* a = new Asteroid(translation[0], translation[1], translation[2]);
				a->scale(scaling[0] * .5f, scaling[1] * .5f, scaling[2] * .5f);
				GameController::addObject(a);
			}
		}

		// EXPLOOOOSIIIONNNNNSSS!!!!!1
		int numParticles = GameController::getRandomBetween((int)(MIN_PARTICLES / scaling[0]), (int)(MAX_PARTICLES / scaling[0]));
		for (int i = 0; i < numParticles; i++)
		{
			Particle* p = new Particle(translation[0], translation[1], translation[2], material[0], material[1], material[2]);
			GameController::addObject(p);
		}

		isExpired = true;
	}
}

void Asteroid::update(void)
{
	// Apply velocity and rotation.
	translate(velocity[0], velocity[1], velocity[2]);
	rotate(xSpin, ySpin, zSpin);
}

void Asteroid::draw(void)
{
	// Model view (since we're drawing something specific)
	glMatrixMode(GL_MODELVIEW);
	// Save glLookAt transformation (I*cam)
	glPushMatrix();

	// Apply current translation to the asteroid.
	glTranslatef(translation[0], translation[1], translation[2]);
	// Apply current orientation to the asteroid.
	glMultMatrixf(orientation);
	// Scale the asteroid.
	glScalef(scaling[0], scaling[1], scaling[2]);

	// Set material.
	glMaterialfv(GL_FRONT, GL_AMBIENT_AND_DIFFUSE, material);

	// Plot out the vertices in counterclockwise order, with normals.
	float n[3];

	glBegin(GL_TRIANGLES);
	getNormal(n, asteroidVertices[0], asteroidVertices[11], asteroidVertices[5]);
	glNormal3fv(n);
	glVertex3fv(asteroidVertices[0]);
	glVertex3fv(asteroidVertices[11]);
	glVertex3fv(asteroidVertices[5]);
	glEnd();

	glBegin(GL_TRIANGLES);
	getNormal(n, asteroidVertices[0], asteroidVertices[5], asteroidVertices[1]);
	glNormal3fv(n);
	glVertex3fv(asteroidVertices[0]);
	glVertex3fv(asteroidVertices[5]);
	glVertex3fv(asteroidVertices[1]);
	glEnd();

	glBegin(GL_TRIANGLES);
	getNormal(n, asteroidVertices[0], asteroidVertices[1], asteroidVertices[7]);
	glNormal3fv(n);
	glVertex3fv(asteroidVertices[0]);
	glVertex3fv(asteroidVertices[1]);
	glVertex3fv(asteroidVertices[7]);
	glEnd();

	glBegin(GL_TRIANGLES);
	getNormal(n, asteroidVertices[0], asteroidVertices[7], asteroidVertices[10]);
	glNormal3fv(n);
	glVertex3fv(asteroidVertices[0]);
	glVertex3fv(asteroidVertices[7]);
	glVertex3fv(asteroidVertices[10]);
	glEnd();

	glBegin(GL_TRIANGLES);
	getNormal(n, asteroidVertices[0], asteroidVertices[10], asteroidVertices[11]);
	glNormal3fv(n);
	glVertex3fv(asteroidVertices[0]);
	glVertex3fv(asteroidVertices[10]);
	glVertex3fv(asteroidVertices[11]);
	glEnd();

	// -----

	glBegin(GL_TRIANGLES);
	getNormal(n, asteroidVertices[1], asteroidVertices[5], asteroidVertices[9]);
	glNormal3fv(n);
	glVertex3fv(asteroidVertices[1]);
	glVertex3fv(asteroidVertices[5]);
	glVertex3fv(asteroidVertices[9]);
	glEnd();

	glBegin(GL_TRIANGLES);
	getNormal(n, asteroidVertices[5], asteroidVertices[11], asteroidVertices[4]);
	glNormal3fv(n);
	glVertex3fv(asteroidVertices[5]);
	glVertex3fv(asteroidVertices[11]);
	glVertex3fv(asteroidVertices[4]);
	glEnd();

	glBegin(GL_TRIANGLES);
	getNormal(n, asteroidVertices[11], asteroidVertices[10], asteroidVertices[2]);
	glNormal3fv(n);
	glVertex3fv(asteroidVertices[11]);
	glVertex3fv(asteroidVertices[10]);
	glVertex3fv(asteroidVertices[2]);
	glEnd();

	glBegin(GL_TRIANGLES);
	getNormal(n, asteroidVertices[10], asteroidVertices[7], asteroidVertices[6]);
	glNormal3fv(n);
	glVertex3fv(asteroidVertices[10]);
	glVertex3fv(asteroidVertices[7]);
	glVertex3fv(asteroidVertices[6]);
	glEnd();

	glBegin(GL_TRIANGLES);
	getNormal(n, asteroidVertices[7], asteroidVertices[1], asteroidVertices[8]);
	glNormal3fv(n);
	glVertex3fv(asteroidVertices[7]);
	glVertex3fv(asteroidVertices[1]);
	glVertex3fv(asteroidVertices[8]);
	glEnd();

	// -----

	glBegin(GL_TRIANGLES);
	getNormal(n, asteroidVertices[3], asteroidVertices[9], asteroidVertices[4]);
	glNormal3fv(n);
	glVertex3fv(asteroidVertices[3]);
	glVertex3fv(asteroidVertices[9]);
	glVertex3fv(asteroidVertices[4]);
	glEnd();

	glBegin(GL_TRIANGLES);
	getNormal(n, asteroidVertices[3], asteroidVertices[4], asteroidVertices[2]);
	glNormal3fv(n);
	glVertex3fv(asteroidVertices[3]);
	glVertex3fv(asteroidVertices[4]);
	glVertex3fv(asteroidVertices[2]);
	glEnd();

	glBegin(GL_TRIANGLES);
	getNormal(n, asteroidVertices[3], asteroidVertices[2], asteroidVertices[6]);
	glNormal3fv(n);
	glVertex3fv(asteroidVertices[3]);
	glVertex3fv(asteroidVertices[2]);
	glVertex3fv(asteroidVertices[6]);
	glEnd();

	glBegin(GL_TRIANGLES);
	getNormal(n, asteroidVertices[3], asteroidVertices[6], asteroidVertices[8]);
	glNormal3fv(n);
	glVertex3fv(asteroidVertices[3]);
	glVertex3fv(asteroidVertices[6]);
	glVertex3fv(asteroidVertices[8]);
	glEnd();

	glBegin(GL_TRIANGLES);
	getNormal(n, asteroidVertices[3], asteroidVertices[8], asteroidVertices[9]);
	glNormal3fv(n);
	glVertex3fv(asteroidVertices[3]);
	glVertex3fv(asteroidVertices[8]);
	glVertex3fv(asteroidVertices[9]);
	glEnd();

	// -----

	glBegin(GL_TRIANGLES);
	getNormal(n, asteroidVertices[4], asteroidVertices[9], asteroidVertices[5]);
	glNormal3fv(n);
	glVertex3fv(asteroidVertices[4]);
	glVertex3fv(asteroidVertices[9]);
	glVertex3fv(asteroidVertices[5]);
	glEnd();

	glBegin(GL_TRIANGLES);
	getNormal(n, asteroidVertices[2], asteroidVertices[4], asteroidVertices[11]);
	glNormal3fv(n);
	glVertex3fv(asteroidVertices[2]);
	glVertex3fv(asteroidVertices[4]);
	glVertex3fv(asteroidVertices[11]);
	glEnd();

	glBegin(GL_TRIANGLES);
	getNormal(n, asteroidVertices[6], asteroidVertices[2], asteroidVertices[10]);
	glNormal3fv(n);
	glVertex3fv(asteroidVertices[6]);
	glVertex3fv(asteroidVertices[2]);
	glVertex3fv(asteroidVertices[10]);
	glEnd();

	glBegin(GL_TRIANGLES);
	getNormal(n, asteroidVertices[8], asteroidVertices[6], asteroidVertices[7]);
	glNormal3fv(n);
	glVertex3fv(asteroidVertices[8]);
	glVertex3fv(asteroidVertices[6]);
	glVertex3fv(asteroidVertices[7]);
	glEnd();

	glBegin(GL_TRIANGLES);
	getNormal(n, asteroidVertices[9], asteroidVertices[8], asteroidVertices[1]);
	glNormal3fv(n);
	glVertex3fv(asteroidVertices[9]);
	glVertex3fv(asteroidVertices[8]);
	glVertex3fv(asteroidVertices[1]);
	glEnd();

	// Clean up after ourselves.
	glPopMatrix();

	// Clear garbage.
	glFlush();
}