#ifndef GLOBALS_H
#define GLOBALS_H

// General
static const float PI = 3.1415926535f;
static const float RGB_MAX = 255.0f;
static const float ROOM_SIZE = 25.0f;

// Lives.
static const int START_LIVES = 3;
static const int BONUS_LIFE_POINTS = 10000;

// Bullets.
static const int MAX_BULLETS = 4;

// Asteroids.
static const int POINTS_PER_ASTEROID = 7;
static const int VAL_ASTEROID_SMALL = 100;
static const int VAL_ASTEROID_MEDIUM = 50;
static const int VAL_ASTEROID_LARGE = 20;
static const int START_ASTEROIDS = 2;
static const int MAX_ASTEROIDS = 6;

// Enemies
static const int VAL_ENEMY = 200;
static const int MIN_ENEMY_SPAWN = 10000;
static const int MAX_ENEMY_SPAWN = 15000;

#endif