#ifndef GAMECONTROLLER_H
#define GAMECONTROLLER_H

#include "GameObject.h"
#include "Player.h"
#include "Asteroid.h"
#include "Bullet.h"
#include "Enemy.h"

class GameController
{
private:
	// Helper functions.
	void reset(void);
	void handleControls(void);
	void drawUI(void);
	void drawScore(void);
	void drawExtraLives(void);
	void drawShip(void);

	// Gameplay.
	void spawnAsteroidWave(void);
	void spawnEnemy(void);
public:
	GameController();
	void setup(void);
	void update(void);
	void draw(void);

	// Utility.
	static void addObject(GameObject*);
	static int getRandomBetween(int, int);
	static float getRandomBetween(float, float);

	// Collisions.
	static bool collided(Player*, Asteroid*);
	static bool collided(Player*, Enemy*);
	static bool collided(Player*, Bullet*);
	static bool collided(Asteroid*, Enemy*);
	static bool collided(Asteroid*, Bullet*);
	static bool collided(Enemy*, Bullet*);

	// Controls.
	void handle_menu(int);
	void handle_keyboard(unsigned char, int, int);
	void handle_special(int, int, int);
	void handle_keyboard_up(unsigned char, int, int);
	void handle_special_up(int, int, int);
};

#endif