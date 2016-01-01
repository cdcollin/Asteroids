#ifndef GAMEOBJECT_H
#define GAMEOBJECT_H

#include <iostream>

class GameObject
{
public:
	bool isExpired = false;

	// Materials (just one for now).
	float *material; // 4x1 matrix.

	// Transform
	float *translation; // 3x1 matrix
	float *orientation; // 4x4 matrix
	float *scaling; // 3x1 matrix
	
	// Phyics/movement
	float *velocity; // 3x1 matrix
	float xSpin, ySpin, zSpin;

	virtual void translate(float, float, float);
	virtual void rotate(float, float, float);
	virtual void scale(float, float, float);

	virtual void generateVertices(void) {}
	virtual bool collidesWith(GameObject*) { return false; }
	virtual void handleCollision(GameObject*) {}
	virtual void update(void) {}
	virtual void draw(void) { std::cout << "Your object needs to override GameObject::draw!"; }
};

#endif