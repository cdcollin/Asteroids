#ifndef ASTEROID_H
#define ASTEROID_H

#include "GameObject.h"

class Asteroid : public GameObject
{
	// Randomly-perturbed icosahedron = asteroid!
	float **asteroidVertices;
	bool wasShot = false;

	// Constructor helper.
	void setupCommon(void);
public:
	Asteroid(); // init on edge of screen.
	Asteroid(float, float, float); // init with position.
	float getRadius(void);
	int getPoints(void);

	void generateVertices(void);
	void getNormal(float*, float*, float*, float*); // Modifies 3x1 array to contain normal.
	bool collidesWith(GameObject*);
	void handleCollision(GameObject*);
	void update(void);
	void draw(void);
};

#endif