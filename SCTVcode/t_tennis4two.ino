// ----------------------------------- Tennis for Two Game -----------------------------------

// T42 variables
const int COURTSIZE 	= 2000;
const int BALLSIZE	= 20;
const int NETHEIGHT	= 250;
const int COURTLENGTH	= 3000;
const int HBOXLEFT	= -1200;
const int HBOXRIGHT	= 1200;
const int HBOXTOP = 1200;
const int HBOXBOT = -1200;
const int BOUNDS = 1000;
const int RESTART	= 50;
const int CENTERPOINT = 0;
const int COURT = -300;
const float COR = 0.75; // realistic tennis bouce coefficient
const float NETCOR = 0.5;
const int NETBOTTOM = -300;
const int NETTOP = -50;
const int BDELAYFRAMES = 50;
const float GRAVITY = 5.5;
const int DRAG = 2.0;

bool twaitingForBall = true;
int tWaitFrames = BDELAYFRAMES;

float tXBall = 1200;
float tYBall = 350;
float tXVel = -15;
float tYVel = -9.7;
int curSec = Secs;
int prevSec = Secs;
int counter = 0;
int modifier = 0;
int retardent = 0;

struct item t42List[] = {};

void T42() 
{
	// read the position controls for paddles, and average for smooth readings
	int leftPaddle = 0;
	int rightPaddle = 0;
	for (int i = 0; i < 40; i++) {
		leftPaddle += analogRead(XPosPin) - 512;	// read position controls
		rightPaddle += analogRead(YPosPin) - 512;	// make bipolar so midpoint is nominal
	}
  // Center in the display area, scale for correct overshoot
  leftPaddle = leftPaddle / 700;
  rightPaddle = rightPaddle / 700;

  // draw the court lines
  drawALine( CENTERPOINT, CENTERPOINT-300, CENTERPOINT, CENTERPOINT+NETHEIGHT-300); // draw the net
  drawALine( HBOXLEFT, COURT, HBOXRIGHT, COURT); // draw the court

  // Check if the ball hits the court floor
  if ((tYBall - BALLSIZE / 2) >= COURT - 20 && (tYBall - BALLSIZE / 2) <= COURT + 20 && tYVel - GRAVITY * modifier <= 0) 
  {
      tYVel = -tYVel*COR;
      modifier = 0; // reset gravity
      curSec = Secs; // reset dt
  }


  // Check if the ball hits the net from the right side
  if ((tXBall - BALLSIZE / 2 ) < 50 && (tXBall - BALLSIZE / 2) > -20 && (tYBall - BALLSIZE / 2) < NETTOP && tXVel < 0) tXVel = -NETCOR * tXVel;

  // Check if the ball hits the net from the left side
  if ((tXBall + BALLSIZE / 2) < 20 && (tXBall + BALLSIZE / 2) > -50 && (tYBall - BALLSIZE / 2) < NETTOP && tXVel > 0) tXVel = -NETCOR * tXVel;

  // Check if the ball has fallen off the court
  if ((tXBall < HBOXLEFT - 1000) || (tXBall > HBOXRIGHT + 1000) || (tYBall > HBOXTOP + 1000) || (tYBall < HBOXBOT - 1000) )
  {
     tXBall = 1200;
     tYBall = 350;
     twaitingForBall = true;
     tWaitFrames = BDELAYFRAMES;
  }

  
  if (pushed) 
  {
     if ((tXBall - BALLSIZE / 2) < 0)
     {
        pushed = false;
        tXVel = 20;
        tYVel = leftPaddle;
        modifier = 0; // reset gravity
        curSec = Secs; // reset dt
     } else {
        pushed = false;
        tXVel = -20;
        tYVel = rightPaddle;
        modifier = 0; // reset gravity
        curSec = Secs; // reset dt
     }
  }

  
  if (twaitingForBall)
  {
     if (tWaitFrames > 0)
     {
      tWaitFrames--;
     } else {
      twaitingForBall = false;
      tYVel = -9.7;
      tXVel = -10;
      modifier = 0; // reset gravity
      curSec = Secs; // reset dt
     }
  } else {
    prevSec = Secs;
    if (curSec != prevSec) retardent++;
    if (retardent == 7)
    {
       modifier++;
       retardent = 0;
    }

    // hold until ball is ready to release
    // Draw the ball now
    tYBall += int(tYVel-GRAVITY*modifier);
    
    tXBall += int(tXVel);
    drawACircle (int(tXBall), int(tYBall), BALLSIZE);
  }

}
		
