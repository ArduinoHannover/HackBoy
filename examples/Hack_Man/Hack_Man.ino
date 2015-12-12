#include <SPI.h>
#include <HackBoy.h>
#include "hackman.h"
#include "defs.h"

uint32_t startScreenFrame = 0;

void setup(void) {
	HackBoy.begin();
	tft.fillScreen(BLACK);
}

void loop(void) {
	startScreenFrame = millis();
	playMatrixman();
}

void startMelody() {
	HackBoy.noteDelay(NOTE_A, 4, 200);
	HackBoy.noteDelay(NOTE_A, 5, 100);
	HackBoy.noteDelay(NOTE_FIS, 5, 100);
	HackBoy.noteDelay(NOTE_ES, 5, 100);
	HackBoy.noteDelay(NOTE_A, 5, 50);
	HackBoy.noteDelay(NOTE_FIS, 5, 150);
	HackBoy.noteDelay(NOTE_ES, 5, 200);
	
	HackBoy.noteDelay(NOTE_C, 5, 100);
	HackBoy.noteDelay(NOTE_C, 6, 200);
	HackBoy.noteDelay(NOTE_G, 5, 100);
	HackBoy.noteDelay(NOTE_E, 5, 100);
	HackBoy.noteDelay(NOTE_C, 6, 50);
	HackBoy.noteDelay(NOTE_G, 5, 150);
	HackBoy.noteDelay(NOTE_E, 5, 200);
	
	HackBoy.noteDelay(NOTE_A, 4, 200);
	HackBoy.noteDelay(NOTE_A, 5, 100);
	HackBoy.noteDelay(NOTE_FIS, 5, 100);
	HackBoy.noteDelay(NOTE_ES, 5, 100);
	HackBoy.noteDelay(NOTE_A, 5, 50);
	HackBoy.noteDelay(NOTE_FIS, 5, 150);
	HackBoy.noteDelay(NOTE_ES, 5, 200);
	
	HackBoy.noteDelay(NOTE_ES, 5, 50);
	HackBoy.noteDelay(NOTE_E, 5, 50);
	HackBoy.noteDelay(NOTE_F, 5, 100);
	HackBoy.noteDelay(NOTE_F, 5, 50);
	HackBoy.noteDelay(NOTE_FIS, 5, 50);
	HackBoy.noteDelay(NOTE_G, 5, 100);
	HackBoy.noteDelay(NOTE_G, 5, 50);
	HackBoy.noteDelay(NOTE_AS, 5, 50);
	HackBoy.noteDelay(NOTE_A, 5, 100);
	HackBoy.noteDelay(NOTE_H, 5, 200);
}

void eatSound() {
	HackBoy.note(NOTE_C, 5, 50);
}





boolean startKey = false;

uint8_t getControl(void) {
	uint8_t keypad = HackBoy.getMovement();
	Serial.println(keypad);
	if (HackBoy.getLeftKey()) return LEFT;
	if (HackBoy.getUpKey()) return UP;
	if (HackBoy.getRightKey()) return RIGHT;
	if (HackBoy.getDownKey()) return DOWN;
	if (HackBoy.getStartKey() != startKey) {
		startKey = !startKey;
		if (startKey) return BUTTON;
	}
	return NOINPUT;
}


//GRAPHICS UNO

static const uint8_t rdy[] = {
	0b01100111, 0b00100110, 0b01010010,
	0b01010100, 0b01010101, 0b01010010,
	0b01100110, 0b01110101, 0b00100100,
	0b01010100, 0b01010101, 0b00100000,
	0b01010111, 0b01010110, 0b00100100
};
void getReady(void) {
	for (uint8_t i = 0; i < 120; i++) {
		if ((rdy[i / 8] >> (0x7 - (i & 0x7))) & 1) {
			tft.drawPixel(X_OFFSET + 13 * PXPF + (i % 24), Y_OFFSET + 20 * PXPF - 2 + i / 24, YELLOW);
		}
	}
	if (getScore() == 0)
		startMelody();
	else
		delay(2000);
	tft.fillRect(X_OFFSET + 13 * PXPF, Y_OFFSET + 20 * PXPF - 2, 24, 5, BLACK);
}

void drawLives(void) {
	for (uint8_t i = 0; i < getLives(); i++) {
		tft.fillCircle(119, 120 - i * 6, 2.5, YELLOW);
		tft.drawPixel(119, 120 - i * 6, BLACK);
		tft.fillRect(120, 119 - i * 6, 2, 3, BLACK);
	}
	tft.fillCircle(119, 120 - getLives() * 6, 2.5, BLACK);
}

boolean initiating = true;
void drawBoard(boolean won) {
	uint8_t length;
	uint8_t offset;
	uint16_t color;
	drawPlayer(getEnemy1(), true);
	drawPlayer(getEnemy2(), true);
	drawPlayer(getEnemy3(), true);
	drawPlayer(getEnemy4(), true);
	for (uint8_t i = 0; i <= 5 * won; i++) {
		uint32_t startingTime = millis();
		color = i & 1 ? WHITE : BLUE;
		for (uint8_t x = 2; x < 30; x++) {
			for (uint8_t y = 3; y < 34; y++) {
				if ((board[y] >> x) & 1) {
					if ((x == 15 || x == 16) && y == 15) {
						tft.drawFastHLine(X_OFFSET + 15 * PXPF - 1, Y_OFFSET + 15 * PXPF, 8, PINK);
					} else {
						if (!((board[y] >> (x - 1)) & 1)) { //line right of field
							offset = PXPF + ((board[y - 1] >> x) & 1) * 2;
							length = 1 + (((board[y + 1] >> x) & 1) + (offset != PXPF)) * 2;
							if (length > 1)
								tft.drawFastVLine(X_OFFSET + x * PXPF - 1, Y_OFFSET + (y + 1) * PXPF - offset, length, color);
						}
						if (!((board[y] >> (x + 1)) & 1)) { //line left of field
							offset = PXPF + ((board[y - 1] >> x) & 1) * 2;
							length = 1 + (((board[y + 1] >> x) & 1) + (offset != PXPF)) * 2;
							if (length > 1)
								tft.drawFastVLine(X_OFFSET + x * PXPF + 1, Y_OFFSET + (y + 1) * PXPF - offset, length, color);
						}
						if (!((board[y - 1] >> x) & 1)) { //line below field
							offset = PXPF + ((board[y] >> (x - 1)) & 1) * 2;
							length = 1 + (((board[y] >> (x + 1)) & 1) + (offset != PXPF)) * 2;
							//if (length > 1)
							tft.drawFastHLine(X_OFFSET + (x + 1) * PXPF - offset, Y_OFFSET + (y + 1) * PXPF - 5, length, color);
						}
						if (!((board[y + 1] >> x) & 1)) { //line over field
							offset = PXPF + ((board[y] >> (x - 1)) & 1) * 2;
							length = 1 + (((board[y] >> (x + 1)) & 1) + (offset != PXPF)) * 2;
							//if (length > 1)
							tft.drawFastHLine(X_OFFSET + (x + 1) * PXPF - offset, Y_OFFSET + (y + 1) * PXPF - 3, length, color);
						}
					}
				}
			}
		}
		if (won) while (millis() - 400 < startingTime);
	}
	if (!won) {
		for (uint8_t x = 3; x < 30; x++) {
			for (uint8_t y = 4; y < 33; y++) {
				if (isPixel(x,y)) {
					displayDots(x, y, true, false);
				}
			}
		}
	}
	initiating = true;
}

void drawPlayer(Player* pawn, boolean erase) {
	if (pawn == getMyGuy()) {
		tft.fillCircle((pawn->x * PXPF) + X_OFFSET, (pawn->y * PXPF) + Y_OFFSET, 2.5, erase ? BLACK : YELLOW);
		if (initiating) {initiating = false;return;}
		if (!erase) {
			boolean fullOpen = (pawn->x + pawn->y) & 1;
			if (fullOpen) tft.drawPixel((pawn->x * PXPF) + X_OFFSET, (pawn->y * PXPF) + Y_OFFSET, BLACK);
			switch (pawn->travelDir) {
				case LEFT:
					if (fullOpen)
						tft.fillRect((pawn->x * PXPF) + X_OFFSET - 2, (pawn->y * PXPF) + Y_OFFSET - 1, 2, 3, BLACK);
					else
						tft.drawFastHLine((pawn->x * PXPF) + X_OFFSET - 2, (pawn->y * PXPF) + Y_OFFSET, 2, BLACK);
					break;
				case UP:
					if (fullOpen)
						tft.fillRect((pawn->x * PXPF) + X_OFFSET - 1, (pawn->y * PXPF) + Y_OFFSET - 2, 3, 2, BLACK);
					else
						tft.drawFastVLine((pawn->x * PXPF) + X_OFFSET, (pawn->y * PXPF) + Y_OFFSET - 2, 2, BLACK);
					break;
				case RIGHT:
					if (fullOpen)
						tft.fillRect((pawn->x * PXPF) + X_OFFSET + 1, (pawn->y * PXPF) + Y_OFFSET - 1, 2, 3, BLACK);
					else
						tft.drawFastHLine((pawn->x * PXPF) + X_OFFSET + 1, (pawn->y * PXPF) + Y_OFFSET, 2, BLACK);
					break;
				case DOWN:
					if (fullOpen)
						tft.fillRect((pawn->x * PXPF) + X_OFFSET - 1, (pawn->y * PXPF) + Y_OFFSET + 1, 3, 2, BLACK);
					else
						tft.drawFastVLine((pawn->x * PXPF) + X_OFFSET, (pawn->y * PXPF) + Y_OFFSET + 1, 2, BLACK);
					break;
			}
		}
	} else {
		if (!erase && pawn->color == GREEN_S) {
			tft.fillRect((pawn->x * PXPF) + X_OFFSET - 2, (pawn->y * PXPF) + Y_OFFSET - 1, 2, 2, WHITE);
			tft.fillRect((pawn->x * PXPF) + X_OFFSET + 1, (pawn->y * PXPF) + Y_OFFSET - 1, 2, 2, WHITE);
			tft.drawPixel((pawn->x * PXPF) + X_OFFSET - 1 - (pawn->travelDir == LEFT), (pawn->y * PXPF) + Y_OFFSET - (pawn->travelDir == UP), BLACK);
			tft.drawPixel((pawn->x * PXPF) + X_OFFSET + 1 + (pawn->travelDir == RIGHT), (pawn->y * PXPF) + Y_OFFSET - (pawn->travelDir == UP), BLACK);
		} else {
			uint16_t color = erase ? BLACK : colors[pawn->color];
			tft.fillRect((pawn->x * PXPF) + X_OFFSET - 2, (pawn->y * PXPF) + Y_OFFSET - 1, 5, 3, color);
			tft.drawFastHLine((pawn->x * PXPF) + X_OFFSET - 1, (pawn->y * PXPF) + Y_OFFSET - 2, 3, color);
			for (uint8_t i = 0; i < 3; i++)
				tft.drawPixel((pawn->x * PXPF) + X_OFFSET - 2 + i * 2, (pawn->y * PXPF) + Y_OFFSET + 2, color);
			if (!erase) {
				tft.drawPixel((pawn->x * PXPF) + X_OFFSET - 1 - (pawn->travelDir == LEFT) + (pawn->travelDir == RIGHT), (pawn->y * PXPF) + Y_OFFSET - (pawn->travelDir == UP), WHITE);
				tft.drawPixel((pawn->x * PXPF) + X_OFFSET + 1 - (pawn->travelDir == LEFT) + (pawn->travelDir == RIGHT), (pawn->y * PXPF) + Y_OFFSET - (pawn->travelDir == UP), WHITE);
			}
		}
	}
	if (pawn != getMyGuy() && (erase || (pawn->x == getMyGuy()->x && pawn->y == getMyGuy()->y))) {
		restoreDot(pawn->x, pawn->y);
		displayFruit(pawn);
		drawPlayer(getMyGuy(), false);
	}
}

#define FRUIT_XC (X_OFFSET + 15*PXPF+PXPF/2) //center
#define FRUIT_YC (Y_OFFSET + FRUIT_Y*PXPF)
void displayFruit(Player *pawn) {
	if (getFruit()->removed) return;
	if (pawn == getMyGuy() || millis() > getFruit()->displayTill) {
		tft.fillRect(FRUIT_XC - 2, FRUIT_YC - 2, 5, 5, BLACK);
		getFruit()->removed = true;
		drawPlayer(getMyGuy(), false);
		return;
	}
	if (pawn != NULL && !intersectsFruit(pawn)) return; //No need to redraw
	switch (getLevel()) {
		case 0:
			tft.fillCircle(FRUIT_XC - 1, FRUIT_YC + 1, 1.5, RED);
			tft.fillCircle(FRUIT_XC + 1, FRUIT_YC + 1, 1.5, RED);
			tft.drawPixel(FRUIT_XC - 1, FRUIT_YC - 1, GREEN);
			tft.drawPixel(FRUIT_XC, FRUIT_YC - 2, GREEN);
			tft.drawPixel(FRUIT_XC + 1, FRUIT_YC - 1, GREEN);
			return;
		case 1:
			tft.drawFastHLine(FRUIT_XC - 1, FRUIT_YC - 2, 3, GREEN);
			tft.fillRect(FRUIT_XC - 2, FRUIT_YC - 1, 5, 2, RED);
			tft.drawFastHLine(FRUIT_XC - 1, FRUIT_YC + 1, 3, RED);
			tft.drawPixel(FRUIT_XC, FRUIT_YC + 2, RED);
			return;
		case 2: case 3:
			tft.fillCircle(FRUIT_XC, FRUIT_YC, 2.5, ORANGE);
			tft.drawFastHLine(FRUIT_XC + 1, FRUIT_YC - 2, 2, GREEN);
			return;
		case 4: case 5:
			tft.fillCircle(FRUIT_XC, FRUIT_YC, 2.5, RED);
			tft.drawPixel(FRUIT_XC, FRUIT_YC - 2, ORANGE);
			tft.drawPixel(FRUIT_XC, FRUIT_YC + 2, BLACK);
			return;
		case 6: case 7:
			tft.drawFastVLine(FRUIT_XC, FRUIT_YC - 2, 2, ORANGE);
			tft.drawPixel(FRUIT_XC - 1, FRUIT_YC - 1, GREEN);
			tft.drawPixel(FRUIT_XC + 1, FRUIT_YC - 1, GREEN);
			tft.drawPixel(FRUIT_XC - 2, FRUIT_YC, GREEN);
			tft.drawPixel(FRUIT_XC, FRUIT_YC, GREEN);
			tft.drawPixel(FRUIT_XC + 2, FRUIT_YC, GREEN);
			tft.drawPixel(FRUIT_XC - 1, FRUIT_YC + 1, GREEN);
			tft.drawPixel(FRUIT_XC + 1, FRUIT_YC + 1, GREEN);
			tft.drawPixel(FRUIT_XC, FRUIT_YC + 2, GREEN);
			return;
		case 8: case 9:
			tft.fillCircle(FRUIT_XC, FRUIT_YC - 1, 1.5, RED);
			tft.drawFastHLine(FRUIT_XC - 1, FRUIT_YC + 1, 3, BLUE);
			tft.drawFastVLine(FRUIT_XC - 2, FRUIT_YC - 2, 3, BLUE);
			tft.drawFastVLine(FRUIT_XC + 2, FRUIT_YC - 2, 3, BLUE);
			tft.drawFastVLine(FRUIT_XC, FRUIT_YC - 1, 3, YELLOW);
			return;
		case 10: case 11:
			tft.drawPixel(FRUIT_XC, FRUIT_YC - 2, YELLOW);
			tft.fillRect(FRUIT_XC - 1, FRUIT_YC - 1, 3, 2, YELLOW);
			tft.drawFastHLine(FRUIT_XC - 2, FRUIT_YC + 1, 5, YELLOW);
			tft.drawFastHLine(FRUIT_XC - 2, FRUIT_YC + 2, 5, BLUE);
			tft.drawPixel(FRUIT_XC + 1, FRUIT_YC + 2, GREY);
			return;
		default:
			tft.fillRect(FRUIT_XC - 1, FRUIT_YC - 2, 3, 2, CYAN);
			tft.drawFastVLine(FRUIT_XC, FRUIT_YC, 3, GREY);
			tft.drawPixel(FRUIT_XC + 1, FRUIT_YC + 1, GREY);
	}
}

void displayDots(uint8_t x, uint8_t y, boolean init, boolean powerOff) {
	if (!isPixel(x,y)) return;
	boolean power = isPowerPixel(x, y);
	if (power) {
		if (x == getEnemy1()->x && y == getEnemy1()->y) return;
		if (x == getEnemy2()->x && y == getEnemy2()->y) return;
		if (x == getEnemy3()->x && y == getEnemy3()->y) return;
		if (x == getEnemy4()->x && y == getEnemy4()->y) return;
	}
	x = (x * PXPF) + X_OFFSET;
	y = (y * PXPF) + Y_OFFSET;
	if (init || !power || !powerOff) {
		tft.drawPixel(x, y, FOODCOLOR);
		if (power) {
			tft.fillCircle(x, y, 1.5, FOODCOLOR);
		}
	} else {
		tft.fillCircle(x, y, 1.5, BLACK);
	}
}

void displayEatScore(void) {
	tft.setTextColor(CYAN, BLACK);
	tft.setCursor(X_OFFSET + 13 * PXPF - 1 + (getEatNextEnemyScore() < 1000) * 3, Y_OFFSET + 16 * PXPF + 2);
	tft.print(getEatNextEnemyScore());
	delay(500);
	tft.fillRect(X_OFFSET + 13 * PXPF, Y_OFFSET + 16 * PXPF + 2, PXPF*5.5, PXPF*2, BLACK);
	//tft.setTextColor(BLACK);
	//tft.setCursor(X_OFFSET + 13 * PXPF - 1 + (getEatNextEnemyScore() < 1000) * 3, Y_OFFSET + 16 * PXPF + 2);
	//tft.print(getEatNextEnemyScore());
}

void deathAnimation(void) {
	getFruit()->displayTill = 0;
	displayFruit(NULL);
	delay(500);
	drawPlayer(getEnemy1(), true);
	drawPlayer(getEnemy2(), true);
	drawPlayer(getEnemy3(), true);
	drawPlayer(getEnemy4(), true);
	
	tft.fillCircle((getMyGuy()->x * PXPF) + X_OFFSET, (getMyGuy()->y * PXPF) + Y_OFFSET, 2.5, YELLOW);
	tft.drawFastVLine((getMyGuy()->x * PXPF) + X_OFFSET, (getMyGuy()->y * PXPF) + Y_OFFSET - 2, 2, BLACK);
	HackBoy.noteDelay(NOTE_A, 5, 100);
	HackBoy.noteDelay(NOTE_GIS, 5, 100);
	tft.fillRect((getMyGuy()->x * PXPF) + X_OFFSET - 1, (getMyGuy()->y * PXPF) + Y_OFFSET - 2, 3, 2, BLACK);
	HackBoy.noteDelay(NOTE_A, 5, 100);
	HackBoy.noteDelay(NOTE_GIS, 5, 100);
	tft.drawFastHLine((getMyGuy()->x * PXPF) + X_OFFSET - 2, (getMyGuy()->y * PXPF) + Y_OFFSET - 1, 5, BLACK);
	HackBoy.noteDelay(NOTE_G, 5, 100);
	HackBoy.noteDelay(NOTE_GIS, 5, 100);
	tft.drawPixel((getMyGuy()->x * PXPF) + X_OFFSET - 2, (getMyGuy()->y * PXPF) + Y_OFFSET, BLACK);
	tft.drawPixel((getMyGuy()->x * PXPF) + X_OFFSET + 2, (getMyGuy()->y * PXPF) + Y_OFFSET, BLACK);
	HackBoy.noteDelay(NOTE_G, 5, 100);
	HackBoy.noteDelay(NOTE_FIS, 5, 100);
	tft.drawPixel((getMyGuy()->x * PXPF) + X_OFFSET - 1, (getMyGuy()->y * PXPF) + Y_OFFSET, BLACK);
	tft.drawPixel((getMyGuy()->x * PXPF) + X_OFFSET - 2, (getMyGuy()->y * PXPF) + Y_OFFSET + 1, BLACK);
	tft.drawPixel((getMyGuy()->x * PXPF) + X_OFFSET + 1, (getMyGuy()->y * PXPF) + Y_OFFSET, BLACK);
	tft.drawPixel((getMyGuy()->x * PXPF) + X_OFFSET + 2, (getMyGuy()->y * PXPF) + Y_OFFSET + 1, BLACK);
	HackBoy.noteDelay(NOTE_G, 5, 100);
	HackBoy.noteDelay(NOTE_FIS, 5, 100);
	tft.drawFastVLine((getMyGuy()->x * PXPF) + X_OFFSET - 1, (getMyGuy()->y * PXPF) + Y_OFFSET + 1, 2, BLACK);
	tft.drawFastVLine((getMyGuy()->x * PXPF) + X_OFFSET + 1, (getMyGuy()->y * PXPF) + Y_OFFSET + 1, 2, BLACK);
	HackBoy.noteDelay(NOTE_F, 5, 100);
	HackBoy.noteDelay(NOTE_FIS, 5, 100);
	tft.drawPixel((getMyGuy()->x * PXPF) + X_OFFSET, (getMyGuy()->y * PXPF) + Y_OFFSET, BLACK);
	tft.drawFastHLine((getMyGuy()->x * PXPF) + X_OFFSET - 2, (getMyGuy()->y * PXPF) + Y_OFFSET, 2, YELLOW);
	tft.drawFastVLine((getMyGuy()->x * PXPF) + X_OFFSET, (getMyGuy()->y * PXPF) + Y_OFFSET - 2, 2, YELLOW);
	tft.drawFastHLine((getMyGuy()->x * PXPF) + X_OFFSET + 1, (getMyGuy()->y * PXPF) + Y_OFFSET, 2, YELLOW);
	tft.drawPixel((getMyGuy()->x * PXPF) + X_OFFSET - 2, (getMyGuy()->y * PXPF) + Y_OFFSET - 2, YELLOW);
	tft.drawPixel((getMyGuy()->x * PXPF) + X_OFFSET + 2, (getMyGuy()->y * PXPF) + Y_OFFSET - 2, YELLOW);
	tft.drawPixel((getMyGuy()->x * PXPF) + X_OFFSET - 2, (getMyGuy()->y * PXPF) + Y_OFFSET + 2, YELLOW);
	tft.drawPixel((getMyGuy()->x * PXPF) + X_OFFSET + 2, (getMyGuy()->y * PXPF) + Y_OFFSET + 2, YELLOW);
	HackBoy.noteDelay(NOTE_F, 5, 150);
	HackBoy.noteDelay(NOTE_F, 5, 150);
	delay(300);
	tft.fillRect((getMyGuy()->x * PXPF) + X_OFFSET - 2, (getMyGuy()->y * PXPF) + Y_OFFSET - 2, 5, 5, BLACK);
}

uint32_t oldScore = -1;
void drawScore() {
	tft.setTextSize(1);
	uint32_t div = 1000000000;
	for (uint8_t i = 0; i < 10; i++) {
		if ((oldScore / div) % 10 != (getScore() / div) % 10) {
			tft.setTextColor(BLACK, BLACK);
			tft.setCursor(117, i * 8);
			tft.print((oldScore / div) % 10);
			tft.setTextColor(WHITE, BLACK);
			tft.setCursor(117, i * 8);
			tft.print((getScore() / div) % 10);
		}
		div /= 10;
	}
	oldScore = getScore();
}

void clearScreen(void) {
	tft.fillScreen(BLACK);
	oldScore = -1;
}

static const uint32_t gmvr[] = {
	0b01100100101011100010010101110110,
	0b10001010111010000101010101000101,
	0b10101110101011000101010101100110,
	0b10101010101010000101010101000101,
	0b01101010101011100010001001110101
};
void displayGameOver(void) {
	if (getMyGuy()->y != 0) {
		deathAnimation();
	}
	for (uint8_t i = 0; i < 5; i++) {
		for (uint8_t b = 0; b < 32; b++) {
			if ((gmvr[i] >> b) & 1) {// ((uint32_t)1 << b)) {
				tft.drawPixel(X_OFFSET + 19 * PXPF - b + 2, Y_OFFSET + 20 * PXPF - 2 + i, RED);
			}
		}
	}
	delay(3000);
	startScreenFrame = millis();
}

uint8_t oldFrame = -1;
void drawStartScreen() {
	uint8_t frame = ((millis()-startScreenFrame) / 150)&127;
	if (oldFrame == frame) return;
	if ((frame & 1) && !(frame & 2)) {
		tft.setTextColor((frame & 4) ? BLACK : WHITE, BLACK);
		tft.setCursor(PXPF*7,PXPF*30);
		tft.print(F("PRESS START"));
	}
	switch(frame) {
		case 52:
			tft.print("   \"CLYDE\"");
			break;
		case 50:
			tft.setCursor(PXPF*4,PXPF*16);
			tft.setTextColor(ORANGE, BLACK);
			tft.print("-POKEY ");
			break;
		case 44:
			getEnemy4()->x = 4;
			getEnemy4()->y = 19;
			getEnemy4()->color = ORANGE_S;
			drawPlayer(getEnemy4(), false);
			break;
		case 40:
			tft.print("  \"INKY\"");
			break;
		case 38:
			tft.setCursor(PXPF*4,PXPF*12);
			tft.setTextColor(CYAN, BLACK);
			tft.print("-BASHFUL");
			break;
		case 32:
			getEnemy3()->x = 4;
			getEnemy3()->y = 15;
			getEnemy3()->color = CYAN_S;
			drawPlayer(getEnemy3(), false);
			break;
		case 28:
			tft.print("  \"PINKY\"");
			break;
		case 26:
			tft.setCursor(PXPF*4,PXPF*8);
			tft.setTextColor(PINK, BLACK);
			tft.print("-SPEEDY ");
			break;
		case 20:
			getEnemy2()->x = 4;
			getEnemy2()->y = 11;
			getEnemy2()->color = PINK_S;
			drawPlayer(getEnemy2(), false);
			break;
		case 16:
			tft.print("  \"BLINKY\"");
			break;
		case 14:
			tft.setCursor(PXPF*4,PXPF*4);
			tft.setTextColor(RED, BLACK);
			tft.print("-SHADOW ");
			break;
		case 8:
			getEnemy1()->x = 4;
			getEnemy1()->y = 7;
			getEnemy1()->color = RED_S;
			drawPlayer(getEnemy1(), false);
			break;
		case 0:
			clearScreen();
			tft.setCursor(PXPF*4,PXPF);
			tft.setTextColor(WHITE, BLACK);
			tft.setTextSize(1);
			tft.print("CHARACTER / NICK");
			getMyGuy()->x = 90;
			break;	
		default:
			if (frame < 60) break;
			if (frame < 90) {
				displayDots(PP1COL, PP2ROW, false, frame & 4);
			}
			getMyGuy()->y = 26;
			getEnemy1()->y = 26;
			getEnemy2()->y = 26;
			getEnemy3()->y = 26;
			getEnemy4()->y = 26;
			getMyGuy()->travelDir = frame<90?LEFT:RIGHT;
			getEnemy1()->travelDir = frame<90?LEFT:RIGHT;
			getEnemy2()->travelDir = frame<90?LEFT:RIGHT;
			getEnemy3()->travelDir = frame<90?LEFT:RIGHT;
			getEnemy4()->travelDir = frame<90?LEFT:RIGHT;
			if (frame == 90) {
				getEnemy1()->color = BLUE_S;
				getEnemy2()->color = BLUE_S;
				getEnemy3()->color = BLUE_S;
				getEnemy4()->color = BLUE_S;
				
			}
			drawPlayer(getEnemy1(), true);
			drawPlayer(getEnemy2(), true);
			drawPlayer(getEnemy3(), true);
			drawPlayer(getEnemy4(), true);
			drawPlayer(getMyGuy(), true);
			getMyGuy()->x = 3+abs(frame-90);
			drawPlayer(getMyGuy(), false);
			getEnemy1()->x = 5+abs(frame-90);
			drawPlayer(getEnemy1(), false);
			getEnemy2()->x = 7+abs(frame-90);
			drawPlayer(getEnemy2(), false);
			getEnemy3()->x = 9+abs(frame-90);
			drawPlayer(getEnemy3(), false);
			getEnemy4()->x = 11+abs(frame-90);
			drawPlayer(getEnemy4(), false);
	}
	oldFrame = frame;
}
