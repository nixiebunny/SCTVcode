// ------------------------------------ Pong game -----------------------------------

// Pong variables
const int courtSize    = 2000; 
const int ballSize     = 80;
const int paddleLength = 200;
const int leftWall     = -courtSize/2;
const int rightWall    =  courtSize/2;
const int topWall      =  courtSize/2;
const int botWall      = -courtSize/2;
const int midWall      = 0;
const int htWall       = courtSize;
const int centerLine   = 0;
const int scoreHeight  = 100;   // score position floating above top wall
const int bounds       = 1000;   //  how far past paddle to let ball fly if missed, delays next ball
const int ballStartDelayFrames = 50;   // how long to wait efore releasing a new ball

bool waitingForBall = true;
int waitFrames = ballStartDelayFrames;

int xBall  = 0;
int yBall  = 0;
int xVel   = 30;
int yVel   = 20;
int lScore = 0;
int rScore = 0;

// Pong draw list just does the score. Court is drawn by doPong().
char lScoreStr[] = "00";
char rScoreStr[] = "00";
struct item pongList[] = {
  {text,10,0,lScoreStr, leftWall/2-100, topWall+scoreHeight},
  {text,10,0,rScoreStr,rightWall/2-100, topWall+scoreHeight},
  {listend,0,0,BlankLn,0,0}
};


// This plays Pong. It uses the position controls as paddles. 
void doPong() {

  // read the position controls for paddles, and average many for smooth readings
  int leftPaddle = 0;
  int rightPaddle = 0;
  for (int i=0; i<40; i++) {
    leftPaddle  += analogRead(XPosPin) - 512;   // read the position controls
    rightPaddle += analogRead(YPosPin) - 512;   // make them bipolar so midpoint is nominal
  }
  // center then in the display area, and scale for the right amount of overshoot
  leftPaddle  = leftPaddle  / 6;
  rightPaddle = rightPaddle / 6;

  // draw the court lines
  drawALine( centerLine, topWall, centerLine, botWall);  // the net
  drawALine( leftWall,   topWall,  rightWall, topWall);  // top bounds
  drawALine( leftWall,   botWall,  rightWall, botWall);  // bottom bounds

  // draw the paddles
  drawALine( leftWall, leftPaddle  - paddleLength,  leftWall, leftPaddle  + paddleLength);
  drawALine(rightWall, rightPaddle - paddleLength, rightWall, rightPaddle + paddleLength);
  
  
  // see if the ball hit anything
  // top or bottom is a simple bounce
  if ((yBall - ballSize/2) < botWall && yVel <= 0) yVel = -yVel;
  if ((yBall + ballSize/2) > topWall && yVel >= 0) yVel = -yVel;
  
  // hit the left player's paddle, so rebound
  if (((xBall - ballSize/2) < leftWall + 20) && ((xBall - ballSize/2) > leftWall - 50) && 
       (xVel < 0) && 
        yBall < (leftPaddle + paddleLength) && yBall > (leftPaddle - paddleLength) ) 
  {
    xVel = -xVel;
    // some English
    if (yBall > leftPaddle + paddleLength/2) yVel += 20;
    if (yBall < leftPaddle - paddleLength/2) yVel -= 20;
  }
  // hit the right player's paddle, so rebound
  if (((xBall + ballSize/2) > rightWall - 20) && ((xBall + ballSize/2) < rightWall + 50) && 
       (xVel > 0) && 
        yBall < (rightPaddle + paddleLength) && yBall > (rightPaddle - paddleLength) ) 
  {
    xVel = -xVel;
    // some English
    if (yBall > rightPaddle + paddleLength/2) yVel += 20;
    if (yBall < rightPaddle - paddleLength/2) yVel -= 20;
  }
  
  // left lpayer missed, ball out of bounds, serve ball again and do some scoring work
  if (xBall < leftWall - bounds)  
  {
    if (rScore >= 10)    // score stops at 11 points in Pong
    {
      rScore = 11;  
      waitingForBall = true;
      waitFrames = 100000;   // stop the game by making it wait forever, the user will eventually notice
    }
    else
    {
      rScore++;  
      xBall = centerLine;
      waitingForBall = true;
      waitFrames = ballStartDelayFrames;
    }
  }
  
  // right player missed, bump left score
  if (xBall > rightWall + bounds) 
  {
    if (lScore >= 10)    // score stops at 11 points in Pong
    {
      lScore = 11;  
      waitingForBall = true;
      waitFrames = 100000;   // stop the game by making it wait forever, the user will eventually notice
    }
    else
    {
      lScore++;  
      xBall = centerLine;
      waitingForBall = true;
      waitFrames = ballStartDelayFrames;
    }
  }
  // don't do any ball display until ball is ready to release. 
  if (waitingForBall)
  {
    if (waitFrames > 0)
    {
      waitFrames--;
    }
    else
    {
      waitingForBall = false;
      xBall = centerLine;
      if (yVel >  20) yVel =  20;          // make it have a tame trajectory
      if (yVel < -20) yVel = -20;
    }
  }
  else
  {
    // move the ball
    xBall += xVel;
    yBall += yVel;
    drawACircle(xBall, yBall, ballSize);
  }

  // make the scoring strings reflect the score
  lScoreStr[0] = (lScore / 10) | '0';
  lScoreStr[1] = (lScore % 10) | '0';
  rScoreStr[0] = (rScore / 10) | '0';
  rScoreStr[1] = (rScore % 10) | '0';
  
}
