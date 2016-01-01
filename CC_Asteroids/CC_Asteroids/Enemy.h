#ifndef ENEMY_H
#define ENEMY_H

#include <vector>
#include "Globals.h"
#include "GameObject.h"

class Enemy : public GameObject
{
	// Things we can shoot at.
	std::vector<GameObject*>* targets;

	// Only award points for our demise if we were shot by the player.
	bool wasShot = false;
	// When can we shoot next?
	int bulletTimer = 0;
	// Which side of the screen will we disappear on?
	bool movingRight;
	// When will we change our y-velocity next?
	int changeVelocityTimer = 0;
	int changeVelocityDelay;
public:
	Enemy(std::vector<GameObject*>*);
	float getRadius(void);
	int getPoints(void);

	void shootAt(GameObject*);
	bool collidesWith(GameObject*);
	void handleCollision(GameObject*);
	void update(void);
	void draw(void);
};

#endif