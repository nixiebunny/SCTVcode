//  SCTVcode.ino  SCTV Scope Clock program
//
// Copyright (C) 2008,2016,2019,2021 David Forbes
//
// This program is free software; you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation; either version 2 of the License, or
// (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// --------------------- Version info -----------------------

// V 1.00  4/07/08 DF  Initial release
// V 2.00  1/27/16 DF  working on Arduino C version
//         1/29/16 DF  Code compiles, DoSeg works as to timing; actual circles await a PC board
//         2/13/16 DF  It can draw a circle, but nothing else. 
//         2/16/16 DF  Text and menus work (still a centering bug), doing RTC
// V 3.00 11/17/19 DF  Converting to Teensy 3.6, software circle generator
// V 3.10 12/23/19 DF  It's starting to work, still need to fix centering bug
// V 3.20 12/26/19 DF  Adding software centering, prelude to Pong capability
// V 3.21 09/04/21 DF  Changed DAC scale in draw code to handle new position system
//                     Pong is incorporated into drawlist, needs scoring work
// V 3.22 09/05/21 DF  Pong has scoring, working on hands clock. Done. Had Shape bug. 
// V 3.30 09/09/21 DF  Changing screen field to 2560, add a 0 to all x,y values, change byte to int
// V 0.4.0 09/10/21 DF  Removed middle, fixed weekday, tweaked Pong
// V 0.4.1 09/11/21 DF  Fixed WDayStr length, leading zeroes, doSeg
// V 0.4.2 09/12/21 DF  Added haikus, Tetris objects, split into files
// V 0.4.3 09/24/21 DF  Improved Tetris
// V 0.5.0 09/29/21 DF  Adding GPS over USB for SCTV-C
// V 0.5.1 09/30/21 DF  GPS works, can be plugged and unplugged  
// V 0.5.2 10/01/21 DF  Improved Pong end of game, added ball delay 
// V 0.5.3 11/23/21 DF  Added 50 Hz menu option for our worldwide friends 
// V 1.0.0 11/30/21 DF  Made screensaver bigger, issued as a release
// V 1.0.1 12/20/21 DF  Made the locale data stored in the RTC chip
// V 1.0.2 01/21/22 DF  Fixed brightness on 0 via stride, moved tails on 6,9
// V 1.0.3 02/12/22 DF  Fixed GPS startup by making splash screen faster, improved 6,9 some more
// V 1.0.4 02/13/22 DF  Gated USB accesses with userial active test

char versionNo[]  = "Version 1.0.4\n";

// THINGS TO DO

// Add FLW

//---------------------- Description -----------------------

// Target CPU: Teensy 3.6
// 32 bit ARM 
// Clock speed: 180 MHz

// Basic description

// The scope clock uses circles to display the time.
// The circle generator is the Teensy's built-in dual DAC. 
// sin and cos lookup tables allow a DAC update rate of ~1 MHz. 
// Lines are drawn with the length calculated using Pythagoras when needed (not horiz or vert). 
// 
// The coordinate system is (0,0) at center of screen. 
// The display scaling is .001" per unit, with active area of 2500x2500 units. 
// All coordinates are 32 bit integers. 

// The display timing has been tweaked to make clean circles. There 
// is a bit of extra delay time provided for the CRT beam to move while blanked, 
// which allows the use of simple low-slew-rate deflection amplifiers. 
//
// Timekeeping
//
// The timebase is a DS3232 RTC chip. It's read once per display loop. 
// Ther is currently no DST. 
// DST may be added with lookup tables based on political regions.
//
// Draw list
//
// Text display is done with a draw list of ASCII character codes.
// The clock displays are draw lists with their text strings modified
// by the timekeeping code.
//
// All text strings are centered in the display for best appearance.
//
// The draw list contains one or more entries per line of text. Each
// string is a zero-terminated string of ASCII characters. The last string
// on each line contains a NL before the 0. The draw list contains
// the X,Y start position of the string, the scale factor, the text
// pointer, and code pointer in case it's a menu or field item.
//
// There are non-text draw list items as well. These are things such
// as clock face markers, outlines, etc.
//
// The Center routine centers the text items in X and Y
// on a line-by-line basis.
//
// The draw lists live in RAM so that the display code can
// pre-calculate the X,Y start position of each line and update the
// menu item selection status.
//
//
// ---------------------------- Hardware definitions -----------------------------


#include <Arduino.h> 
#include <Wire.h>    // I2C library
#include "USBHost_t36.h"
#include <TinyGPS.h>

// if old board with weird position ADC pins
// #define SCTVA

 // The clock chip's I2C bus address
#define DS3232_ADDRESS  0x68 

// Teensy 3.6 pin definitions
int BlankPin   =  2;   // high blanks the display
int encButPin  = 14;   // encoder button, 0 = pressed
int encBPin    = 15;   // encoder quadrature
int encAPin    = 16; 
int XPosPin    = A15;  // horizontal centering knob
#ifdef SCTVA
  int YPosPin  = A18;  // vertical centering knob on SCTVA is in odd place
#else
  int YPosPin  = A16;  // vertical centering knob
#endif
int XDACPin    = A21;  // DAC for x axis
int YDACPin    = A22;  // DAC for y axis
int randPin    = A12;   // random numbers are read from the air here

bool doingHand = true;   // diagnostic print for drawing code

// new variables for software circle drawing
float pi = 3.141592;
int thisX, thisY;    // circle position
int i;
const int nsteps = 1024;   // was 240 to be consistent with the old circle angle definition

// the sin/cos lookup table outputs are signed ints in range from -65536 to +65536
int sintab[nsteps];
int costab[nsteps];  // scaled int angle lookup tables, filled in during init

// --------------------------- RAM variables --------------------------

// display parameters
const int middle = 1280;      // X,Y middle of visible screen
const int midDAC = 2048;      // X,Y middle of dispaly
const int motionDelay = 15;  // how fast the beam will get there before enabling
const int settlingDelay = 8;  // let the beam finish its move before turning on
const int glowDelay   =  2;  // was 7 usec to wait for beam to get bright before moving
const int circleSpeed = 200;   // angular step; bigger makes circles draw faster and more coarsely
const int lineStride  =  1;   // linear step; bigger makes lines draw faster and more coarsely

int xPos;     // where the knob says to be
int yPos;     // -512 to +511 range (10 bit ADC code)

// timekeeping variables
int Hr12 = 1;       // 1 if 12 hour mode, 0 if 24 hour mode
int Century = 20;   // century range 0-99 (not Y10K compliant!)
int Years = 21;     // years range 0-99
int Mons = 1;       // months range 1-12
int WDay = 0;       // day of week, 0=Sunday
int Days = 1;       // days range 1-31
int Hrs = 0;        // hours range 0-23
int Mins = 0;       // minutes range 0-59            All of these have signed math when menu modifies them! 
int Secs = 0;       // seconds range 0-59
int Ticks = 0;      // ticks range 0-59 (0-49 if 50 Hz)
int Jiffies = 0;    // 300 Hz jiffies 0-4 or 0-5
int Blink;          // increments at tick rate, use Blnkbit to blink
int HalfSec = 30;   // 30 for 60 Hz or 25 for 50 Hz
unsigned int Hertz = 60;     // 50 or 60
int JifTick = 5;    // 5 or 6 - jiffies per tick depends on Hz
int ScrX;            // screensaver position table index 0..ScrTabL
int XSaver;          // current X offset of screensaver
int YSaver;          // current Y offset of screensaver
int DST = 0;        // daylight savings, 0 or 1 hour advance
int Zone = -7;      // time zone, -12 to +12 hours
int ZMins = 0;      // zone minutes, 0,15,30,45
int rtcMagic = 0x33;   // magic number for verifying RTC data
int rtcValid = 0;   // set to rtcMagic when RTC data written to tell if it's trustworthy

int HundrSec = 0;   // hundredths of seconds, maybe not needed but GPS provides it
unsigned long GPSage;  // GPS time since last valid reading, may indicate invalid


unsigned int lastMicros;     // for display refresh

// Rotary encoder
int EncDir = 0;      // initial position
int ButHist = 0;     // button pressing history
int LastEnc;        // encoder previous state

// Menu processing variables
int HotItem;         // menu item currently highlighted
int MenuCod;         // the address to execute when menu selected
int theClock;        // which clock face to show
int NItems;          // how many items in menu to make hot, found by DoAList???

// Text string variables
char *StrPtr;        // points to text string location
char TheChr;         // current character ASCII code
int ChrXPos;         // X position of LL corner of this character
int ChrYPos;         // Y position
bool NewL;           // newline seen by GetWid for Center

// String formatting variables
int Scale;          // Font scale factor * 20 pixels high
int ChrHt;          // scaled character height in pixels (uppercase)
int ChrWid;         // scaled character width in pixels (variable)
int RowGap;         // scaled row gap in pixels (vert. kern)
int Kern;           // scaled space between chars in pixels
int DispHt;         // height of entire draw list display
int ChrCnt;         // String character count from GetWid
int StrWid;         // scaled width of string in pixels from GetWid

// The current segment being displayed
const int * TheSeg;        // pointer to our segment data in Fonts
int XCenter;        // X coord of circle center
int YCenter;        // Y coord of circle center
int XSize;          // Width of circle
int YSize;          // Height of circle
int XStart;         // X start of a line
int YStart;         // Y start of a line
int XEnd;           // X end of a line
int YEnd;           // Y end of a line
int Shape;          // shape code: lin/cir
int FirstO;         // first octant to display
int LastO;          // last octant to display

// The text, menu and field items are similar in format.
// xpos and ypos may be zero for the centering code to figure out.
// Segments are not centered.
// Any items after the first segment item will not be centered.
// They need a real xpos and ypos specified.

// A draw list is a list of items to be drawn. These comprise:
const int listend = 0;  //                                         end of list
const int text    = 1;  // size, zero,     stringptr, xpos, ypos   text item, no menu
const int menu    = 2;  // size, function, stringptr, xpos, ypos   text item with menu
const int field   = 3;  // size, function, stringptr, xpos, ypos   changeable field text
const int seg     = 4;  // size, function, segptr,    xpos, ypos   segment (special character)

// Each item of a drawlist is one of these:
struct item {
  int type;        // see list above
  int scale;       // scale factor
  int func;        // function to execute (mod or menu numeration) if used, 0 if not
  char *string;     // the string to display
  int xpos;        // where it goes, or used when calculating where it goes
  int ypos;
};


const int maxItems = 20;    // a list is limited to this many things

struct item TheList[maxItems];    // the list above is copied into here to allow modification

item * whichList;  // pointer to current draw list

// Menu flag bits are now variables
bool InMenu = false;     // 1 if currently in a menu at all
bool InField = false;    // 1 if currently in a parameter field
bool InParam = false;    // 1 if in param mode
bool pushed = false;     // 1 if button was pushed this frame

int BlnkBit = 3;     // bit of Blink to use for blinking
int blinkCount = 0;   // when to blink

int frame = 0;   // for diagnostic printing one time only
int beforeTime = 0;
int afterTime = 0;

// Shape is code for what type of thing to draw
const int lin = 0;   //  line
const int cir = 1;   // circle or arc

// --------------------------------- USB port -----------------------------

uint32_t usbBaudRate = 4800;
uint32_t format = USBHOST_SERIAL_8N1;
USBHost myusb;
USBHub hub1(myusb);
USBHub hub2(myusb);
USBHIDParser hid1(myusb);
USBHIDParser hid2(myusb);
USBHIDParser hid3(myusb);
USBSerial userial(myusb);  // works only for those Serial devices who transfer <=64 bytes (like T3.x, FTDI...)
//USBSerial_BigBuffer userial(myusb, 1); // Handles anything up to 512 bytes
//USBSerial_BigBuffer userial(myusb); // Handles up to 512 but by default only for those > 64 bytes

USBDriver *drivers[] = {&hub1, &hub2, &hid1, &hid2, &hid3, &userial};
#define CNT_DEVICES (sizeof(drivers)/sizeof(drivers[0]))
const char * driver_names[CNT_DEVICES] = {"Hub1", "Hub2",  "HID1", "HID2", "HID3", "USERIAL1" };
bool driver_active[CNT_DEVICES] = {false, false, false, false, false, false};

// --------------------------------- Serial2 port -----------------------------

uint32_t serial2BaudRate = 9600;

// ----------------------- DS3232 time keeping code ----------------------------------

// Make day of week from date variables
int GetWDay(int d, int m, int yr, int cent) 
{
  int weekday;
  int y = cent*100 + yr;
  weekday  = (d += m<3 ? y-- : y-2, 23*m/9 + d + 4 + y/4 - cent + cent/4) % 7;
  return weekday;
}

// Sets the RTC to the current time, called when exiting the time setting menu
void writeRTCtime()
{ 
  int yr, mo, dy, dw, hr, mi, sc; 

  // convert our decimal values to BCD
  sc = Secs%10 + ((Secs/10) << 4);   // run enable: bit 7 = 0
  mi = Mins%10 + ((Mins/10) << 4);
  hr = Hrs%10  + ((Hrs/10) << 4);   // 24 hour mode: bit 6 = 0
  dw = WDay;
  dy = Days%10  + ((Days/10) << 4);
  mo = Mons%10  + ((Mons/10) << 4);
  yr = Years%10 + ((Years/10) << 4);

  Wire.beginTransmission(DS3232_ADDRESS);
  Wire.write(0); // register select
  Wire.write(sc);  // write registers 0 through 6 in order
  Wire.write(mi);
  Wire.write(hr);
  Wire.write(dw);
  Wire.write(dy);
  Wire.write(mo);
  Wire.write(yr);
  Wire.endTransmission();
}

// Sets the RTC to the current locale, called when exiting the locale setting menu
void writeRTClocale()
{ 
  int zn = Zone + 30;  // make it fit in an unsigned byte so it won't get garbled
  
  Wire.beginTransmission(DS3232_ADDRESS);
  Wire.write(0x18); // register select is in the SRAM region of the DS3232
  Wire.write(rtcMagic);   // flag the RTC data as being valid
  Wire.write(Century);  // write registers 0x18 through 6 in order
  Wire.write(zn);
  Wire.write(ZMins);
  Wire.write(Hr12);
  Wire.write(Hertz);
  Wire.endTransmission();
}

// Reads the DS3232 via I2C and fills time variables with binary data
void readRTCtime()
{
 // int yr, mo, dy, dw, hr, mi, sc; // not using day of week now... fix this!!!
  int yr, mo, dy, hr, mi, sc; 

  Wire.beginTransmission(DS3232_ADDRESS);
  Wire.write(00);  // read data starting at byte 00
  Wire.endTransmission();

  Wire.requestFrom(DS3232_ADDRESS, 7, 1);  // request seven bytes, then stop
  sc = Wire.read();  // second
  mi = Wire.read();  // minute
  hr = Wire.read();  // hour
  Wire.read();  // day of week
 // dw = Wire.read();  // day of week
  dy = Wire.read();  // day of month
  mo = Wire.read();  // month
  yr = Wire.read();  // year

  Secs  = ((sc & 0x0f) + ((sc & 0x70) >> 3) * 5);  // do some BCD to binary
  Mins  = ((mi & 0x0f) + ((mi & 0x70) >> 3) * 5);
  Hrs   = ((hr & 0x0f) + ((hr & 0x30) >> 3) * 5);
  Days  = ((dy & 0x0f) + ((dy & 0x30) >> 3) * 5);
  Mons  = ((mo & 0x0f) + ((mo & 0x10) >> 3) * 5);
  Years = ((yr & 0x0f) + ((yr & 0x70) >> 3) * 5);
  WDay  = GetWDay(Days, Mons, Years, Century);
}

// Reads the DS3232 via I2C and fills locale variables if they're valid, otherwise skips
void readRTClocale()
{
  Wire.beginTransmission(DS3232_ADDRESS);
  Wire.write(0x18);  // read locale data starting at byte 0x18
  Wire.endTransmission();

  Wire.requestFrom(DS3232_ADDRESS, 6, 1);  // request seven bytes, then stop
  if (Wire.read() == rtcMagic) {
    Century = Wire.read();
    Zone    = Wire.read() - 30;
    ZMins   = Wire.read();
    Hr12    = Wire.read();
    Hertz   = Wire.read();
    rtcValid = 1;
  }
  else {
    Wire.read();
    Wire.read();
    Wire.read();
    Wire.read();
    Wire.read();
    rtcValid = 0;
  }
}

// ------------------------------------ GPS handler ---------------------------------

TinyGPS myGps;

// GPS variables
int GPSHrs;         // Hour from GPS receiver
int GPSMin;         // Minute from GPS receiver
int GPSSec;         // Second from GPS receiver
int GPSHun;         // Hundredths of second from GPS receiver
int GPSDay;         // Day from GPS receiver
int GPSMon;         // Month from GPS receiver
int GPSYrs;         // Year from GPS receiver
int GPSCen;         // what century it is in GPS land

// read the GPS time
static void readGPStime(TinyGPS &gps)
{
  byte mon, days, hr, mins, sec, hund;
  int yrs;
  
  gps.crack_datetime(&yrs, &mon, &days, &hr, &mins, &sec, &hund, &GPSage);
  
  GPSHun  = hund; 
  GPSSec  = sec; 
  GPSMin  = mins;
  GPSHrs  = hr;
  GPSDay  = days;
  GPSMon  = mon;
  GPSYrs  = yrs % 100;   // remove century
  GPSCen  = yrs/100;
}

// ----------------------- Process non-zero time zone oddness --------------------

// Number of days in a month
//                      JanFebMarAprMayJunJulAugSepOctNovDec
const int NDays[]  = {0,31,28,31,30,31,30,31,31,30,31,30,31,  // normal year
                        31,29,31,30,31,30,31,31,30,31,30,31}; // leap year


// Move forwards in calendar if needed due to GPS -> local conversion
// Figures out month lengths, century, etc.
void IncDays(void) 
{
  int lastDay;
  if (Years % 4) lastDay = NDays[Mons + 12];  // a leap year
  else lastDay = NDays[Mons];                  // not a leap year

  Days += 1;
  if (Days > lastDay) 
  {
    Days = 1;                // begin the next month
    Mons += 1;
    if (Mons > 12) 
    {
      Mons = 1;
      Years += 1;
      if (Years > 99)      // begin the next century
      {
        Years = 0;
        Century += 1;
      }
    }
  }
}

// Move backwards in calendar if needed due to GPS -> local conversion
// On month borrow, we go to the last day in the previous month
// Figures out month lengths, century, etc.
void DecDays(void) 
{
  Days -= 1;
  if (Days == 0) 
  {
    Mons -= 1;   // goto last day of prev month
    if (Mons == 0) 
    {
      Mons = 12;
      Years -= 1;
      if (Years < 0) 
      {
        Years = 99;
        Century -= 1;
      }
    }
    // Set to the last day of this month, after the month gets reset if needed
    if (Years % 4) Days = NDays[Mons + 12];  // a leap year
    else Days = NDays[Mons];                  // not a leap year
  }
}

// read the GPS clock if it's there and valid, otherwise use the RTC time
void getTheTime(void)
{
  // Read time from the GPS receiver if it's there
  readGPStime(myGps);   // get current time into time variables

  // See if the GPS time is good, use either it or the RTC accordingly
  if ((GPSage == TinyGPS::GPS_INVALID_AGE) || (GPSage > 1500))     // not conencted or stale time
  {
    readRTCtime();   // get current time into time variables
  }
  else
  {
    Secs  = GPSSec;         // Second from GPS receiver
    Days  = GPSDay;         // Day from GPS receiver
    Mons  = GPSMon;         // Month from GPS receiver
    Years = GPSYrs;         // Year from GPS receiver
    Century = GPSCen;

    // deal with the fact that GPS is UTC, but we display local time
    Hrs   = GPSHrs + Zone + DST;         // Hour from GPS receiver, shifted to local time
    Mins  = GPSMin + (Zone > 0 ? ZMins : -ZMins);         // Minute from GPS receiver
    
    // The minutes and hours may be out of range. Correct them if so
    if (Mins > 59)
    {
      Hrs++;
    }
    if (Mins < 0)
    {
      Hrs--;
    }
    if (Hrs > 23) 
    {
      Hrs -= 24;
      IncDays();   // do some math on the calendar
    }
    if (Hrs < 0)
    {
      Hrs += 24;
      DecDays();
    }
    // After the dust has settled, see what day it is
    WDay  = GetWDay(Days, Mons, Years, Century);

    // write time back into the RTC so that it's fresh
    if (frame%1000 == 0) writeRTCtime();
  }
//  if (frame%50 == 0) Serial.printf("%02d:%02d:%02d %5d\n", Hrs, Mins, Secs, GPSage);
}

// ---------------------- Rotary encoder -------------------

// We save a copy of the previous reading in bits 1:0 of LastEnc.
// Then we read the current encoder to bits 3:2 of LastEnc and look up the
// direction in the nifty table below.

// The result is in EncDir - 0 for no motion, 1+ for right, -1- for left.
// The code that uses the encoder motion has to clear EncDir!

// Encoder quadrature lookup table
int incr = 1;  // encoder turned clockwise
int decr = -1;  // encoder turned counter-clockwise
int none = 0; // encoder didn't move
int impos = 0;  // not possible - glitch detected

// PEC11R encoder: Two detents per cycle, only change on bit 0 moving
//         old:   00  01   10    11       new
int EncTab[] = {none,decr,none,impos,  // 00
                incr,none,impos,none,  // 01
                none,impos,none,incr,  // 10
                impos,none,decr,none}; // 11

// This works best if called once per MainLp loop

// Initialize the encoder history to match its position
void InitEnc()
{
  LastEnc = digitalRead(encBPin) << 1 | digitalRead(encAPin); 
  LastEnc = LastEnc * 5;  // copy of old and new next to each other
}

// Read encoder, update history, look for motion, update into EncDir
// This must be called exactly once per MainLp loop
// Returns # of detents in EncDir, -=CCW, +=CW
void DoEnc() {
  LastEnc = LastEnc >> 2 | digitalRead(encBPin) << 3 | digitalRead(encAPin) << 2; 
  EncDir = EncDir + EncTab[LastEnc];   // bump encoder value by motion via lookup table
}

// DoButt detects if the encoder button is pushed, and debounces.
// If it's time for action, it returns Button bit set in Flags
// This must be called exactly once per MainLp loop
void DoButt() {
  if (digitalRead(encButPin) == 0) {  // zero is pressed
    ButHist++;
    pushed = (ButHist == 3);
  }
  else {
    ButHist = 0;
    pushed = false;
  }
}
