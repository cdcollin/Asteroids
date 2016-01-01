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

#include "Bullet.h"
#include "Globals.h"
#include "GameController.h"
#include "Asteroid.h"

const int LIFESPAN = 1000;

extern int delta_time;

Bullet::Bullet(bool ps)
{
	setupCommon(ps);

	translation = new float[3]
	{
		0, 0, 0
	};

	velocity = new float[3]
	{
		0, 0, 0
	};
}

Bullet::Bullet(bool ps, float x, float y, float z,
	float xVel, float yVel, float zVel)
{
	setupCommon(ps);

	// Starting pos (in front of ship).
	translation = new float[3]
	{
		x, y, z
	};

	// Apply velocity.
	velocity = new float[3]
	{
		xVel, yVel, zVel
	};
}

void Bullet::setupCommon(bool ps)
{
	playerShot = ps;

	material = new float[4]
	{
		136.0f / RGB_MAX, 222.0f / RGB_MAX, 235.0f / RGB_MAX, 1
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
}

bool Bullet::shotByPlayer()
{
	return playerShot;
}

bool Bullet::collidesWith(GameObject* other)
{
	if (Asteroid* a = dynamic_cast<Asteroid*>(other))
	{
		return GameController::collided(a, this);
	}
	else if (Enemy* e = dynamic_cast<Enemy*>(other))
	{
		if (playerShot == true)
		{
			return GameController::collided(e, this);
		}
	}
	if (Player* p = dynamic_cast<Player*>(other))
	{
		if (p->isInvulnerable() == false && playerShot == false)
		{
			return GameController::collided(p, this);
		}
	}
	
	return false;
}

void Bullet::handleCollision(GameObject* other)
{
	isExpired = true;
}

void Bullet::update(void)
{
	timer += delta_time;
	if (timer > LIFESPAN)
	{
		isExpired = true;
	}

	// Apply bullet velocity.
	translate(velocity[0], velocity[1], velocity[2]);
}

void Bullet::draw(void)
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

	// The bullet is just a tiny sphere.
	glPushMatrix();
	{
		glutSolidSphere(.1f, 10, 5);
	}
	glPopMatrix();

	// Clean up after ourselves.
	glPopMatrix();

	// Clear garbage.
	glFlush();
}