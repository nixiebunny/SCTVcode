
// ----------------------------- Main startup code ------------------------------
void setup() 
{
  analogWriteResolution(12);    // Use the real DACs for X and Y position
  
  // Circle lookup tables
  for (i=0;i<nsteps;i++) {
    costab[i] = int(65536.*cos(float(i*2)*pi/float(nsteps)));
    sintab[i] = int(65536.*sin(float(i*2)*pi/float(nsteps)));
  }
// Set Arduino pins to proper directions
  pinMode(encButPin, INPUT_PULLUP);   // encoder pushbutton
  pinMode(encAPin,   INPUT_PULLUP);   // encoder quadrature signals
  pinMode(encBPin,   INPUT_PULLUP);
  pinMode(BlankPin, OUTPUT);   // high blanks the display

  InitEnc();
  rn1 = rn2 = rn3 = rn4 = 0x45;   // random number generator seed? Why not use random()?
  Wire.begin();          // RTC uses I2C
  Serial.begin(115200);    // debug port
  delay(100);
  EncDir = 0;    // no buttons being pushed or knobs being turned, we hope
  pushed = false;
  theClock = NClks;    // draw a splash screen until knob turned
  InMenu = false;
//  FlwStr[4] = '\n';
//  MakeFLW();
  doHaiku();                 // get a fresh one

  // initialize scores etc.
  reset_tetris();
  // and start dropping the first tetromino
  get_next_tetromino();
  tetr_type  = next_tetr_type;
  get_next_tetromino();
  drop_tetromino();

  myusb.begin();     // start the USB device service
  readRTClocale();   // get the locale data if it was stored
}


// --------------------------- Main loop --------------------------


/* don't run the real one for now, testing things first.
// test the string writing code. 

char TestString[] = "Hello world";

void loop() {
  Scale = 2;
  SetScale();
  ChrXPos = 00;
  ChrYPos = 128;
  XSaver = 0;
  YSaver = 0;
  StrPtr = TestString;
  DispStr();
 // Serial.println("This is one strings worth.");
//  delay(30);   // once per frame?
}
// end of test code */ 


/* don't run the real one for now, testing things first.
// test the list drawing code. 

void loop() {
  XSaver = 0;
  YSaver = 0;
  TheList = ALPHAList; 
//  TheList = alphaList; 
//  TheList = symbolList; 
//  TheList = time6nList; 
  Center(TheList);
  DoAList(TheList);
 // delay(200);   // don't write for a long time, to debug hardware circles.
}
// end of test code */ 

 // don't do the real thing while testing 

// real code. This is not a test
void loop() 
{
  // process USB connection and disconnection events
  myusb.Task();
  // Print out information about different devices.
  for (uint8_t i = 0; i < CNT_DEVICES; i++) 
  {
    if (*drivers[i] != driver_active[i]) 
    {
      if (driver_active[i]) 
      {
        Serial.printf("*** Device %s - disconnected ***\n", driver_names[i]);
        driver_active[i] = false;
      } 
      else 
      {
        Serial.printf("*** Device %d %s %04X:%04X - connected ***\n", i, driver_names[i], drivers[i]->idVendor(), drivers[i]->idProduct());
        driver_active[i] = true;

        const uint8_t *psz = drivers[i]->manufacturer();
        if (psz && *psz) Serial.printf("  manufacturer: %s\n", psz);
        psz = drivers[i]->product();
        if (psz && *psz) Serial.printf("  product: %s\n", psz);
        psz = drivers[i]->serialNumber();
        if (psz && *psz) Serial.printf("  Serial: %s\n", psz);

        // If this is a new Serial device, start it up at NMEA buad rate
        Serial.printf("Connect before begin\n");
        if (drivers[i] == &userial) 
        {
          userial.begin(usbBaudRate);
        }
        Serial.printf("Exiting connect code\n");
      }
    }
  }
  // Read the USB serial port if anything's there
  while (userial.available())
    myGps.encode(userial.read());

  if ((theClock != 1) && (theClock != 2))  // Pong and Tetris use position controls as paddles
  {
    xPos = yPos = 0;
    for (i=0;i<40;i++) {
      xPos += analogRead(XPosPin) - 512;   // read the position controls
      yPos += analogRead(YPosPin) - 512;   // make them bipolar so midpoint is nominal
    }
    xPos = xPos / 10;
    yPos = yPos / 10;
  }

  getTheTime();   // read whichever clock is correctest, make it be local time
  
  blinkCount++;
  Blink = (blinkCount >> BlnkBit) & 1;  // a one bit, 5 times a second thingie
  
  DoEnc();
  DoButt();
  if (InMenu) 
  {
    DoMenus();      // if menu, process it
    pushed = false;
    EncDir = 0;
  }
  else 
  {  // it's a clock, not a menu
    if (EncDir != 0) 
    {   // If knob turned, choose the next clock face
      theClock += EncDir;
      if (theClock >= NClks) theClock = 0;   // select the next clock face
      if (theClock < 0) theClock = NClks - 1;
      if (theClock == 1) 
      {
        lScore = 0;                 // reset the score when entering Pong
        rScore = 0;
        waitingForBall = true;                 // let the user have a chance to get ready
        waitFrames = ballStartDelayFrames;
        xBall = centerLine;                   // give them a chance
      }
      if (theClock == 2)
      {
        reset_tetris();
      }
      EncDir = 0;
    }
    whichList = ClkList[theClock];       // point to the clock drawlist we are displaying now
    if (theClock == 0) DrawClk();        // clock 0 has hands to draw
    if (theClock == 1) doPong();         // clock 1 is Pong
    if (theClock == 2) drawTetris();     // clock 2 is Tetris
    if (pushed) 
    {
      whichList = mainMenu;
      HotItem = 1;
      InMenu = true;
    }
  }

  do_tetris_keys();

  if ( !game_over && millis() > next_tick ) {
    next_tick += tick_length;

    move_tetromino_down();
  }

  if (frame%haikuFrames == 0) doHaiku();                 // get a fresh one

//  unsigned int beforeTime = micros();
  copyList(whichList);            // make fresh copy of draw list for us to muck with
  makeTimeStrings();              // fill in the time variables into number strings
  updateScreenSaver();
//  unsigned int stringsTime = micros();
  Center(TheList);                // fill in the positions of each string in our copy
//  unsigned int centerTime = micros();
  DoAList(TheList);               // draw it on the screen
//  unsigned int drawTime = micros();
  while (micros() - lastMicros < (1000000/Hertz)) {
    delayMicroseconds(10);
  }
  lastMicros = micros();   // for next time

 // if (frame%20 == 0) Serial.printf("strings %4d   center %4d   draw %6d us\n", 
 //                  stringsTime-beforeTime, centerTime-stringsTime, drawTime-centerTime);
  frame++;   // turn off diagnostic printing
}

 // end of real code */
