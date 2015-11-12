#ifndef PACMAN_H
#define PACMAN_H

#include "defs.h"
#include "proto.h"

void movePlayer(Player*);
void checkEaten(void);
void changeSpeed(Player*,uint8_t);
void enemyTick(Player*);

void restoreDot(uint8_t,uint8_t);
void refreshPowerDots(void);
boolean intersectsFruit(Player*);
boolean isPixel(uint8_t,uint8_t);
boolean isPowerPixel(uint8_t,uint8_t);
void addExtraLife(void);

void playMatrixman(void);


Player* getMyGuy();
Player* getEnemy1();
Player* getEnemy2();
Player* getEnemy3();
Player* getEnemy4();
Fruit* getFruit();
uint16_t getLevel();
uint8_t getLives();
uint32_t getScore();
uint16_t getEatNextEnemyScore();
#endif //PACMAN_H
