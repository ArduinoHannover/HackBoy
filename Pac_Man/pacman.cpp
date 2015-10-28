#include <SPI.h>
#include <Adafruit_GFX.h>
#include <TFT_ILI9163C.h>
#include <gamebox.h>
#include "pacman.h"
#include "proto.h"


#define EATENEMYSCOREBASE   200		//The least you'll get for eating enemy


uint8_t nextDir;			//Stores the newest direction input from user
uint8_t enemyMode;			//SCATTER, CHASE, or FRIGHT
uint32_t dotTracker[36];
uint16_t frightTimer;		//Counts down to end of FRIGHT mode
uint8_t lastBehavior;		//Saves mode before entering FRIGHT
uint16_t dotTimer;			//Countdown release enemies if dots not eaten
uint16_t behaviorTicks;		//Timer for switch from scatter to chase
uint8_t behaviorIndex;		//Index that tracks timer values for mode changes
uint8_t useGlobalDot;		//FALSE = use enemy dot counters, TRUE = use globalDotCounter
uint16_t globalDotCounter;	// after death, release ghosts on dots eaten: 7/17/32

uint16_t level;			//Which level is currently running (zero index)
//uint8_t powerPixelColor;//Used to facilitate flashing of the powerPixels
int8_t lives;          //Remaining extra lives
uint32_t score;         //Keeps score
uint16_t eatNextEnemyScore = EATENEMYSCOREBASE; //Doubles with each enemy eaten

Player myGuy;
Player enemy1;
Player enemy2;
Player enemy3;
Player enemy4;

Fruit fruit;

Player* getMyGuy() {
	return &myGuy;
}
Player* getEnemy1() {
	return &enemy1;
}
Player* getEnemy2() {
	return &enemy2;
}
Player* getEnemy3() {
	return &enemy3;
}
Player* getEnemy4() {
	return &enemy4;
}
Fruit* getFruit() {
	return &fruit;
}
uint16_t getLevel() {
	return level;
}
uint8_t getLives() {
	return lives;
}
uint32_t getScore() {
	return score;
}
uint16_t getEatNextEnemyScore() {
	return eatNextEnemyScore;
}

boolean gameRunning;    //TRUE unless game over: FALSE


void setupLevel() {
	drawBoard(false);
	drawPlayer(&myGuy, false);
	drawLives();
	drawScore();
	getReady();
}

void refreshDotTracker(void) {
	memcpy(dotTracker, dots, 36 * 4);
}

void setupPlayer(Player *pawn, uint8_t newId) {
	pawn->id = newId;
	pawn->x = startingX[pawn->id];
	pawn->y = startingY[pawn->id];
	if (newId) {
		changeSpeed(pawn, SPEEDENEMY);
	}
	else {
		changeSpeed(pawn, SPEEDPLAYER);
	}
	pawn->travelDir = LEFT;
	pawn->color = playerColor[pawn->id];
	pawn->tarX = scatterX[pawn->id];
	pawn->tarY = scatterY[pawn->id];
	pawn->dotCount = 0;
	if (level >= 3) {
		pawn->dotLimit = 0;
	}
	else {
		pawn->dotLimit = dotLimitTable[(level * 5) + pawn->id];
	}
	pawn->inPlay = FALSE;
}

void setupDefaults(void) {
	refreshDotTracker();
	//set initial values for player and enemies
	setupPlayer(&myGuy, 0);
	setupPlayer(&enemy1, 1);
	enemy1.inPlay = TRUE; //Enemy1 always starts inPlay
	setupPlayer(&enemy2, 2);
	setupPlayer(&enemy3, 3);
	setupPlayer(&enemy4, 4);
	enemyMode = SCATTER;
	useGlobalDot = FALSE;
}

void restoreDot(uint8_t x, uint8_t y) {
	displayDots(x, y, false, (millis() / 200) & 1);
}

boolean lastPowerDotState = false;
void refreshPowerDots() {
	if (lastPowerDotState == ((millis() / 200) & 1)) return;
	lastPowerDotState = !lastPowerDotState;
	displayDots(PP1COL, PP1ROW, false, lastPowerDotState);
	displayDots(PP2COL, PP1ROW, false, lastPowerDotState);
	displayDots(PP1COL, PP2ROW, false, lastPowerDotState);
	displayDots(PP2COL, PP2ROW, false, lastPowerDotState);
}

boolean intersectsFruit(Player *pawn) {
	return pawn->y == FRUIT_Y && (pawn->x == FRUIT_X || pawn->x == FRUIT_X + 1);
}

boolean isPixel(uint8_t x, uint8_t y) {
	if (dotTracker[y] & (uint32_t)1 << (31 - x)) {
		return TRUE;
	}
	return FALSE;
}

boolean isPowerPixel(uint8_t x, uint8_t y) {
	if ((x == PP1COL) || (x == PP2COL)) {
		if ((y == PP1ROW) || (y == PP2ROW)) {
			return TRUE;
		}
	}
	return FALSE;
}

void reverseDir(Player *pawn) {
	switch (pawn->travelDir) {
		case UP:
			pawn->travelDir = DOWN;
			break;
		case DOWN:
			pawn->travelDir = UP;
			break;
		case LEFT:
			pawn->travelDir = RIGHT;
			break;
		case RIGHT:
			pawn->travelDir = LEFT;
			break;
	}
}

void setScatterTar(Player *pawn) {
	pawn->tarX = scatterX[pawn->id];
	pawn->tarY = scatterY[pawn->id];
}

void changeSpeed(Player *pawn, uint8_t index) {
	uint8_t rowOffset;
	if (level == 0) {
		rowOffset = 0;
	}
	else if ((level >= 1) && (level <= 3)) {
		rowOffset = 5;
	}
	else if ((level >= 4) && (level <= 19)) {
		rowOffset = 10;
	}
	else {
		rowOffset = 15;
	}

	pawn->speed = speed[index + rowOffset];
}

uint8_t wasEaten(Player *player, Player *pawn) {
	if (pawn->color == GREEN_S) {
		return FALSE;	//GREEN is enemy in retreat -- no collision
	}
	if ((player->x == pawn->x) && (player->y == pawn->y)) {
		return TRUE;
	}
	return FALSE;
}

void performRetreat(Player *pawn) {
	/*TODO: Each player should have enemyMode setting
		and it should be checked here */
	//Add to the score and double the next score
	drawPlayer(pawn, true);
	drawPlayer(&myGuy, false);
	displayEatScore();
	score += eatNextEnemyScore;
	eatNextEnemyScore = eatNextEnemyScore * 2;
	drawScore();

	pawn->color = GREEN_S;
	pawn->tarX = scatterX[0];
	pawn->tarY = scatterY[0];
}

void enterHouse(Player *pawn) {
	/*TODO: Each player should have enemyMode setting
		and it should be changed here */
	pawn->color = playerColor[pawn->id];
	pawn->x = scatterX[0];
	pawn->y = scatterY[0] + 2;
	pawn->tarX = scatterX[pawn->id];
	pawn->tarY = scatterY[pawn->id];
	pawn->inPlay = FALSE;
}

void changeBehavior(Player *pawn, uint8_t mode) {
	//GREEN means enemy is in retreat mode; do nothing
	if (pawn->color == GREEN_S) {
		return;
	}

	if ((enemyMode != FRIGHT) && (pawn->id)) {
		//Enemies should reverse current direction when modes change
		//Unless coming out of FRIGHT mode
		reverseDir(pawn);

		//Not FRIGHT mode so revive natural color
		pawn->color = playerColor[pawn->id];
	}

	switch (mode) {
		case SCATTER:
			//Change Speed
			if (pawn->id == 0) {
				pawn->speedMode = SPEEDPLAYER;
				return;
			}
			pawn->speedMode = SPEEDENEMY;
			//Change Targets
			setScatterTar(pawn);
			break;
		case CHASE:
			//Change Speed
			if (pawn->id == 0) {
				pawn->speedMode = SPEEDPLAYER;
				return;
			}
			pawn->speedMode = SPEEDENEMY;
			break;
		case FRIGHT:
			//Change speeds
			if (pawn->id == 0) {
				pawn->speedMode = SPEEDPLAYERFRIGHT;
				return;
			}
			pawn->speedMode = SPEEDENEMYFRIGHT;
			//Fix colors
			pawn->color = LAVENDAR_S;
			break;
	}
}

void flashEnemy(Player *pawn, uint8_t color) {
	if ((pawn->color == WHITE_S) || (pawn->color == LAVENDAR_S)) {
		pawn->color = color;
		drawPlayer(pawn, false);
	}
}

uint16_t getDistance(uint8_t x, uint8_t y, uint8_t targetX, uint8_t targetY) {
	//Takes point and a target point and returns squared distance between them
	uint8_t hor, vert;
	if (x < targetX) {
		hor = targetX - x;
	} else {
		hor = x - targetX;
	}
	if (y < targetY) {
		vert = targetY - y;
	} else {
		vert = y - targetY;
	}

	return (hor * hor) + (vert * vert);
}

void setTarget(Player *pawn) {
	if (enemyMode != CHASE) {
		return;
	}
	if (pawn->color == GREEN_S) {
		return;	//Don't interrupt retreat
	}
	int8_t tempNum;
	switch (pawn->id) {
		case 1:
			/*--------------- Enemy1 ------------------*/
			enemy1.tarX = myGuy.x;
			enemy1.tarY = myGuy.y;
			break;

		case 2:
			/*--------------- Enemy2 ------------------*/
			switch (myGuy.travelDir) {
				case UP:
					if (myGuy.y < 4) {
						enemy2.tarY = 0;
					}
					else {
						enemy2.tarY = myGuy.y - 4;
					}
					//Account for original game overflow bug
					if (myGuy.x < 4) {
						enemy2.tarX = 0;
					}
					else {
						enemy2.tarX = myGuy.x - 4;
					}
					break;
				case DOWN:
					if (myGuy.y > 31) {
						enemy2.tarY = 35;
					}
					else {
						enemy2.tarY = myGuy.y + 4;
					}
					break;
				case LEFT:
					if (myGuy.x < 4) {
						enemy2.tarX = 0;
					}
					else {
						myGuy.tarX = enemy2.x - 4;
					}
					break;
				case RIGHT:
					if (myGuy.x > 27) {
						enemy2.tarX = 31;
					}
					else {
						enemy2.tarX = myGuy.x + 4;
					}
					break;
			}
			break;

		case 3:
			/*--------------- Enemy3 ------------------*/

			//setX
			tempNum = myGuy.x - (enemy1.x - myGuy.x);
			if (tempNum < 0) {
				tempNum = 0;
			}
			if (tempNum > 31) {
				tempNum = 31;
			}
			enemy3.tarX = (uint8_t)tempNum;

			//setY
			tempNum = myGuy.y - (enemy1.y - myGuy.y);
			if (tempNum < 0) {
				tempNum = 0;
			}
			if (tempNum > 35) {
				tempNum = 35;
			}
			enemy3.tarY = (uint8_t)tempNum;
			break;

		case 4:
			/*--------------- Enemy4 ------------------*/
			if (getDistance(enemy4.x, enemy4.y, myGuy.x, myGuy.y) > 64) {
				enemy4.tarX = myGuy.x;
				enemy4.tarY = myGuy.y;
			}
			else {
				enemy4.tarX = scatterX[enemy4.id];
				enemy4.tarY = scatterY[enemy4.id];
			}
			break;
	}
}

uint8_t canMove(uint8_t nextX, uint8_t nextY) {
	if (board[nextY] & ((uint32_t)1 << (31 - nextX))) {
		return FALSE;
	}
	else return TRUE;
}

void playerRoute(Player *pawn, uint8_t nextDir) {
	if (nextDir == pawn->travelDir) return;

	uint8_t testX = pawn->x;
	uint8_t testY = pawn->y;
	switch (nextDir) {
		case UP:
			testY--;
			break;
		case DOWN:
			testY++;
			break;
		case LEFT:
			testX--;
			break;
		case RIGHT:
			testX++;
			break;
	}

	if (canMove(testX, testY)) {
		pawn->travelDir = nextDir;
	}
}

void routeChoice(Player *pawn) {
	//This function is only used for enemies. NEVER for the player
	//TODO: This function works but seems overly complex

	//Does the pawn have a choice of routes right now?
	uint8_t testX = pawn->x;
	uint8_t testY = pawn->y;

	//Test for four intersections where turning upward is forbidden
	if (((testX == 14) || (testX == 17))
			&& ((testY == 14) || (testY == 26))
			&& pawn->travelDir != DOWN) {
		return;
	}

	//Set 3 distances then choose the shortest
	uint16_t route1, route2, route3;
	//Set arbitrarily high distance numbers
	route1 = 6000;
	route2 = 6000;
	route3 = 6000;

	/*TODO: Fix this dirty hack
		This whole block is ineloquent
		Check for "GREEN" color is workaround for retreating in FRIGHT mode
	*/
	if ((enemyMode == FRIGHT) && (pawn->color != GREEN_S)) {
		//Enemies choose route randomly in this mode
		uint8_t rnd = random(4);
		while (true) {
			switch (rnd) {
				case UP:
					if ((pawn->travelDir != DOWN) && canMove(testX, testY - 1)){
						pawn->travelDir = UP;
						return;
					}
					break;
				case LEFT:
					if ((pawn->travelDir != RIGHT) && canMove(testX - 1, testY)) {
						pawn->travelDir = LEFT;
						return;
					}
					break;
				case DOWN:
					if ((pawn->travelDir != UP) && canMove(testX, testY + 1)){
						pawn->travelDir = DOWN;
						return;
					}
					break;
				case RIGHT:
					if ((pawn->travelDir != LEFT) && canMove(testX + 1, testY)) {
						pawn->travelDir = RIGHT;
						return;
					}
					break;
			}
			rnd = (rnd+1)&7;
		}
	}
	/*---end of dirty hack*/

	switch (pawn->travelDir) {
		case UP:
			if (canMove(testX - 1, testY)) {
				route1 = getDistance(testX - 1, testY, pawn->tarX, pawn->tarY);
			}
			if (canMove(testX + 1, testY)) {
				route2 = getDistance(testX + 1, testY, pawn->tarX, pawn->tarY);
			}
			if (canMove(testX, testY - 1)) {
				route3 = getDistance(testX, testY - 1, pawn->tarX, pawn->tarY);
			}
			if ((route1 < route2) && (route1 < route3)) {
				pawn->travelDir = LEFT;
			}
			else if ((route2 < route1) && (route2 < route3)) {
				pawn->travelDir = RIGHT;
			}
			break;
		case DOWN:
			if (canMove(testX - 1, testY)) {
				route1 = getDistance(testX - 1, testY, pawn->tarX, pawn->tarY);
			}
			if (canMove(testX + 1, testY)) {
				route2 = getDistance(testX + 1, testY, pawn->tarX, pawn->tarY);
			}
			if (canMove(testX, testY + 1)) {
				route3 = getDistance(testX, testY + 1, pawn->tarX, pawn->tarY);
			}
			if ((route1 < route2) && (route1 < route3)) {
				pawn->travelDir = LEFT;
			}
			else if ((route2 < route1) && (route2 < route3)) {
				pawn->travelDir = RIGHT;
			}
			break;
		case LEFT:
			if (canMove(testX, testY - 1)) {
				route1 = getDistance(testX, testY - 1, pawn->tarX, pawn->tarY);
			}
			if (canMove(testX, testY + 1)) {
				route2 = getDistance(testX, testY + 1, pawn->tarX, pawn->tarY);
			}
			if (canMove(testX - 1, testY)) {
				route3 = getDistance(testX - 1, testY, pawn->tarX, pawn->tarY);
			}
			if ((route1 < route2) && (route1 < route3)) {
				pawn->travelDir = UP;
			}
			else if ((route2 < route1) && (route2 < route3)) {
				pawn->travelDir = DOWN;
			}
			break;
		case RIGHT:
			if (canMove(testX, testY - 1)) {
				route1 = getDistance(testX, testY - 1, pawn->tarX, pawn->tarY);
			}
			if (canMove(testX, testY + 1)) {
				route2 = getDistance(testX, testY + 1, pawn->tarX, pawn->tarY);
			}
			if (canMove(testX + 1, testY)) {
				route3 = getDistance(testX + 1, testY, pawn->tarX, pawn->tarY);
			}
			if ((route1 < route2) && (route1 < route3)) {
				pawn->travelDir = UP;
			}
			else if ((route2 < route1) && (route2 < route3)) {
				pawn->travelDir = DOWN;
			}
			break;
	}
}

void enemyTick(Player *pawn) {
	if (--pawn->speed == 0) {
		setTarget(pawn);
		routeChoice(pawn);
		movePlayer(pawn);
		checkEaten();
		changeSpeed(pawn, pawn->speedMode);
	}
}

void checkEaten(void) {
	if (enemyMode != FRIGHT) {
		if (wasEaten(&myGuy, &enemy1) ||
			wasEaten(&myGuy, &enemy2) ||
			wasEaten(&myGuy, &enemy3) ||
			wasEaten(&myGuy, &enemy4)) {
			//Game over
			gameRunning = FALSE;
			if (lives == 0) {
				//Display gameover message
				displayGameOver();
			}
		}
	} else {
		//Enemies should change color and go home when eaten
		if (wasEaten(&myGuy, &enemy1) && (enemy1.color != GREEN_S)) {
			performRetreat(&enemy1);
		}
		if (wasEaten(&myGuy, &enemy2) && (enemy2.color != GREEN_S)) {
			performRetreat(&enemy2);
		}
		if (wasEaten(&myGuy, &enemy3) && (enemy3.color != GREEN_S)) {
			performRetreat(&enemy3);
		}
		if (wasEaten(&myGuy, &enemy4) && (enemy4.color != GREEN_S)) {
			performRetreat(&enemy4);
		}
	}
}

void gobbleCount(void) {
	myGuy.dotCount += 1;
	if (myGuy.dotCount == 244) {
		//All dots have been eaten, time for next level
		gameRunning = FALSE;
		return;
	}
	if (myGuy.dotCount == 70 || myGuy.dotCount == 170) {
		fruit.displayTill = millis() + 9000 + random(1000);
		fruit.removed = false;
		displayFruit(NULL);
	}
	dotTimer = 0;	//Reset timer

	if (useGlobalDot) {
		if (globalDotCounter <= 32) {
			++globalDotCounter;
		}
	}
	else {
		if (enemy1.inPlay == FALSE) {
			enemy1.dotCount += 1;
			return;
		}
		if (enemy2.inPlay == FALSE) {
			enemy2.dotCount += 1;
			return;
		}
		if (enemy3.inPlay == FALSE) {
			enemy3.dotCount += 1;
			return;
		}
		if (enemy4.inPlay == FALSE) {
			enemy4.dotCount += 1;
			return;
		}
	}
}

void movePlayer(Player *pawn) {
	uint8_t testX = pawn->x;
	uint8_t testY = pawn->y;

	if ((pawn->color == GREEN_S) && (pawn->x == scatterX[0]) && (pawn->y == scatterY[0])) {
		//Gobbled enemy has made it home, put it in the house
		drawPlayer(pawn, true);
		enterHouse(pawn);
		drawPlayer(pawn, false);
		return;
	}
	else {
		switch (pawn->travelDir) {
			case UP:
				testY--;
				break;
			case DOWN:
				testY++;
				break;
			case LEFT:
				testX--;
				break;
			case RIGHT:
				testX++;
				break;
		}
	}

	//is next space unoccupied?
	if (canMove(testX, testY)) {
		//erase player at current spot (redraw dot if necessary)
		drawPlayer(pawn, true);
		//Tunnel Tests
		if (testY == 17) {
			if (testX == 1) {
				testX = 29;	//Warp left to right
			}
			else if (testX == 30) {
				testX = 2;	//Warp right to left
			}
			else if ((pawn->id) && (pawn->travelDir == LEFT)) {
				if (testX == 7) {
					pawn->speedMode = SPEEDENEMYTUNNEL;	//Slow down
				}
				if (testX == 23) {
					//Speed Up
					if (enemyMode != FRIGHT) {
						pawn->speedMode = SPEEDENEMY;
					}
					else {
						pawn->speedMode = SPEEDENEMYFRIGHT;
					}
				}
			}
			else if ((pawn->id) && (pawn->travelDir == RIGHT)) {
				if (testX == 24) {
					pawn->speedMode = SPEEDENEMYTUNNEL;	//Slow down
				}
				if (testX == 8) {
					//Speed Up
					if (enemyMode != FRIGHT) {
						pawn->speedMode = SPEEDENEMY;
					}
					else {
						pawn->speedMode = SPEEDENEMYFRIGHT;
					}
				}
			}
		}
		//increment player position
		pawn->x = testX;
		pawn->y = testY;
		//redraw player at new spot
		drawPlayer(pawn, false);
		//gobble the dot
		if (pawn == &myGuy && !fruit.removed && intersectsFruit(&myGuy)) {
			displayFruit(&myGuy);
			switch (level) {
				default:
					score += 2000; //5000
				case 10: case 11:
					score += 1000; //3000
				case 8: case 9:
					score += 1000; //2000
				case 6: case 7:
					score += 300;	//1000
				case 4: case 5:
					score += 200;	//700
				case 2: case 3:
					score += 200;	//500
				case 1:
					score += 200;	//300
				case 0:
					score += 100;
			}
			drawScore();
		}
		if ((pawn == &myGuy) && isPixel(pawn->x, pawn->y)) {
			dotTracker[pawn->y] &= ~((uint32_t)1 << (31 - pawn->x)); //Remove dot from the board
			score += 10;
			drawScore();
			gobbleCount();	//Increment dotCounts
			//There is a speed hit for each dot
			pawn->speed += 17;	// ~1/60th of a second
			if (isPowerPixel(pawn->x, pawn->y)) {
				//Additional speed hit for PowerPixels
				pawn->speed += 33;	// ~ 2/60th of a second
				//Additional points for gobbling this powerPixel
				score += 40;
				drawScore();
				/*---Switch to Fright mode---*/
				//Save last mode but don't if last mode was FRIGHT
				if (enemyMode != FRIGHT) {
					lastBehavior = enemyMode;
				}
				enemyMode = FRIGHT;
				frightTimer = 6000; //TODO: This should change with each level
				//Reset eatNextEnemyScore to default
				eatNextEnemyScore = EATENEMYSCOREBASE;

				changeBehavior(&myGuy, enemyMode);
				changeBehavior(&enemy1, enemyMode);
				changeBehavior(&enemy2, enemyMode);
				changeBehavior(&enemy3, enemyMode);
				changeBehavior(&enemy4, enemyMode);
			}
		}
	}
}

void checkDots(Player *pawn, uint8_t force) {
	uint8_t releaseEnemy = FALSE;
	if (pawn->inPlay == TRUE) {
		return;	//Do nothing if enemy already in play
	}
	if (force == TRUE) {
		releaseEnemy = TRUE;
	}
	else if (useGlobalDot) {
		//These global dot thresholds are always the same
		if ((pawn->id == 2) && globalDotCounter >= 7) {
			releaseEnemy = TRUE;
		}
		if ((pawn->id == 3) && globalDotCounter >= 17) {
			releaseEnemy = TRUE;
		}
		if ((pawn->id == 4) && globalDotCounter >= 32) {
			releaseEnemy = TRUE;
		}
	}
	else if (pawn->dotCount >= pawn->dotLimit) {
		releaseEnemy = TRUE;
	}

	if (releaseEnemy) {
		//displayPixel(pawn->x, pawn->y, BLACK); //erase current locaiton
		drawPlayer(pawn, true);
		pawn->x = 18;
		pawn->y = 14;
		//displayPixel(pawn->x, pawn->y, pawn->color); //Draw new locaiton
		drawPlayer(pawn, false);
		//displayLatch(); //Redraw display (if necessary)
		pawn->inPlay = TRUE;
		pawn->travelDir = LEFT; //TODO: shouldn't need to reset direction here
	}
}

void expiredDotTimer(void) {
	//Too long since a dot was eaten, release an enemy if you canMove
	dotTimer = 0;	//Reset timer

	//Will try releasing the high priority enemy and return if successful
	if (enemy1.inPlay == FALSE) {
		checkDots(&enemy1, TRUE);
		return;
	}
	if (enemy2.inPlay == FALSE) {
		checkDots(&enemy2, TRUE);
		return;
	}
	if (enemy3.inPlay == FALSE) {
		checkDots(&enemy3, TRUE);
		return;
	}
	if (enemy4.inPlay == FALSE) {
		checkDots(&enemy4, TRUE);
		return;
	}
}

void playerTick(Player * pawn) {
	if (--pawn->speed == 0) {
		playerRoute(&myGuy, nextDir);
		/*NOTE: changeSpeed must be called before
			movePlayer because the dot eaten timer
			penalty is added to the speed counter */
		changeSpeed(pawn, pawn->speedMode);
		movePlayer(&myGuy);
		checkEaten();
	}
}

void setupPlayerAfterDeath(Player *pawn) {
	pawn->x = startingX[pawn->id];
	pawn->y = startingY[pawn->id];
	if (pawn->id) {
		changeSpeed(pawn, SPEEDENEMY);
	}
	else {
		changeSpeed(pawn, SPEEDPLAYER);
	}
	pawn->travelDir = LEFT;
	pawn->color = playerColor[pawn->id];
	pawn->tarX = scatterX[pawn->id];
	pawn->tarY = scatterY[pawn->id];
	pawn->inPlay = FALSE;
}

void deathRestart(void) {
	deathAnimation();

	//Reset behavioral timer and index
	behaviorIndex = 0;
	behaviorTicks = 0;

	//set initial values for player and enemies
	setupPlayerAfterDeath(&myGuy);
	setupPlayerAfterDeath(&enemy1);
	enemy1.inPlay = TRUE; //Enemy1 always starts inPlay
	setupPlayerAfterDeath(&enemy2);
	setupPlayerAfterDeath(&enemy3);
	setupPlayerAfterDeath(&enemy4);
	enemyMode = SCATTER;
	useGlobalDot = TRUE;
}

void playMatrixman(void) {
	//TODO: Level change: Update dot counters by level

	//TODO: PowerPixel blink
	//TODO: bonus food
	/*TODO: When enemy in fright mode is eaten and makes it back to house:
		1) It should emerge as a danger to player (can be re-eaten)
		2) Does it go back into SCATTER/CHASE mode?
		3) Retreating should be at highest speed
	*/

	level = 0;
	//lives = 2;	//Including the one in play
	setupDefaults();
	//initDisplay();
	//initControl();
	//setupLevel();		 //show everything on the display

	uint8_t programRunning = TRUE;
	//gameRunning = TRUE;
	behaviorTicks = 0;
	behaviorIndex = 0;
	nextDir = RIGHT;
	dotTimer = 0;
	score = 0;
	while (programRunning)
	{

		//TODO: This is all input code which need to change when ported
		/*---- User Input ----*/
		uint8_t control = getControl();
		switch (control) {
			case NOINPUT:
				break;
			case ESCAPE:
				gameRunning = FALSE;
				programRunning = FALSE;
				continue;
			case BUTTON:
				if (gameRunning == FALSE) {
					gameRunning = TRUE;
					behaviorTicks = 0;
					behaviorIndex = 0;
					nextDir = RIGHT;
					dotTimer = 0;
					score = 0;
					setupDefaults();
					lives = 2;
					clearScreen();
					setupLevel();		 //show everything on the display
				}
				break;
			default:
				nextDir = control;
		}

		if (gameRunning) {
			refreshPowerDots();

			/* This animates the game */

			//Switch Modes
			if (enemyMode == FRIGHT) {
				--frightTimer;
				/*-- This Block Flashes the enemies coming out of FRIGHT mode --*/
				if (frightTimer <= 1800) {
					if (frightTimer % 200 == 0) {
						uint8_t flashColor;
						//if ((frightTimer/200)%2) {
						if ((frightTimer/200)%2) {
							//1800 1400 1000 600 200
							flashColor = WHITE_S;
						}
						else {
							//1600 1200 800 4000 0
							flashColor = LAVENDAR_S;
						}
						flashEnemy(&enemy1, flashColor);
						flashEnemy(&enemy2, flashColor);
						flashEnemy(&enemy3, flashColor);
						flashEnemy(&enemy4, flashColor);
					}
				}
				//Leave fright mode when timer expires
				if (frightTimer == 0) {
					enemyMode = lastBehavior;
				}

				changeBehavior(&myGuy, enemyMode);
				changeBehavior(&enemy1, enemyMode);
				changeBehavior(&enemy2, enemyMode);
				changeBehavior(&enemy3, enemyMode);
				changeBehavior(&enemy4, enemyMode);
			}
			//Switch between SCATTER and CHASE depending on level paramaters
			//This is an else statement so that the timer doesn't run during FRIGHT mode
			else if (behaviorTicks++ > behaviors[behaviorIndex]) {
				if (behaviors[behaviorIndex] > 0) {
					//Checking for 0 lets us run final behavior forever
					behaviorIndex++;
					behaviorTicks = 0;

					if (behaviorIndex & 1) {
						enemyMode = CHASE;
					}
					else {
						enemyMode = SCATTER;
					}

					changeBehavior(&myGuy, enemyMode);
					changeBehavior(&enemy1, enemyMode);
					changeBehavior(&enemy2, enemyMode);
					changeBehavior(&enemy3, enemyMode);
					changeBehavior(&enemy4, enemyMode);
				}
			}

			//decrement counter and check for trigger
			//set target or take user input
			//choose route
			//movePiece
			//checkEaten

			if (gameRunning) {
				enemyTick(&enemy1);
			}
			if (gameRunning) {
				enemyTick(&enemy2);
			}
			if (gameRunning) {
				enemyTick(&enemy3);
			}
			if (gameRunning) {
				enemyTick(&enemy4);
			}

			if (gameRunning) {
				playerTick(&myGuy);
			}

			//Enemy dot counters
			checkDots(&enemy1, FALSE);
			checkDots(&enemy2, FALSE);
			checkDots(&enemy3, FALSE);
			checkDots(&enemy4, FALSE);

			if (dotTimer++ >= 4000) {
				//TODO: this time limit should change to 3000 at lvl5+
				expiredDotTimer();
			}

			delay(1);
			/* End of game animation */
		}

		//Restart if necessary
		if (myGuy.dotCount == 244) {
			//get new level ready
			++level;
			drawPlayer(&enemy1, true);
			drawPlayer(&enemy2, true);
			drawPlayer(&enemy3, true);
			drawPlayer(&enemy4, true);
			drawBoard(true);
			drawPlayer(&myGuy, true);
			delay(500);
			setupDefaults();
			setupLevel();
			gameRunning = TRUE;
		} else if (lives && gameRunning == FALSE) {
			//Life lost, restart this level
			--lives;
			//TODO: Pause after a life is lost
			deathRestart();
			drawScore();
			setupLevel();
			gameRunning = TRUE;
		}/* else if (!lives) {
			deathAnimation();
		}*/

	}
	return;
}
