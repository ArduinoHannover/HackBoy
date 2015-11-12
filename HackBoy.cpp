#include "HackBoy.h"

HackBoyC::HackBoyC() {
}

void HackBoyC::begin() {
	Serial.begin(115200);
	tft.begin();
	#ifdef ARDUINO_SAM_DUE
		tft.setRotation(iliRotation90);
		tft.setFont(SystemFont5x7);
	#endif
	tft.fillScreen(WHITE);
	tft.setTextColor(BLACK, WHITE);
	tft.setCursor(32,100);
	tft.println(F("Ardtendo(c)"));
	tft.setCursor(32,112);
	tft.println(F("ardtendo.de"));
	pinMode(LEFT_KEY, INPUT_PULLUP);
	pinMode(UP_KEY, INPUT_PULLUP);
	pinMode(RIGHT_KEY, INPUT_PULLUP);
	pinMode(DOWN_KEY, INPUT_PULLUP);
	pinMode(START_KEY, INPUT_PULLUP);
	pinMode(SELECT_KEY, INPUT_PULLUP);
	pinMode(B_KEY, INPUT_PULLUP);
	pinMode(A_KEY, INPUT_PULLUP);
	delay(150);
	uint16_t colors[] = {YELLOW, RED, MAGENTA, GREEN, BLUE};
	#ifdef SIMPLE_SETUP
	tft.setTextSize(2);
	char* gb = "HACK BOY";
	
	for (uint8_t i = 0; i < 20; i++) {
		tft.setCursor(15,50);
		for (uint8_t c = 0; c < 8; c++) {
			if (i - c < 0) break;
			tft.setTextColor(colors[min((i-c)/2, 4)]);
			tft.write(gb[c]);
		}
		delay(50);
	}
	#else
	static const uint8_t logo [21][16] = { 
		{ 0x0,  0x0, 0x30,  0x0,  0x0,  0x0,  0x0,  0x0,  0x0,  0x0,  0x0,  0x0,  0x6,  0x0,  0x0,  0x0},
		{ 0x0,  0x7, 0xfe, 0x1f, 0xc0, 0x7c, 0x1f, 0x87, 0xff, 0x81, 0xff, 0xc0, 0x3f, 0xcf, 0x83, 0xe0},
		{ 0x0, 0x1f, 0xfe, 0x1f, 0xc0, 0x7c, 0x1f, 0x87, 0xff, 0x81, 0xff, 0xe0, 0xff, 0xef, 0x87, 0xc0},
		{ 0x0, 0x7f, 0xfe, 0x1f, 0xc0, 0xfc, 0x1f, 0x87, 0xff, 0x81, 0xff, 0xf1, 0xff, 0xf7, 0x8f, 0xc0},
		{ 0x0, 0xff, 0xfe, 0x3f, 0xc0, 0xfe, 0x3f, 0x87, 0xff,  0x3, 0xff, 0xf1, 0xff, 0xff, 0xcf, 0x80},
		{ 0x1, 0xfc,  0x0, 0x3f, 0xc0, 0xfe, 0x3f, 0x87, 0x80,  0x3, 0xe1, 0xf3, 0xe0, 0xfb, 0xdf,  0x0},
		{ 0x1, 0xf8,  0x0, 0x3f, 0xc1, 0xfe, 0x3f, 0xc7, 0x80,  0x3, 0xe0, 0xf3, 0xe0, 0x7b, 0xff,  0x0},
		{ 0x3, 0xe0,  0x0, 0x7f, 0xc1, 0xfe, 0x7f, 0xcf, 0x80,  0x3, 0xe1, 0xf7, 0xc0, 0x7b, 0xfe,  0x0},
		{ 0x3, 0xe0,  0x0, 0x7b, 0xe1, 0xfe, 0x7f, 0xcf, 0xfe,  0x3, 0xff, 0xf7, 0xc0, 0x7d, 0xfc,  0x0},
		{ 0x7, 0xc0,  0x0, 0xfb, 0xe3, 0xfe, 0xff, 0xcf, 0xfe,  0x3, 0xff, 0xe7, 0x80, 0x7d, 0xfc,  0x0},
		{ 0x7, 0xc0,  0x0, 0xf1, 0xe3, 0xfe, 0xf7, 0xcf, 0xfe,  0x3, 0xff, 0xe7, 0x80, 0x78, 0xf8,  0x0},
		{ 0x7, 0x83, 0xfd, 0xff, 0xe3, 0xdf, 0xf7, 0xcf, 0xfc,  0x7, 0xff, 0xe7, 0x80, 0x78, 0xf8,  0x0},
		{ 0x7, 0x83, 0xfd, 0xff, 0xe7, 0xdf, 0xf7, 0xcf, 0xfc,  0x7, 0xff, 0xf7, 0x80, 0x79, 0xfc,  0x0},
		{ 0x7, 0xc3, 0xfd, 0xff, 0xe7, 0xcf, 0xe7, 0xcf,  0x0,  0x7, 0xc1, 0xf7, 0x80, 0xf9, 0xfc,  0x0},
		{ 0x7, 0xc3, 0xfb, 0xff, 0xe7, 0x8f, 0xe3, 0xdf,  0x0,  0x7, 0xc1, 0xf7, 0x80, 0xfb, 0xfe,  0x0},
		{ 0x3, 0xe0, 0x7b, 0xe1, 0xff, 0x8f, 0xe3, 0xdf,  0x0,  0x7, 0xc1, 0xf7, 0xc1, 0xf3, 0xfe,  0x0},
		{ 0x3, 0xf8, 0xff, 0xc1, 0xff, 0x8f, 0xc3, 0xdf, 0xfe,  0x7, 0xff, 0xe7, 0xf7, 0xf7, 0xcf,  0x0},
		{ 0x1, 0xff, 0xff, 0xc1, 0xff,  0xf, 0xc3, 0xdf, 0xfe,  0x7, 0xff, 0xe3, 0xff, 0xe7, 0x8f, 0x80},
		{ 0x0, 0xff, 0xff, 0x80, 0xff,  0xf, 0x83, 0xdf, 0xfc,  0xf, 0xff, 0xc1, 0xff, 0xcf, 0x87, 0x80},
		{ 0x0, 0x7f, 0xff, 0x80, 0xff,  0x7, 0x83, 0xdf, 0xfc,  0xf, 0xff, 0x80, 0xff,  0xf,  0x7, 0xc0},
		{ 0x0, 0x1f, 0xcf, 0x80, 0xfe,  0x7, 0x83, 0xdf, 0xfc,  0xf, 0xfe,  0x0, 0x3c, 0x1f,  0x3, 0xc0}
	};
	uint16_t color;
	uint32_t nextFrame;
	for (int8_t i = 0; i < 50; i++) {
		nextFrame = millis()+40;
		for (uint8_t col = 0; col < 16; col++) {
			uint8_t prev = ((i-1)/2 - col) / 2;
			uint8_t now  = (i/2 - col) / 2;
			if (prev == now && now != 0) continue;
			if (prev  >= 4) continue;
			if (i/2 < col) break;
			color = colors[min(now, 4)];
			for (uint8_t row = 0; row < 21; row++) {
				for (uint8_t b = 7; b < 8; b--) {
					if ((logo[row][col] >> b) & 1) {
						tft.drawPixel(col * 8 + 7 - b, 50 + row, color);
					}
				}
			}
		}
		while (millis() < nextFrame);
	}
	#endif
	soundEnabled = !getSelectKey();
	noteDelay(NOTE_C, 5, 100);
	noteDelay(NOTE_C, 6, 200);
	delay(1500);
	tft.fillScreen(WHITE);
}

uint8_t HackBoyC::startMultiplayer() {
	tft.fillScreen(BLACK);
	tft.setTextColor(WHITE);
	tft.setTextSize(3);
	tft.setCursor(38,40);
	tft.print(F("> <"));
	tft.setTextSize(1);
	tft.setCursor(32,64);
	tft.print(F("Mehrspieler"));
	tft.setCursor(12,72);
	tft.print(F("Auf Gegner warten"));
	tft.setTextSize(3);
	uint8_t playerN = 1;
	unsigned long started = millis();
	//while(Serial.available()) Serial.read();
	if (Serial.peek() != 1)
		Serial.write(1);
	while (true) {
		tft.setTextColor((millis()/300)%2 ? WHITE : BLACK);
		tft.setCursor(40,40);
		tft.print(" ?");
		if (Serial.available()) {
			char c = Serial.read();
			switch (c) {
			case 1:
				playerN = 2;
				Serial.write(2);
				break;
			case 2:
				Serial.write(3);
			case 3:
				tft.fillScreen(BLACK);
				return playerN;
			}
		}
		if (started+10000 < millis()) {
			break;
		}
	}
	tft.setTextColor(BLACK);
	tft.setCursor(40,40);
	tft.print(F(" ?"));
	tft.setTextColor(WHITE);
	tft.setCursor(40,40);
	tft.print(F(" X"));
	tft.setTextSize(1);
	tft.setCursor(44,80);
	tft.print(F("Fehler:"));
	tft.setCursor(5,88);
	switch (playerN) {
		case 1:
			tft.print(F("Kein Gegner gefunden"));break;
		case 2:
			tft.print(F("    Keine Antwort   "));break;
	}
	while (!getStartKey());
	return false;
}

uint8_t HackBoyC::getMovement() {
	return (getLeftKey()*LEFT_KEY_BIT)|(getUpKey()*UP_KEY_BIT)|(getRightKey()*RIGHT_KEY_BIT)|(getDownKey()*DOWN_KEY_BIT);
}

boolean HackBoyC::getLeftKey() {
	//#ifdef ARDUINO_SAM_DUE
	//return digitalRead(LEFT_KEY);
	//#else
	return !digitalRead(LEFT_KEY);
	//#endif
}
boolean HackBoyC::getUpKey() {
	//#ifdef ARDUINO_SAM_DUE
	//return digitalRead(UP_KEY);
	//#else
	return !digitalRead(UP_KEY);
	//#endif
}
boolean HackBoyC::getRightKey() {
	//#ifdef ARDUINO_SAM_DUE
	//return digitalRead(RIGHT_KEY);
	//#else
	return !digitalRead(RIGHT_KEY);
	//#endif
}
boolean HackBoyC::getDownKey() {
	//#ifdef ARDUINO_SAM_DUE
	//return digitalRead(DOWN_KEY);
	//#else
	return !digitalRead(DOWN_KEY);
	//#endif
}
boolean HackBoyC::getStartKey() {
	//#ifdef ARDUINO_SAM_DUE
	//return digitalRead(START_KEY);
	//#else
	return !digitalRead(START_KEY);
	//#endif
}
boolean HackBoyC::getSelectKey() {
	//#ifdef ARDUINO_SAM_DUE
	//return digitalRead(SELECT_KEY);
	//#else
	return !digitalRead(SELECT_KEY);
	//#endif
}
boolean HackBoyC::getBKey() {
	//#ifdef ARDUINO_SAM_DUE
	//return digitalRead(B_KEY);
	//#else
	return !digitalRead(B_KEY);
	//#endif
}
boolean HackBoyC::getAKey() {
	//#ifdef ARDUINO_SAM_DUE
	//return digitalRead(A_KEY);
	//#else
	return !digitalRead(A_KEY);
	//#endif
}

int16_t HackBoyC::recvMessage() {
	unsigned long started = millis();
	while (started+1000 > millis()) {
		if (Serial.available()) {
			Serial.write(255);
			return Serial.read();
		}
	}
	return -1;
}

boolean HackBoyC::sendMessage(byte msg) {
	Serial.write(msg);
	unsigned long sent = millis();
	while (sent+1000 > millis()) {
		if (Serial.available()) {
			if (Serial.read() == 255) return true;
		}
	}
	tft.fillScreen(BLACK);
	tft.setTextColor(WHITE);
	tft.setTextSize(3);
	tft.setCursor(40,40);
	tft.print(F(">X<"));
	tft.setTextSize(1);
	tft.setCursor(6,64);
	tft.print(F("Verbindung verloren"));
	tft.setCursor(10,72);
	tft.print(F("Gleiches Spiel an?"));
	while(!getStartKey());
	return false;
}

void HackBoyC::note(float noteHz, uint16_t duration) {
	note(noteHz, 4, duration);
}

void HackBoyC::note(float noteHz, uint8_t octave, uint16_t duration) {
	if (!soundEnabled) return;
	if (octave >= 4)
		tone(TONE_PIN, noteHz * (1 << (octave - 4)), duration);
	else
		tone(TONE_PIN, noteHz / (1 << (4 - octave)), duration);
}

void HackBoyC::noteDelay(float noteHz, uint16_t duration) {
	noteDelay(noteHz, 4, duration);
}

void HackBoyC::noteDelay(float noteHz, uint8_t octave, uint16_t duration) {
	note(noteHz, octave, duration);
	delay(duration*1.3);
}

HackBoyC HackBoy = HackBoyC();

#ifdef ARDUINO_SAM_DUE
	ILI9341_due tft = ILI9341_due(__CS, __DC, __RST);
#else
	TFT_ILI9163C tft = TFT_ILI9163C(__CS, __DC);
#endif
