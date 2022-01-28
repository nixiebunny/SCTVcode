// --------- Clock code ---------------------------

#include <array>

constexpr std::array<const char*, 12> monthStrings {
    "Jan",  "Feb", "March", "April", "May", "June",
    "July", "Aug", "Sept",  "Oct",   "Nov", "Dec",
};

// Weekday strings
const char sunStr[] = "Sunday\n";
const char monStr[] = "Monday\n";
const char tueStr[] = "Tuesday\n";
const char wedStr[] = "Wednesday\n";
const char thuStr[] = "Thursday\n";
const char friStr[] = "Friday\n";
const char satStr[] = "Saturday\n";

const char* const SunStr[] = {sunStr, monStr, tueStr, wedStr, thuStr, friStr, satStr};

// Other time related strings
char WDayStr[16];    // weekday string with NL
char MonthStr[12];   // month abbreviation with built-in NL
char HrSelStr[8];    // 12 or 24 for time format

// Time zone strings allow selection of +/-hh:mm in 15 min increments
char ZoneStr[] = "-07";
char ZMinStr[] = "00";
char DSTStr[] = "Off\n";
char HzStr[] = "00";

// Time strings - These are updated by makeTimeStrings()
char CenStr[] = "00";
char YrsStr[] = "00";
char MonStr[] = "00";
char DayStr[] = "00\n";
char HrsStr[] = "00";
char MinStr[] = "00";
char SecStr[] = "00\n";

// Some fixed strings
char BlankLn[] = "\n";
char ColStr[]  = ":";
char DashStr[] = "-";
char SlaStr[]  = "/";
char SpaStr[]  = " ";
char DoneStr[] = "Exit menu\n"; // done with this menu
char OnStr[]   = "On \n";  // for on/off options like DST
char OffStr[]  = "Off\n";  // both are 5 bytes for easy copying

// The splash screen draw list
const char scopeClock[] = "The Scope Clock\n";
const char fromStr[]    = "from\n";
const char cathCorn[]   = "Cathode Corner\n";
const char copyR[] = "(C)2021 David Forbes\n";
const char customized[]   = "customized by Nikolas Klauser\n";
constexpr struct item splashList[] = {
  {ItemType::text,12,0,scopeClock,0,0},
  {ItemType::text,8,0,fromStr,   0,0},
  {ItemType::text,12,0,cathCorn,  0,0},
  {ItemType::text,10,0,BlankLn,   0,0},
  {ItemType::text,6,0,versionNo, 0,0},
  {ItemType::text,6,0,copyR, 0,0},
  {ItemType::text,6,0,customized, 0,0},
  {ItemType::listend,0,0,BlankLn,0,0}
};

// ----------------------------- Analog clock drawing -------------------

// The Face draw list has circles for minor marks and Roman
// numerals for the compass points. A circle in the center too.
// The hands are drawn by DrawClk
// The positions of the numerals are trial-and-error.

constexpr std::array<const char *, 12> romanNums = {
    "I", "II", "III", "IIII", "V", "VI", "VII", "VIII", "IX", "X", "XI", "XII",
};

constexpr struct item faceList[] = {
  {ItemType::text,10,0,romanNums[0],      490,  760},    //  1 o'clock
  {ItemType::text,10,0,romanNums[1],     820,  400},    //  2 o'clock
  {ItemType::text,10,0,romanNums[2],    900, -100},    //  3 o'clock
  {ItemType::text,10,0,romanNums[3],   740, -590},    //  4 o'clock
  {ItemType::text,10,0,romanNums[4],      400, -960},    //  5 o'clock
  {ItemType::text,10,0,romanNums[5],    -100,-1080},    //  6 o'clock
  {ItemType::text,10,0,romanNums[6],   -600, -960},    //  7 o'clock
  {ItemType::text,10,0,romanNums[7], -1000, -600},    //  7 o'clock
  {ItemType::text,10,0,romanNums[8],   -1040, -100},    //  9 o'clock
  {ItemType::text,10,0,romanNums[9],     -940,  400},    // 10 o'clock
  {ItemType::text,10,0,romanNums[10],    -600,  760},    // 11 o'clock
  {ItemType::text,10,0,romanNums[11],   -160,  880},    // 12 o'clock
  {ItemType::listend,0,0,BlankLn,0,0}
};

// Draw clock hand on screen

// angle is the angle (0=N, 60=E, 120=S, 180=W)
// len is length of hand
void DoHand(int len, int angle) {
  int handAngle = (angle*nsteps/240) % nsteps;  // get angle in range of new sin/cos tab
  YSize = costab[handAngle]/500;      // swap X and Y, because 0 deg is at north CW like a clock, not east CCW like math
  XSize = sintab[handAngle]/500;
  XStart = XSize * 10 / 13; // start at center circle radius
  YStart = YSize * 10 / 13;
  XEnd = (len * XSize) >>8;   // scale to hand length
  YEnd = (len * YSize) >>8;
 // Serial.printf("size %5d %5d  start %5d %5d  end %5d %5d\n", XSize, YSize, XStart, YStart, XEnd, YEnd);
  Scale = 1;
  ChrXPos = ChrYPos = 0;
  Shape = lin;
  DoSeg();
}

// DrawClk draws the three clock hands. They are drawn in fine
// increments of 1.5 degrees per step. The steps are calculated
// incorporating the smaller time units for smoother movement.
// The angle is (0=N, 60=E, 120=S, 180=W)
void DrawClk() {
  drawACircle(0, 0, 180);   //
//  DoHand(250, (Ticks / HalfSec) << 1 + (Secs << 2));  // no Ticks right now, so no smooth sweep.
  // doingHand = true;
  DoHand(2500, Secs << 2);
  DoHand(2000, (Secs / 15) + (Mins << 2));
  DoHand(1500, (Hrs % 12) * 20 + Mins / 3);
  DoHand(2000, (Secs / 15) + (Mins << 2));   // make the hour and minute hands bright by doubling up
  DoHand(1500, (Hrs % 12) * 20 + Mins / 3);
  // doingHand = false;
}

// ------------------------ digital clocks -----------------------------------
/*
// total time/date/day digital clock draw list
struct item timefList[]  = {
  {text,20,0,WDayStr, 0,0},   // day of week
  {text,20,0,HrsStr,  0,0},   // hours
  {text,20,0,ColStr,  0,0},   // colon
  {text,20,0,MinStr,  0,0},   // mins
  {text,20,0,ColStr,  0,0},   // colon
  {text,20,0,SecStr,  0,0},   // secs
  {text,20,0,MonthStr,0,0},   // month
  {text,20,0,SpaStr,  0,0},
  {text,20,0,DayStr,  0,0},   // day
  {text,20,0,CenStr,  0,0},   // the full year
  {text,20,0,YrsStr,  0,0},
  {text,20,0,BlankLn, 0,0},
  {listend,0,0,BlankLn, 0,0}
};
*/
// 6 digit digital clock with date draw list
constexpr struct item time6dList[] = {
  {ItemType::text,16,0,MonthStr,0,0},  // months
  {ItemType::text,16,0,SpaStr,  0,0},  // space
  {ItemType::text,16,0,DayStr,  0,0},  // days
  {ItemType::text,40,0,HrsStr,  0,0},  // hours
  {ItemType::text,40,0,ColStr,  0,0},  // colon
  {ItemType::text,40,0,MinStr,  0,0},  // mins
  {ItemType::text,40,0,BlankLn, 0,0},  // next line
  {ItemType::text,30,0,SecStr,  0,0},  // secs
  {ItemType::listend,0,0,BlankLn, 0,0}
};


// 4 digit digital clock with date draw list
constexpr struct item time4dList[] = {
  {ItemType::text,16,0,WDayStr, 0,0},  // weekday
  {ItemType::text,40,0,HrsStr,  0,0},  // hours
  {ItemType::text,40,0,ColStr,  0,0},  // colon
  {ItemType::text,40,0,MinStr,  0,0},  // mins
  {ItemType::text,40,0,BlankLn, 0,0},  // next line
  {ItemType::text,16,0,MonthStr,0,0},  // months
  {ItemType::text,16,0,SpaStr,  0,0},  // space
  {ItemType::text,16,0,DayStr,  0,0},  // days
  {ItemType::listend,0,0,BlankLn,0,0}
};


// 6 digit digital clock draw list
constexpr struct item time6nList[] = {
  {ItemType::text,10,0,BlankLn, 0,0},  // make hh:mm line lower for better centered appearance
  {ItemType::text,40,0,HrsStr,  0,0},  // hours
  {ItemType::text,40,0,ColStr,  0,0},  // colon
  {ItemType::text,40,0,MinStr,  0,0},  // mins
  {ItemType::text,40,0,BlankLn, 0,0},  // next line
  {ItemType::text,30,0,SecStr,  0,0},  // secs
  {ItemType::listend,0,0,BlankLn,0,0}
};


// 4 digit digital clock draw list
constexpr struct item time4nList[] = {
  {ItemType::text,40,0,HrsStr,  0,0},  // hours
  {ItemType::text,40,0,ColStr,  0,0},  // colon
  {ItemType::text,40,0,MinStr,  0,0},  // mins
  {ItemType::text,40,0,BlankLn, 0,0}, // next line
  {ItemType::listend,0,0,BlankLn,0,0}
};

// ----------------------- Time string generator -----------------------

// makeTimeStrings fills in the time and date strings in RAM with the
// current time in ASCII. The month and weekday strings are set also.
void makeTimeStrings() {
  int hours, zon;  // temp storage for manipulation
  if (Hr12)
  {
    HrSelStr[0] = '1';
    HrSelStr[1] = '2';
  }
  else
  {
    HrSelStr[0] = '2';
    HrSelStr[1] = '4';
  }
  CenStr[0] = (Century / 10) | '0';
  CenStr[1] = (Century % 10) | '0';
  YrsStr[0] = (Years / 10) | '0';
  YrsStr[1] = (Years % 10) | '0';
  MonStr[0] = (Mons / 10) | '0';
  MonStr[1] = (Mons % 10) | '0';

  DayStr[0] = (Days / 10) | '0';
  DayStr[1] = (Days % 10) | '0';
  DayStr[2] = '\n';

  // make the day of month be one digit if leading zero
  if (DayStr[0] == '0')
  {
    DayStr[0] = DayStr[1];
    DayStr[1] = '\n';
    DayStr[2] = 0;
  }

  // read the weekday string from ROM
  strcpy(WDayStr, SunStr[WDay]);

  // read the month string from ROM
  strcpy(MonthStr, monthStrings[Mons - 1]);

  hours = Hrs;  // temp for doing 12 hour madness
  if (Hr12)
  {
    if (hours == 0) hours = 12;
    if (hours > 12) hours -= 12;
  }
  HrsStr[0] = (hours / 10) | '0';
  HrsStr[1] = (hours % 10) | '0';
  if ((Hr12) && (HrsStr[0] == '0'))
  {
    HrsStr[0] = HrsStr[1];
    HrsStr[1] = 0;  // one digit if < 10
  }
  MinStr[0] = (Mins / 10) | '0';
  MinStr[1] = (Mins % 10) | '0';
  SecStr[0] = (Secs / 10) | '0';
  SecStr[1] = (Secs % 10) | '0';
  HzStr[0] = (Hertz / 10) | '0';
  if (DST)
  {
    DSTStr[1] = 'n';   // DST on
    DSTStr[2] = ' ';
  }
  else
  {
    DSTStr[1] = 'f';
    DSTStr[2] = 'f';
  }
  zon = Zone;   // temp copy of timezone to monge
  if (zon >= 0) ZoneStr[0] = '+';
  else {
    ZoneStr[0] = '-';
    zon = 0 - zon;      // make it positive for display
  }
  ZoneStr[1] = (zon / 10) | '0';
  ZoneStr[2] = (zon % 10) | '0';
  if (ZoneStr[0] == '0')
  {
    ZoneStr[0] = ZoneStr[1];
    ZoneStr[1] = '\n';  // one digit if < 10
  }
  ZMinStr[0] = (ZMins / 10) | '0';
  ZMinStr[1] = (ZMins % 10) | '0';
}
