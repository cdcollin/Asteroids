#ifndef PLAYER_H
#define PLAYER_H

#include "GameObject.h"
#include "Bullet.h"

class Player : public GameObject
{
	float *boostMaterial;
	float *forward;
	float rotation = 0; // offset from original.

	// Spawning info.
	int timeSinceSpawn = 0;
	bool invulnerable = true;

	// Shooting info.
	bool canShoot = true;
	int shootTimer = 0;

	// Booster draw info.
	bool drawAccelerating = false;
	bool drawDecelerating = false;
	float lastSpeed = 0;
public:
	Player();
	bool isInvulnerable(void);
	float getRadius(void);

	void turnLeft(void);
	void turnRight(void);
	void accelerate(void);
	void shoot(void);
	bool collidesWith(GameObject*);
	void handleCollision(GameObject*);
	void update(void);
	void draw(void);
};

#endif