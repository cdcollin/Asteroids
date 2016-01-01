#ifndef BULLET_H
#define BULLET_H

#include "GameObject.h"

class Bullet : public GameObject
{
	// Did the player fire this bullet?
	bool playerShot = false;
	// Our lifespan.
	int timer = 0;

	// Constructor helper.
	void setupCommon(bool);
public:
	Bullet(bool);
	// Initial position and velocity.
	Bullet(bool, float, float, float,
		float, float, float);

	bool shotByPlayer(void);
	bool collidesWith(GameObject*);
	void handleCollision(GameObject*);
	void update(void);
	void draw(void);
};

#endif