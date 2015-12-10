#ifndef GAMEBOX_H
#define GMAEBOX_H

#include <SPI.h>

#ifndef ARDUINO_SAM_DUE
	//Don't include those Libraries
	#define _ILI9341_dueH_
	#define _ILI9341_due_configH_
	#define SYSTEM5x7_H
#endif

#ifdef ARDUINO_SAM_DUE
	#define TEXT_SCALING_ENABLED
	#include "ILI9341_due_config.h"
	#include "ILI9341_due.h"
	#include "SystemFont5x7.h"
	#define setCursor cursorToXY
	#define setTextSize setTextScale
#else
	#include "TFT_ILI9163C.h"
	#include "Adafruit_GFX.h"
	#define TONE_PIN 3
#endif

#define __CS 10
#define __DC 9
#define __RST 12

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


#define LEFT_KEY_BIT	0b1000
#define UP_KEY_BIT		0b0100
#define RIGHT_KEY_BIT	0b0010
#define DOWN_KEY_BIT	0b0001

class HackBoyC {
	private:
		boolean soundEnabled;
	public:
		HackBoyC(void);
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
		void note(float, uint16_t);
		void note(float, uint8_t, uint16_t);
		void noteDelay(float, uint16_t);
		void noteDelay(float, uint8_t, uint16_t);
};

extern HackBoyC HackBoy;

#ifdef ARDUINO_SAM_DUE
extern ILI9341_due tft;
#else
extern TFT_ILI9163C tft;
#endif


#define NOTE_C		261.63
#define NOTE_CIS	277.18
#define NOTE_DES	NOTE_CIS
#define NOTE_D		293.66
#define NOTE_DIS	311.13
#define NOTE_ES	NOTE_DIS
#define NOTE_E		329.63
#define NOTE_F		349.23
#define NOTE_FIS	369.99
#define NOTE_GES	NOTE_FIS
#define NOTE_G		392.00
#define NOTE_GIS	415.30
#define NOTE_AS		NOTE_GIS
#define NOTE_A		440.00
#define NOTE_AIS	466.16
#define NOTE_B		NOTE_AIS
#define NOTE_H		493.88

#endif //GAMEBOX_H
