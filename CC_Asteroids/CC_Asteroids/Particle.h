#ifndef PARTICLE_H
#define PARTICLE_H

#include "GameObject.h"

class Particle : public GameObject
{
	int timer = 0;

	// Constructor helper.
	void setupCommon(void);
public:
	Particle();
	// Initial position and color.
	Particle(float, float, float, float, float, float);

	void update(void);
	void draw(void);
};

#endif