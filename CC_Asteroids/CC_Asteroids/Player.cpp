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

#include "Player.h"
#include "Globals.h"
#include "GameController.h"
#include "Bullet.h"
#include "Particle.h"

// Ship size.
const float SHIP_LENGTH = 1.5f;
const float RADIUS = .75f;

// Movement.
const float TURN_VAL = .03f;
const float ACCEL_VAL = .005f;
const float DECEL_VAL = .002f;
const float MAX_SPEED = .25f;

// Spawn.
const int INVULN_TIME = 3000;

// Combat.
const int SHOOT_COOLDOWN = 250;
const float BULLET_SPEED = .3f;
const int MIN_PARTICLES = 7;
const int MAX_PARTICLES = 10;

extern int delta_time;

Player::Player()
{
	material = new float[4]
	{
		240.0f / RGB_MAX, 114.0f / RGB_MAX, 3.0f / RGB_MAX, 1
	};

	boostMaterial = new float[4]
	{
		240.0f / RGB_MAX, 44.0f / RGB_MAX, 13.0f / RGB_MAX, 1
	};

	translation = new float[3]
	{
		0, 0, 0
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

	// Facing +y initially.
	forward = new float[3]
	{
		0, 1, 0
	};

	velocity = new float[3]
	{
		0, 0, 0
	};

	rotation = PI;
	rotation /= 2.0f;
}

bool Player::isInvulnerable()
{
	return invulnerable;
}

float Player::getRadius()
{
	// Should always be same x and y scale.
	return scaling[0] * 1.5f;
}

void Player::turnLeft()
{
	rotate(0, 0, -TURN_VAL);
	rotation -= TURN_VAL;

	forward[0] = -cos(rotation);
	forward[1] = sin(rotation);
}

void Player::turnRight()
{
	rotate(0, 0, TURN_VAL);
	rotation += TURN_VAL;

	forward[0] = -cos(rotation);
	forward[1] = sin(rotation);
}

void Player::accelerate()
{
	velocity[0] += forward[0] * ACCEL_VAL;
	velocity[1] += forward[1] * ACCEL_VAL;

	// Cap max speed.
	float speed = sqrtf((velocity[0] * velocity[0]) + (velocity[1] * velocity[1]));
	if (speed > MAX_SPEED)
	{
		velocity[0] = (velocity[0] / speed) * MAX_SPEED;
		velocity[1] = (velocity[1] / speed) * MAX_SPEED;
	}

	drawAccelerating = true;
}

void Player::shoot()
{
	if (canShoot)
	{
		// Give the bullet its starting point and velocity.
		float front[2];
		front[0] = translation[0] + (forward[0] * SHIP_LENGTH);
		front[1] = translation[1] + (forward[1] * SHIP_LENGTH);
		float vShot[2];
		vShot[0] = forward[0] * BULLET_SPEED;
		vShot[1] = forward[1] * BULLET_SPEED;

		Bullet* b = new Bullet(true, front[0], front[1], 0, vShot[0], vShot[1], 0);
		GameController::addObject(b);

		canShoot = false;
	}
}

bool Player::collidesWith(GameObject* other)
{
	if (invulnerable == false)
	{
		if (Asteroid* a = dynamic_cast<Asteroid*>(other))
		{
			return GameController::collided(this, a);
		}
		else if (Enemy* e = dynamic_cast<Enemy*>(other))
		{
			return GameController::collided(this, e);
		}
		else if (Bullet* b = dynamic_cast<Bullet*>(other))
		{
			if (b->shotByPlayer() == false)
			{
				return GameController::collided(this, b);
			}
		}
	}

	return false;
}

void Player::handleCollision(GameObject* other)
{
	if (dynamic_cast<Asteroid*>(other) || dynamic_cast<Enemy*>(other) || dynamic_cast<Bullet*>(other))
	{
		// EXPLOOOOSIIIONNNNNSSS!!!!!1
		int numParticles = GameController::getRandomBetween(MIN_PARTICLES, MAX_PARTICLES);
		for (int i = 0; i < numParticles; i++)
		{
			Particle* p = new Particle(translation[0], translation[1], translation[2], material[0], material[1], material[2]);
			GameController::addObject(p);
		}

		isExpired = true;
	}
}

void Player::update(void)
{
	// Update spawn invulnerability.
	if (invulnerable == true)
	{
		timeSinceSpawn += delta_time;
		if (timeSinceSpawn >= INVULN_TIME)
		{
			invulnerable = false;
		}
	}

	// Get velocity direction (normalized vector).
	float speed = sqrtf((velocity[0] * velocity[0]) + (velocity[1] * velocity[1]));
	if (speed > 0)
	{
		// Apply deceleration.
		float vDir[2];
		vDir[0] = velocity[0] / speed;
		vDir[1] = velocity[1] / speed;

		velocity[0] -= vDir[0] * DECEL_VAL;
		velocity[1] -= vDir[1] * DECEL_VAL;
	}

	speed = sqrtf((velocity[0] * velocity[0]) + (velocity[1] * velocity[1]));

	// See if our speed is decreasing since last frame.
	if (speed < lastSpeed)
	{
		drawAccelerating = false;
		drawDecelerating = true;
	}
	lastSpeed = speed;

	// If we're slow enough after deceleration, just stop.
	if (speed < DECEL_VAL)
	{
		velocity[0] = 0;
		velocity[1] = 0;

		drawAccelerating = false;
		drawDecelerating = false;
	}

	// Update position.
	translate(velocity[0], velocity[1], velocity[2]);

	// Update shooting cooldown.
	if (canShoot == false)
	{
		shootTimer += delta_time;
		if (shootTimer >= SHOOT_COOLDOWN)
		{
			shootTimer = 0;
			canShoot = true;
		}
	}
}

void Player::draw(void)
{
	// Model view (since we're drawing something specific)
	glMatrixMode(GL_MODELVIEW);
	// Save glLookAt transformation (I*cam)
	glPushMatrix();

	// Apply current transform.
	glTranslatef(translation[0], translation[1], translation[2]);
	// Apply current orientation to the Player.
	glMultMatrixf(orientation);
	// Scale the Player.
	glScalef(scaling[0], scaling[1], scaling[2]);

	// Set material.
	float mat[4] = { material[0], material[1], material[2], material[3] };
	// If we just spawned, draw light blue.
	if (invulnerable == true)
	{
		mat[0] = 80.0f / RGB_MAX;
		mat[1] = 140.0f / RGB_MAX;
		mat[2] = 200.0f / RGB_MAX;
	}

	glMaterialfv(GL_FRONT, GL_AMBIENT_AND_DIFFUSE, mat);
	glMaterialf(GL_FRONT, GL_SHININESS, 100.0f);

	// Main body
	glPushMatrix();
	{
		glScalef(1, 3, 1);
		glutSolidCube(1);
	}
	glPopMatrix();

	// Cockpit
	glPushMatrix();
	{
		glScalef(.75, 1.25, .25);
		glTranslatef(0, -.5, 2.5);
		glutSolidCube(1);
	}
	glPopMatrix();

	// Wings
	glPushMatrix();
	{
		glScalef(.5, 2, .5);

		glPushMatrix();
		{
			glTranslatef(-1.5, -.25, -.5);
			glutSolidCube(1);
		}
		glPopMatrix();

		glPushMatrix();
		{
			glTranslatef(1.5, -.25, -.5);
			glutSolidCube(1);
		}
		glPopMatrix();

	}
	glPopMatrix();

	// Wing extensions
	glPushMatrix();
	{
		glScalef(.25, 1.5, .25);

		glPushMatrix();
		{
			glTranslatef(-4.5, -.5, -1.25);
			glutSolidCube(1);
		}
		glPopMatrix();

		glPushMatrix();
		{
			glTranslatef(4.5, -.5, -1.25);
			glutSolidCube(1);
		}
		glPopMatrix();

	}
	glPopMatrix();

	// Cannon
	glPushMatrix();
	{
		glTranslatef(0, 2.5, 0);
		glRotatef(90, 1, 0, 0);
		glutSolidCylinder(.1, 1, 6, 1);
	}
	glPopMatrix();

	// Boost.
	if (drawAccelerating || drawDecelerating)
	{
		glPushMatrix();
		{
			glMaterialfv(GL_FRONT, GL_AMBIENT_AND_DIFFUSE, boostMaterial);
			glMaterialf(GL_FRONT, GL_SHININESS, 100.0f);

			glTranslatef(0, -2.5f, 0);
			if (drawAccelerating == true)
			{
				// Left thruster.
				glPushMatrix();
				{
					glTranslatef(-.8f, 0, 0);

					glutSolidSphere(.5f, 10, 5);
					glPushMatrix();
					{
						glTranslatef(0, -1, 0);
						glutSolidSphere(.25f, 10, 5);
					}
					glPopMatrix();
				}
				glPopMatrix();

				// Right thruster.
				glPushMatrix();
				{
					glTranslatef(.8f, 0, 0);

					glutSolidSphere(.5f, 10, 5);
					glPushMatrix();
					{
						glTranslatef(0, -1, 0);
						glutSolidSphere(.25f, 10, 5);
					}
					glPopMatrix();
				}
				glPopMatrix();

			}
			else if (drawDecelerating == true)
			{
				// Left thruster.
				glPushMatrix();
				{
					glTranslatef(-.8f, 0, 0);
					glutSolidSphere(.25f, 10, 5);
				}
				glPopMatrix();
				

				// Right thruster.
				glPushMatrix();
				{
					glTranslatef(.8f, 0, 0);
					glutSolidSphere(.25f, 10, 5);
				}
				glPopMatrix();
			}
		}
		glPopMatrix();
	}
	
	// Clean up after ourselves.
	glPopMatrix();

	// Clear garbage.
	glFlush();
}