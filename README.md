# Hack Boy

## Quickstart
Download Library ("Download ZIP"), unpack it, remove the nasty branch name -master from the folder. Then open Arduino and click Sketch > Include Library > Add ZIP Library… and choose the folder you just extracted. Now you'll find a new entry under File > Examples called HackBoy (surprise!). You can now choose any game in there and copy it to your HackBoy.
Sound is annoying as the orange? Press SELECT on Boot to mute (until reboot).

## Arduino-Beginner Quickstart
Opening Arduino IDE the first time? No problem. Copying a game to your HackBoy is as easy as toast. Close to.

Just connect your HackBoy via a USB-Cable (on BETA connect the FTDI-Adaptor, GND goes to BLK, DTR to GRN. Make sure, the switch is off or battery disconnected – but if your BETA user I hope that you've seen this environment before.). A new device will pop up under Tools > Port. Take the chance and select it!

For UNO users: Select Arduino Nano from Tools > Board (ATmega328 should be selected by default as processor), then hit upload. You *may* need to hold the select button down until the log shows "This Sketch uses ..." (your game is compiled and ready to move via the tiny wires).

For DUE users: Select Ardunio DUE (Native USB Port) from Tools > Board and hit Upload. You *may* need to press the erase button first.

## Builtin Functions
The library offers methods to improve compatibility and readability.

| Method `HackBoy.` | Function |
|------------------------------|-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------|
| `void begin()` | Always call in setup routine. Initializes the display and Pull-Ups. |
| `booelan get*Key()` | Returns `true` if Button is pressed. `*` can be Up, Right, Down, Left, Start, Select, A, B. |
|  |  |
| `booelan startMultiplayer()` | Shows Screen that it is starting Multiplayer. Does a Handshake with connected device with a timeout of 10s. Returns true if handshake was successful (otherwise you may send data to nirvana). |
| `boolean sendMessage(byte)` | Sends a Message to connected device, returns true if sent successfully (timeout max. 1s) |
| `int16_t recvMessage()` | Returns byte (0-255) if successful, -1 if failed to receive message. Must be called of receiving device, otherwise `sendMessage()` will timeout and return false. |
| `note(float, uint16_t)` | Plays a note (float = NOTE_C, NOTE_CIS...) or any frequency with a duration, specified by uint16_t. Optionally in a different octave (0-8 will be fine), specified by the uint8_t: `note(float, uint8_t, uint16_t)`. Optional delay: `noteDelay(float, uint16_t)` or `noteDelay(float, uint8_t, uint16_t)`. |

## Guide for writing Games

First of all, great that you're interested! As there will be two hardware versions - one with a ATmega328 and 128x128 screen and another with ATSAM3X8E (Arduino DUE) coming with a 240 by 320 screen. There may be pin changes, so be sure to use the builtin functions.

All external libraries needed are included directly. Just go ahead and include `<SPI.h>` and `<HackBoy.h>`.

If you can, write the game logic independent from the graphics. How do I do that?

* In Arduino create a new Tab (top right drop down arrow), name it mygame.cpp [replace mygame with your games title]. There you can write down all the logic and variables needed. Now create another file mygame.h. Put in all your function prototypes [see note below] and static variables/defines etc. Include that with `#include "mygame.h"`

* The game logic should call the draw jobs. So create the draw functions either in the main .ino Sketch or a separate .cpp file. If you have to access variables of your game logic, then you have to write a getter for that [if your game logic isn't a class and the parameters you need to access public]. Generally, if you have to access anything of the game logic, you'll have to include mygame.h in the drawing file too.

* Now that you've written those things down - your game logic doesn't yet know about the functions provided by the drawing one. Create a .h file for that too and include it in either mygame.h or mygame.cpp

~~Have a look at the BareBasic or BareBasicClass [recommend] Sketch included.~~ ***[TODO]***

Start a Pull request with your game added to the examples folder.

### Function Prototypes

Function prototypes declare that a function exists. So for a function `void move(uint8_t steps) {...}` your prototype will be `void move(uint8_t);`. But the easiest learning is provided by taking a look at the examples ;-)
