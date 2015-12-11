#include <SPI.h>
#include "HackBoy.h"

#define	BLACK   0x0000
#define	BLUE    0x001F
#define	RED     0xF800
#define	GREEN   0x07E0
#define CYAN    0x07FF
#define MAGENTA 0xF81F
#define YELLOW  0xFFE0  
#define WHITE   0xFFFF



void setup(void) {
	HackBoy.begin();
	//startGame(3);
	showMenu();
}
uint8_t difficulty = 0;
boolean multiplayer = false;
uint8_t player = 1;
uint8_t p1_paddle = 100;
uint8_t p2_paddle = 90;
uint16_t touches = 0, speed = 1;
uint8_t ball[] = {106,44};
int8_t movement[] = {0,0,-1,1};
uint8_t points[] = {0,0};

void drawField(boolean clear) {
	if (clear)
		tft.fillScreen(BLACK);
	for (uint8_t i = 0; i < 8; i++) {
		tft.drawLine(63, i*16, 63, i*16+8, WHITE);
	}
	tft.setTextSize(3);
	tft.setTextColor(WHITE);
	tft.setCursor(40, 10);
	tft.print(player == 2 ? points[1] : points[0]);
	tft.setCursor(70, 10);
	tft.print(player == 1 ? points[1] : points[0]);
}

void startGame(uint8_t mode) {
	multiplayer = false;
	if (mode == 3) {
		player = HackBoy.startMultiplayer();
		if (player) multiplayer = true;
		else return showMenu();
		drawField(true);
		tft.setTextSize(4);
		for (uint8_t i = 3; i > 0; i--) {
			tft.setTextColor(BLACK);
			tft.setCursor(54, 48);
			tft.print(i+1);
			tft.setTextColor(WHITE);
			tft.setCursor(54, 48);
			tft.print(i);
			delay(950);
		}
	} else {
		difficulty = mode;
		player = 1;
	}
	p1_paddle = 63;
	p2_paddle = 63;
	ball[0] = 63;
	ball[1] = 63;
	movement[0] = 0;
	movement[1] = 0;
	movement[2] = -1;
	movement[3] = 0;
	touches = 0;
	points[0] = 0;
	points[1] = 0;
	drawField(true);
}

void showMenu() {
	tft.fillScreen(BLACK);
	tft.setTextSize(3);
	tft.setTextColor(WHITE);
	tft.setCursor(30, 10);
	tft.print("PONG");
	tft.setTextSize(1);
	tft.setCursor(0, 50);
	tft.println("  Easy");
	tft.println("");
	tft.println("  Medium");
	tft.println("");
	tft.println("  Hard");
	tft.println("");
	tft.println("  Multiplayer");
	uint8_t selected = 0;
	uint8_t old_sel = 0;
	tft.fillRect(4, 43, 4, 20, WHITE);
	while (true) {
		uint8_t sel = HackBoy.getMovement();
		if ((sel & 5) != (old_sel & 5)) {
			tft.fillRect(4, 43+selected*16, 4, 20, BLACK);
			selected = (selected + ((sel&1)-((sel>>2)&1)) + 4) & 3;
			tft.fillRect(4, 43+selected*16, 4, 20, WHITE);
		}
		old_sel = sel;
		if (HackBoy.getStartKey()) {
			return startGame(selected);
		}
	}
	
}

int16_t estimated_pos = 0;
int8_t hit_diff = 0;
void loop(void) {
	if (multiplayer) {
		if (player == 2) {
			movement[0] = HackBoy.recvMessage() - p1_paddle;
			if (!HackBoy.sendMessage(p2_paddle + movement[1])) showMenu();
		} else {
			if (!HackBoy.sendMessage(p1_paddle + movement[0])) showMenu();
			movement[1] = HackBoy.recvMessage() - p2_paddle;
		}
	} else {
		movement[1] = 0;
		tft.drawPixel(116, estimated_pos, BLACK);
		if (movement[2] > 0) {
			if (difficulty == 2) {
				estimated_pos = abs((ball[1] - 2) + (116 - ball[0]) * movement[3] / movement[2]);
				while (estimated_pos >= 124) estimated_pos = abs(246 - estimated_pos);
				estimated_pos += 2;
				estimated_pos += hit_diff;
				movement[1] = min(4, max(-4, estimated_pos - p2_paddle));
				//if (p2_paddle - estimated_pos > 0) movement[1] = max(-4, estimated_pos - p2_paddle);
				//else if (estimated_pos - p2_paddle > 0) movement[1] = min(4, estimated_pos - p2_paddle);
				if (p2_paddle + movement[1] >= 117) movement[1] = 117-p2_paddle;
				if (p2_paddle + movement[1] <= 10) movement[1] = 10-p2_paddle;
			} else {
				if (random(4-difficulty) < 2) {
					if (p2_paddle - ball[1] > 2) movement[1] = -3;
					else if (ball[1] - p2_paddle > 2) movement[1] = 3;
					if (p2_paddle + movement[1] >= 117) movement[1] = 117-p2_paddle;
					if (p2_paddle + movement[1] <= 10) movement[1] = 10-p2_paddle;
				}
			}
		} else {
			if (abs(p2_paddle - 63) > 2) movement[1] = (63 - p2_paddle) > 0 ? 3 : -3;
		}
	}
	
	tft.fillRect((player == 2 ? (126 - ball[0]) : (ball[0] - 2)), ball[1]-2, 4, 4, BLACK);
	if (movement[0] != 0)
		tft.fillRect((player == 2 ? 119 : 4), p1_paddle-10, 4, 20, BLACK);
	if (movement[1] != 0)
		tft.fillRect((player == 1 ? 119 : 4), p2_paddle-10, 4, 20, BLACK);
	
	p1_paddle+=movement[0];p2_paddle+=movement[1];
	int8_t ball_y = ball[1] + movement[3]; 
	if (ball_y < 2) {
		ball[1] = 2 - ball_y;
		movement[3] *= -1; 
	} else if (ball_y > 125) {
		ball[1] = 250 - ball_y;
		movement[3] *= -1;
	} else {
		ball[1] += movement[3];
	}
	ball[0] += movement[2];

	if (multiplayer) {
		if (player == 2) {
			movement[2] = HackBoy.recvMessage() - ball[0];
			movement[3] = HackBoy.recvMessage() - ball[1];
		} else {
			if (!HackBoy.sendMessage(ball[0] + movement[2])) showMenu();
			if (!HackBoy.sendMessage(ball[1] + movement[3])) showMenu();
		}
	}

	
	tft.fillRect((player == 2 ? (126 - ball[0]) : (ball[0] - 2)), ball[1]-2, 4, 4, WHITE);
	tft.fillRect((player == 2 ? 119 : 4), p1_paddle-10, 4, 20, WHITE);
	tft.fillRect((player == 1 ? 119 : 4), p2_paddle-10, 4, 20, WHITE);
	drawField(false);
	if (ball[1] <= 2 || ball[1] >= 125) {
		movement[3] *= -1;
	}
	if (ball[0] <= 10 && ball[0] > 6 && movement[2] < 0) {
		if (p1_paddle+10 >= ball[1] && p1_paddle-10 <= ball[1]) {
			touches++;
			speed = min(110, (touches >> 2) + 1);
			movement[2] = speed;
			movement[3] = (ball[1] - p1_paddle) / 3 * movement[2];
			tone(3, 440, 50);
		}
	} else if (ball[0] >= 116 && ball[0] < 121 && movement[2] > 0) {
		if (p2_paddle+10 >= ball[1] && p2_paddle-10 <= ball[1]) {
			touches++;
			speed = min(110, (touches >> 2) + 1);
			movement[2] = - speed;
			movement[3] = (ball[1] - p2_paddle) / 3 * -movement[2];
			hit_diff = 7 - random(17);
			tone(3, 440, 50);
		}
	} else if (ball[0] <= 6 || ball[0] >= 122) {
		tft.fillRect((player == 2 ? (126 - ball[0]) : (ball[0] - 2)), ball[1]-2, 4, 4, BLACK);
		tft.setTextSize(3);
		tft.setTextColor(BLACK);
		tft.fillRect(40,10,50,24,BLACK);
		points[ball[0] < 64]++;
		if (points[ball[0] < 64] == 10) {
			tft.fillScreen(BLACK);
			tft.setTextSize(3);
			tft.setTextColor(WHITE);
			tft.setCursor(40,40);
			tft.println("You");
			tft.setCursor(0,64);
			tone((3, 110-330*(ball[0] < 64) == player), 600);
			tft.print((1 + (ball[0] < 64) == player) ? "  won!" : "lost :(");
			while(!HackBoy.getStartKey());
			showMenu();
			return;
		} else {
			tone(3, 110, 400);
		}
		movement[3] = 0;
		movement[2] = 1 - ((ball[0] < 64) * 2);
		ball[0] = 63;
		ball[1] = min(125, max(2, ball[1]));
		touches = 0;
		drawField(false);
	}
	if ((ball[0] + movement[2] <= 7 && movement[2] < 0)) {
		movement[3] /= movement[2];
		movement[2] = min(-1, 8 - ball[0]);
		movement[3] *= movement[2];
	}
	if (ball[0] + movement[2] >= 121 && movement[2] > 0) {
		movement[3] /= movement[2];
		movement[2] = max(1, 120 - ball[0]);
		movement[3] *= movement[2];
	}
	delay(20);
	uint8_t mvmnt = HackBoy.getMovement();
	movement[player == 2] = ((mvmnt&1)-((mvmnt>>2)&1)) * 3;
	if (player == 1) {
		if (p1_paddle + movement[0] >= 117) movement[0] = 117-p1_paddle;
		if (p1_paddle + movement[0] <= 10) movement[0] = 10-p1_paddle;
	} else {
		if (p2_paddle + movement[1] >= 117) movement[1] = 117-p2_paddle;
		if (p2_paddle + movement[1] <= 10) movement[1] = 10-p2_paddle;
	}
}

/**/
