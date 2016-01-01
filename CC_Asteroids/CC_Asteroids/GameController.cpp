#ifdef _M_IX86
#include <windows.h>
#else
#include <stream.h>
#endif

#include <iostream>
#include <string>
#include <sstream>
#include <vector>
#include <map>
#include <GL/gl.h>
#include <GL/glu.h>
#include <GL/freeglut.h>
#include <math.h>
#include <time.h>

#include "GameController.h"
#include "Globals.h"
#include "Player.h"
#include "Bullet.h"
#include "Asteroid.h"
#include "Enemy.h"

// GameObjects.
std::vector<GameObject*> objects;
Player* player;

// Control.
extern int delta_time;
std::map<std::string, bool> keys;

// Lives.
int numLives;

// Asteroids.
unsigned int asteroidLevel;
int numAsteroids;

// Enemies.
bool canSpawnEnemy;
int enemySpawnTimer;
int enemySpawnWait;

// Bullets.
int numBullets;

// Score.
int score;
int bonusLifeScore;

// UI.
float uiMaterial[4] = { 255.0f, 255.0f, 255.0f, 255.0f };

GameController::GameController()
{
	// Seed RNG based on time of creation.
	srand((unsigned)time(NULL));
}

/*
Instantiate the player and start us off with a few asteroids.
*/
void GameController::setup()
{
	// Init controls.
	keys["x"] = false;
	keys["z"] = false;
	keys["left"] = false;
	keys["right"] = false;

	// Reset local variables.
	numLives = START_LIVES;
	asteroidLevel = START_ASTEROIDS;
	numAsteroids = 0;
	canSpawnEnemy = true;
	enemySpawnTimer = 0;
	enemySpawnWait = MIN_ENEMY_SPAWN;
	numBullets = 0;
	score = 0;
	bonusLifeScore = BONUS_LIFE_POINTS;

	player = new Player();
	player->scale(.5f, .5f, .5f);

	objects.push_back(player);

	spawnAsteroidWave();
}

/*
Update all game objects.
*/
void GameController::update()
{
	handleControls();

	// Update game objects.
	unsigned int i;
	for (i = 0; i < objects.size(); i++)
	{
		GameObject* go = objects.at(i);
		if (go != NULL)
		{
			go->update();
			
			// Check for collisions.
			unsigned int j;
			for (j = 0; j < objects.size(); j++)
			{
				GameObject* other = objects.at(j);
				if (go != other)
				{
					if (go->collidesWith(other))
					{
						go->handleCollision(other);
						other->handleCollision(go);
					}
				}
			}

			// See if the object should be deleted.
			if (go->isExpired)
			{
				if (dynamic_cast<Bullet*>(go)) {
					numBullets--;
				}
				else if (dynamic_cast<Asteroid*>(go)) {
					numAsteroids--;
					Asteroid* a = dynamic_cast<Asteroid*>(go);
					score += a->getPoints();

					// Wave cleared! Spawn the next one.
					if (numAsteroids < 1)
					{
						spawnAsteroidWave();
					}
				}
				else if (dynamic_cast<Enemy*>(go)) {
					Enemy* e = dynamic_cast<Enemy*>(go);
					score += e->getPoints();

					// We can start waiting for a new enemy to appear.
					canSpawnEnemy = true;
					enemySpawnTimer = 0;
				}
				else if (dynamic_cast<Player*>(go)) {
					numLives--;
					if (numLives > 0)
					{
						player = new Player();
						player->scale(.5f, .5f, .5f);
						objects.push_back(player);
					}
					else
					{
						// Build end game string.
						std::ostringstream oss;
						oss << "You scored " << score << " points, then died horribly.";
						std::string msg = oss.str();

						// Show string. When they press OK, restart the game.
						MessageBoxA(0, msg.c_str(), "You lost!", 0);
						reset();
						return;
					}
				}
				
				objects.erase(objects.begin() + i);
				delete(go);
				i--;
			}

			// See if we've earned another life.
			if (score >= bonusLifeScore)
			{
				numLives++;
				bonusLifeScore += BONUS_LIFE_POINTS;
			}
		}
	}

	// Check for enemy spawn. TODO: Move to spawner object.
	if (canSpawnEnemy)
	{
		enemySpawnTimer += delta_time;

		if (enemySpawnTimer > enemySpawnWait)
		{
			spawnEnemy();

			// Randomize delay for next enemy.
			enemySpawnWait = getRandomBetween(MIN_ENEMY_SPAWN, MAX_ENEMY_SPAWN);

			// Only have one enemy onscreen at a time.
			canSpawnEnemy = false;
		}
	}
}

/*
Draw all game objects.
*/
void GameController::draw()
{
	unsigned int i;
	for (i = 0; i < objects.size(); i++)
	{
		if (objects.at(i) != NULL)
		{
			objects.at(i)->draw();
		}
	}

	drawUI();
}

void GameController::drawUI()
{
	drawScore();
	drawExtraLives();
}

void GameController::drawScore()
{
	// Model view (since we're drawing something specific)
	glMatrixMode(GL_MODELVIEW);
	// Save glLookAt transformation (I*cam)
	glPushMatrix();

	int quotient = score;
	int scoreDigits = 0;
	while (quotient != 0)
	{
		scoreDigits++;
		quotient = quotient / 10;
	}

	// Draw location.
	glTranslatef(17.5f - .5f * (scoreDigits - 1), 23.0f, 0);
	glScalef(.01f, .01f, .01f);

	// Set material.
	glMaterialfv(GL_FRONT, GL_AMBIENT_AND_DIFFUSE, uiMaterial);
	glMaterialf(GL_FRONT, GL_SHININESS, 100.0f);

	// Build score string.
	std::ostringstream oss;
	oss << "Score: " << score;
	std::string scoreStr = oss.str();
	unsigned char* uglyWorkaround = new unsigned char[scoreStr.length() + 1];
	strcpy((char *)uglyWorkaround, scoreStr.c_str());

	glutStrokeString(GLUT_STROKE_ROMAN, uglyWorkaround);

	delete(uglyWorkaround);

	// Clean up after ourselves.
	glPopMatrix();

	// Clear garbage.
	glFlush();
}

void GameController::drawExtraLives()
{
	// Model view (since we're drawing something specific)
	glMatrixMode(GL_MODELVIEW);
	// Save glLookAt transformation (I*cam)
	glPushMatrix();

	// Draw location.
	glTranslatef(-24.0f, 23.0f, 0);
	glScalef(.01f, .01f, .01f);

	// Set material.
	glMaterialfv(GL_FRONT, GL_AMBIENT_AND_DIFFUSE, uiMaterial);
	glMaterialf(GL_FRONT, GL_SHININESS, 100.0f);

	glutStrokeString(GLUT_STROKE_ROMAN, (unsigned char*)"Lives:");

	// Draw a ship for each extra life we've got.
	int i;
	for (i = 1; i < numLives; i++)
	{
		glPushMatrix();
		{
			glTranslatef(100.0f * i, 35.0f, 0);
			glScalef(30.0f, 30.0f, 30.0f);
			drawShip();
		}
		glPopMatrix();
	}

	// Clean up after ourselves.
	glPopMatrix();

	// Clear garbage.
	glFlush();
}

void GameController::drawShip()
{
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
}

#pragma region Utility

void GameController::reset()
{
	// Clear objects.
	while (objects.size() > 0)
	{
		GameObject* go = objects.at(0);
		objects.erase(objects.begin());
		delete(go);
	}

	// Restart.
	setup();
}

void GameController::addObject(GameObject* go)
{
	objects.push_back(go);
	if (dynamic_cast<Bullet*>(go))
	{
		numBullets++;
	}
}

int GameController::getRandomBetween(int min, int max)
{
	return min + (rand()) / (RAND_MAX / (max - min));
}

float GameController::getRandomBetween(float min, float max)
{
	return min + ((float)rand()) / ((float)RAND_MAX / (max - min));
}

#pragma endregion

#pragma region Collisions

bool GameController::collided(Player* p, Asteroid* a)
{
	// Polygon-circle collision between player and asteroid, respectively.
	float aX = a->translation[0];
	float aY = a->translation[1];

	float pX = p->translation[0];
	float pY = p->translation[1];

	// Euclidean distance = (sx - x)^2 + (sy - y)^2
	float dist = ((aX - pX) * (aX - pX)) + ((aY - pY) * (aY - pY));
	if (dist < (a->getRadius() + p->getRadius()))
	{
		return true;
	}

	return false;
}

bool GameController::collided(Player* p, Enemy* e)
{
	// Polygon-polygon collision between player and enemy, respectively.
	float eX = e->translation[0];
	float eY = e->translation[1];

	float pX = p->translation[0];
	float pY = p->translation[1];

	// Euclidean distance = (sx - x)^2 + (sy - y)^2
	float dist = ((eX - pX) * (eX - pX)) + ((eY - pY) * (eY - pY));
	if (dist < (e->getRadius() + p->getRadius()))
	{
		return true;
	}

	return false;
}

bool GameController::collided(Player* p, Bullet* b)
{
	// Polygon-point collision between player and bullet, respectively.
	float pX = p->translation[0];
	float pY = p->translation[1];

	float bX = b->translation[0];
	float bY = b->translation[1];

	// Euclidean distance = (sx - x)^2 + (sy - y)^2
	float dist = ((pX - bX) * (pX - bX)) + ((pY - bY) * (pY - bY));
	if (dist < p->getRadius())
	{
		return true;
	}

	return false;
}

bool GameController::collided(Asteroid* a, Enemy* e)
{
	// Circle-polygon collision between asteroid and enemy, respectively.
	float aX = a->translation[0];
	float aY = a->translation[1];

	float eX = e->translation[0];
	float eY = e->translation[1];

	// Euclidean distance = (sx - x)^2 + (sy - y)^2
	float dist = ((aX - eX) * (aX - eX)) + ((aY - eY) * (aY - eY));
	if (dist < (a->getRadius() + e->getRadius()))
	{
		return true;
	}

	return false;
}

bool GameController::collided(Asteroid* a, Bullet* b)
{
	// Circle-point collision between asteroid and bullet, respectively.
	float aX = a->translation[0];
	float aY = a->translation[1];

	float bX = b->translation[0];
	float bY = b->translation[1];

	// Euclidean distance = (sx - x)^2 + (sy - y)^2
	float dist = ((aX - bX) * (aX - bX)) + ((aY - bY) * (aY - bY));
	if (dist < a->getRadius())
	{
		return true;
	}

	return false;
}

bool GameController::collided(Enemy* e, Bullet* b)
{
	// Polygon-point collision between enemy and bullet, respectively.
	float eX = e->translation[0];
	float eY = e->translation[1];

	float bX = b->translation[0];
	float bY = b->translation[1];

	// Euclidean distance = (sx - x)^2 + (sy - y)^2
	float dist = ((eX - bX) * (eX - bX)) + ((eY - bY) * (eY - bY));
	if (dist < e->getRadius())
	{
		return true;
	}

	return false;
}

#pragma endregion

#pragma region Controls

/*
Handle standard Alphanumeric keys. We use:
x = accelerates player ship forward.
z = player shoots a super-heated-proto-giga-blast
*/
void GameController::handle_keyboard(unsigned char key, int x, int y)
{
	switch (key)
	{
	case 'x':
		keys["x"] = true;
		break;
	case 'z':
		keys["z"] = true;
		break;
	}
}
void GameController::handle_keyboard_up(unsigned char key, int x, int y)
{
	switch (key)
	{
	case 'x':
		keys["x"] = false;
		break;
	case 'z':
		keys["z"] = false;
		break;
	}
}

/*
Handle special keys. In our case, just the arrow keys:
<- : rotates player ship left.
-> : rotates player ship right.
*/
void GameController::handle_special(int key, int x, int y)
{
	switch (key)
	{
	case GLUT_KEY_LEFT:
		keys["left"] = true;
		break;
	case GLUT_KEY_RIGHT:
		keys["right"] = true;
		break;
	}
}
void GameController::handle_special_up(int key, int x, int y)
{
	switch (key)
	{
	case GLUT_KEY_LEFT:
		keys["left"] = false;
		break;
	case GLUT_KEY_RIGHT:
		keys["right"] = false;
		break;
	}
}

/*
Just closes the application if they picked the one menu option.
*/
void GameController::handle_menu(int x)
{
	switch (x)
	{
	case 0: // New Game.
		reset();
		
		break;
	case 1: // Quit.
		exit(0);
		
		break;
	}
}

/*
Update player based on user inputs.
*/
void GameController::handleControls()
{
	if (keys["x"] == true)
	{
		player->accelerate();
	}
	if (keys["z"] == true && numBullets < MAX_BULLETS)
	{
		player->shoot();
	}
	if (keys["left"] == true)
	{
		player->turnLeft();
	}
	if (keys["right"] == true)
	{
		player->turnRight();
	}
}

#pragma endregion

#pragma region Gameplay

void GameController::spawnAsteroidWave()
{
	unsigned int i;
	for (i = 0; i < asteroidLevel; i++)
	{
		objects.push_back(new Asteroid());
		numAsteroids++;
	}

	// Make life less difficult.
	numAsteroids *= POINTS_PER_ASTEROID;

	// If we clear this wave, we'll start with one more asteroid next time.
	if (asteroidLevel < MAX_ASTEROIDS)
	{
		asteroidLevel++;
	}
}

void GameController::spawnEnemy()
{
	objects.push_back(new Enemy(&objects));
}

#pragma endregion