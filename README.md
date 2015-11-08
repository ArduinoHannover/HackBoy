# Hack Boy

All external libraries neede are included. Just go ahead and include `<SPI.h>` and `<HackBoy.h>`.



## Guide for writing Games

First of all, great that you're interested! As there will be two hardware versions - one with a ATmega328 and 128x128 screen and another with ATSAM3X8E (Arduino DUE) coming with a 240 by 320 screen - please write the game logic independent from the graphics. How do I do that?

* In Arduino create a new Tab (top right drop down arrow), name it mygame.cpp [replace mygame with your games title]. There you can write down all the logic and variables needed. Now create another file mygame.h. Put in all your function prototypes [see note below] and static variables/defines etc. Include that with `#include "mygame.h"`

* The game logic should call the draw jobs. So create the draw functions either in the main .ino Sketch or a separate .cpp file. If you have to access variables of your game logic, then you have to write a getter for that [if your game logic isn't a class and the parameters you need to access public]. Generally, if you have to access anything of the game logic, you'll have to include mygame.h in the drawing file too.

* Now that you've written those things down - your game logic doesn't yet know about the functions provided by the drawing one. Create a .h file for that too and include it in either mygame.h or mygame.cpp

Have a look at the BareBasic or BareBasicClass [recommend] Sketch included.

### Function Prototypes

Function prototypes declare that a function exists. So for a function `void move(uint8_t steps) {...}` your prototype will be `void move(uint8_t);`.
