#ifndef NHDLCD9_h
#define NHDLCD9_h

#include "Arduino.h"
#include "SoftwareSerial.h"

/******************************************************************************************************/
/* NHDLCD9 -- manages the NewHaven Design SerLCD, based on SoftwareSerial to aid pinning and printing */
/*                                                                                                    */
/*     some cmds are cached so repeated calls will not actually be sent which can cause               */
/*     flickering of the display, printed values are not cached and are always sent                   */
/*                                                                                                    */
/*     autoOn: turn off the display and turn it back on with the next command                         */
/*                                                                                                    */
/*     posBase: cursor positioning via 0x0 or 1x1                                                     */
/*                                                                                                    */
/*     on/off: display of characters, not backlight                                                   */
/*                                                                                                    */
/*     bright: backlight control, by percentage                                                       */
/*                                                                                                    */
/*     scrolling: scrolling is slow because of the amount of time the LCD takes to redraw.            */
/*     scrolling is persistant and moves the x-origin a single column at a time                       */
/*                                                                                                    */
/******************************************************************************************************/

class NHDLCD9 : public SoftwareSerial
{
private:
   int _bv[10];
   int _ro[5];
   void command(uint8_t);

public:
   NHDLCD9(int pin, int numRows, int numCols, int posBase = 1);
   void setup(int brightPcnt = 100, boolean startEmpty = true);

   void on();
   void off();

   void empty();

   // void scrollLeft ();
   // void scrollRight ();

   void bright(int pcnt);
   void oldbright(int pcnt);
   void contrast(int contrastval);
   void pos(int row, int col);

   void cursorUnderline();
   void cursorBlock();
   void cursorOff();

   // shortcuts for printing at particular positions
    template<typename T>
    void at(int row, int col, const T& value) {
        pos(row, col);
        print(value);
    }
    
    template<typename T>
    void at(int row, int col, const T& value, int base) {
        pos(row, col);
        print(value, base);
    }
};

#endif
