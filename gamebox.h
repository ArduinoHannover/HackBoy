#ifndef GAMEBOX_H
#define GMAEBOX_H

#include <SPI.h>
#include <TFT_ILI9163C.h>
#include <Adafruit_GFX.h>

#define __CS 10
#define __DC 9

//#define SIMPLE_SETUP

#define	BLACK   0x0000
#define	BLUE    0x001F
#define	RED     0xF800
#define	GREEN   0x07E0
#define CYAN    0x07FF
#define MAGENTA 0xF81F
#define YELLOW  0xFFE0
#define WHITE   0xFFFF

#define LEFT_KEY	4
#define UP_KEY		5
#define RIGHT_KEY	6
#define DOWN_KEY	7
#define START_KEY	A0
#define SELECT_KEY	A1
#define B_KEY		A2
#define A_KEY		A3

#define LEFT_BIT	0b1000
#define UP_BIT		0b0100
#define RIGHT_BIT	0b0010
#define DOWN_BIT	0b0001

class gamebox {
	private:
	public:
		gamebox(void);
		void begin(void);
		uint8_t startMultiplayer(void);
		uint8_t getMovement(void);
		boolean getLeftKey(void);
		boolean getUpKey(void);
		boolean getRightKey(void);
		boolean getDownKey(void);
		boolean getStartKey(void);
		boolean getSelectKey(void);
		boolean getBKey(void);
		boolean getAKey(void);
		int16_t recvMessage(void);
		boolean sendMessage(byte);
};

extern gamebox Gamebox;
extern TFT_ILI9163C tft;

#endif //GAMEBOX_H
