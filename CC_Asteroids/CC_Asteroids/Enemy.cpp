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

#include "Enemy.h"
#include "GameController.h"
#include "Particle.h"
#include "Bullet.h"
#include "Player.h"

const int BULLET_DELAY = 1500;
const float BULLET_SPEED = .15f;

const int MIN_VELOCITY_DELAY = 1000;
const int MAX_VELOCITY_DELAY = 1500;

const int MIN_PARTICLES = 10;
const int MAX_PARTICLES = 15;

extern int delta_time;

Enemy::Enemy(std::vector<GameObject*>* objects)
{
	targets = objects;
	changeVelocityDelay = MAX_VELOCITY_DELAY;

	material = new float[4]
	{
		105.0f / RGB_MAX, 180.0f / RGB_MAX, 34.0f / RGB_MAX, 1
	};

	// Start all the way to the left or right, but with random y.
	translation = new float[3]
	{
		-ROOM_SIZE, GameController::getRandomBetween(-ROOM_SIZE, ROOM_SIZE), 0
	};

	// Alien ships have constant x-velocity and periodically-randomized y-velocity.
	velocity = new float[3]
	{
		GameController::getRandomBetween(.05f, .1f), GameController::getRandomBetween(-.1f, .1f), 0
	};

	// Pick a side of the screen to appear on.
	float edge = GameController::getRandomBetween(0.0f, 1.0f);
	movingRight = (edge > .5f);
	if (movingRight == false)
	{
		translation[0] *= -1;
		velocity[0] *= -1;
	}

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

float Enemy::getRadius()
{
	// Any scaling factor should work; it's always the same value as the radius.
	return scaling[0] * 3;
}

int Enemy::getPoints()
{
	if (wasShot == true)
	{
		return VAL_ENEMY;
	}

	return 0;
}

void Enemy::shootAt(GameObject* target)
{
	float direction[2];
	float vShot[2];
	if (dynamic_cast<Asteroid*>(target) || dynamic_cast<Player*>(target))
	{
		// Fire at the target.
		float dX = target->translation[0] - translation[0];
		float dY = target->translation[1] - translation[1];
		float magnitude = sqrt((dX * dX) + (dY * dY));

		direction[0] = dX / magnitude;
		direction[1] = dY / magnitude;
	}
	else
	{
		// Fire in a random direction.
		float dX = GameController::getRandomBetween(-1.0f, 1.0f);
		float dY = GameController::getRandomBetween(-1.0f, 1.0f);
		float magnitude = sqrt((dX * dX) + (dY * dY));

		direction[0] = dX / magnitude;
		direction[1] = dY / magnitude;
	}
	vShot[0] = direction[0] * BULLET_SPEED;
	vShot[1] = direction[1] * BULLET_SPEED;

	Bullet* b = new Bullet(false, translation[0], translation[1], 0, vShot[0], vShot[1], 0);
	GameController::addObject(b);
}

bool Enemy::collidesWith(GameObject* other)
{
	if (Bullet* b = dynamic_cast<Bullet*>(other))
	{
		if (b->shotByPlayer() == true)
		{
			return GameController::collided(this, b);
		}
	}
	else if (Asteroid* a = dynamic_cast<Asteroid*>(other))
	{
		return GameController::collided(a, this);
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

void Enemy::handleCollision(GameObject* other)
{
	if (dynamic_cast<Bullet*>(other) || dynamic_cast<Asteroid*>(other) || dynamic_cast<Player*>(other))
	{
		if (dynamic_cast<Bullet*>(other))
		{
			wasShot = true;
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

void Enemy::update(void)
{
	bulletTimer += delta_time;
	changeVelocityTimer += delta_time;

	// Apply x velocity.
	translate(velocity[0], velocity[1], velocity[2]);

	// Change y velocity occasionally.
	if (changeVelocityTimer > changeVelocityDelay)
	{
		velocity[1] = velocity[1] = GameController::getRandomBetween(-.1f, .1f);

		changeVelocityTimer = 0;
		changeVelocityDelay = GameController::getRandomBetween(MIN_VELOCITY_DELAY, MAX_VELOCITY_DELAY);
	}

	// Shoot at stuff.
	if (bulletTimer > BULLET_DELAY)
	{
		GameObject* target = targets->at(GameController::getRandomBetween(0, targets->size() - 1));
		shootAt(target);

		// Reload.
		bulletTimer = 0;
	}
	

	// Disappear if we've reached the other side of the room.
	if (movingRight)
	{
		if (translation[0] > ROOM_SIZE - 1)
		{
			isExpired = true;
		}
	}
	else
	{
		if (translation[0] < -ROOM_SIZE + 1)
		{
			isExpired = true;
		}
	}
}

void Enemy::draw(void)
{
	// Model view (since we're drawing something specific)
	glMatrixMode(GL_MODELVIEW);
	// Save glLookAt transformation (I*cam)
	glPushMatrix();

	// Scale the Enemy.
	glScalef(scaling[0], scaling[1], scaling[2]);
	// Apply current orientation to the Enemy.
	glMultMatrixf(orientation);
	// Apply current transform.
	glTranslatef(translation[0], translation[1], translation[2]);
	
	// Set material.
	glMaterialfv(GL_FRONT, GL_AMBIENT_AND_DIFFUSE, material);

	// Main body
	glPushMatrix();
	{
		glScalef(1, 2, 1);
		glutSolidCube(1);
	}
	glPopMatrix();

	// Sides
	glPushMatrix();
	{
		glScalef(.25, 1.5, .25);

		glPushMatrix();
		{
			glTranslatef(-2.5, 0, 0);
			glutSolidCube(1);
		}
		glPopMatrix();

		glPushMatrix();
		{
			glTranslatef(2.5, 0, 0);
			glutSolidCube(1);
		}
		glPopMatrix();

	}
	glPopMatrix();

	// Hovering stuff
	glPushMatrix();
	{
		glScalef(.5, 1.5, .5);

		// Left side
		glPushMatrix();
		{
			glTranslatef(-3, 1, 0);
			glRotatef(30, 0, 1, 1);
			glutSolidCube(1);
		}
		glPopMatrix();

		glPushMatrix();
		{
			glTranslatef(-3, -1, 0);
			glRotatef(-30, 0, 1, 1);
			glutSolidCube(1);
		}
		glPopMatrix();

		// Right side
		glPushMatrix();
		{
			glTranslatef(3, 1, 0);
			glRotatef(-30, 0, 1, 1);
			glutSolidCube(1);
		}
		glPopMatrix();

		glPushMatrix();
		{
			glTranslatef(3, -1, 0);
			glRotatef(30, 0, 1, 1);
			glutSolidCube(1);
		}
		glPopMatrix();

	}
	glPopMatrix();

	// Cannon holder
	glPushMatrix();
	{
		glScalef(.5f, .5f, .25f);
		glTranslatef(0, 0, -2.5);
		glutSolidCube(1);
	}
	glPopMatrix();

	// Cannon
	glPushMatrix();
	{
		glTranslatef(0, 0, -.75);
		glutSolidSphere(.2, 10, 5);
	}
	glPopMatrix();
	
	// Clean up after ourselves.
	glPopMatrix();

	// Clear garbage.
	glFlush();
}