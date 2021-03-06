#pragma config(Sensor, S1,     LINELEADER,     sensorI2CCustom9V)
#pragma config(Sensor, S2,     BUMPER,         sensorTouch)
#pragma config(Sensor, S4,     COMPASS,        sensorI2CCustom9V)
#pragma config(Motor,  motorB,          LEFT,          tmotorNormal, openLoop, encoder)
#pragma config(Motor,  motorC,          RIGHT,         tmotorNormal, openLoop, encoder)
//*!!Code automatically generated by 'ROBOTC' configuration wizard               !!*//

// Xander Soldaat's driver suite
#include "drivers/MSLL-driver.h"
#include "drivers/HTMC-driver.h"
#include "drivers/LEGOTS-driver.h"

string sFileName = "pie.dat";

TFileIOResult nIoResult;
TFileHandle hFileHandle;
int nFileSize;

int basePower = 57;
float Min = -100, Max = 100;
// 56/32 1/256 50 6/16@9:44p on low gears
// 18/32 0 55 6/17@1:30a on high gears @ 50 base
// 18/32 0 45 @2:00a @57 base
// the green and yellow patches are throwing it off, show up as noise and spikes derivative
float Kp = 18.0 / 32.0,
Ki = 0.0 / 4096.0,
Kd = 45.0;
int previousError;
float integral = 0.0, derivative = 0.0;

void T(string t)
{
  nxtScrollText(t);
  writeDebugStreamLine(t);
}

void PressBumperToContinue()
{
  nxtScrollText("");
  nxtScrollText("press button");
  nxtScrollText("to continue");
  PlaySoundFile("! Attention.rso");
  while (!TSreadState(BUMPER))
    wait1Msec(10); // not pressed
  while (TSreadState(BUMPER))
    wait1Msec(10); // pressed
  wait1Msec(500);
}

void Calibrate()
{
  T("enter Calibrate");
  LLinit(LINELEADER);
  nxtScrollText("Place over white");
  PressBumperToContinue();
  wait1Msec(500);
  LLcalWhite(LINELEADER);

  nxtScrollText("");
  nxtScrollText("Place over line");
  nxtScrollText("(Black)");
  PressBumperToContinue();
  wait1Msec(500);
  LLcalBlack(LINELEADER);

  // compass calibration
  nxtScrollText("");
  nxtScrollText("compass calibrate");
  PressBumperToContinue();
  wait1Msec(500);
  motor[LEFT] = 30;
  motor[RIGHT] = -30;
  wait1Msec(500);
  HTMCstartCal(COMPASS);
  wait1Msec(5000);
  HTMCstopCal(COMPASS);
  motor[LEFT] = 0;
  motor[RIGHT] = 0;

  PlaySound(soundFastUpwardTones);
  T("leave Calibrate");
  wait1Msec(2000);
}

int DoPID(int pv, int sp, float kp, float ki, float kd, int dt)
{
  int error = sp - pv;
  integral = integral + (error * dt);
  if (dt > 0)
    derivative = (float)(error - previousError) / (float)dt;
  derivative = clip(derivative, -.6, .6);
  float output = (kp * error) + (ki * integral) + (kd * derivative);
  previousError = error;
  return (int)output;
}

void FollowSegment()
{
  int steering;

  T("enter FollowSegment");

  ClearTimer(T1);
  long lastTime = time1[T1];

  while (true) {
    int timeNow = time1[T1];
    int elapsedTime = timeNow - lastTime;
    lastTime = timeNow;

    int llAvg = LLreadAverage(LINELEADER);
    unsigned byte llResult = LLreadResult(LINELEADER);

    if (llAvg != 0)
    {
      steering = DoPID(llAvg, 45, Kp, Ki, Kd, elapsedTime); // 45 is our goal, set point
      //steering = llAvg;
      motor[LEFT] = clip((basePower + steering), Min, Max);
      motor[RIGHT] = clip((basePower - steering), Min, Max);

      // data logging
      int heading = HTMCreadHeading(COMPASS);
      WriteShort(hFileHandle, nIoResult, timeNow);
      WriteShort(hFileHandle, nIoResult, llAvg);
      WriteShort(hFileHandle, nIoResult, steering);
      WriteFloat(hFileHandle, nIoResult, integral);
      WriteFloat(hFileHandle, nIoResult, derivative);
      WriteShort(hFileHandle, nIoResult, elapsedTime);
      WriteShort(hFileHandle, nIoResult, heading);
      WriteByte(hFileHandle, nIoResult, llResult);
    }
    else
    {
      // stuff gone wrong, we lost the line
      // this may be where we check result for an intersection
      PlaySound(soundException);
    }

    if (TSreadState(BUMPER))
      break;

    lastTime = timeNow; // using the variable, not the actual clock
    //wait1Msec(10);    // some throttling
  }

  T("leave FollowSegment");
}

task main()
{
  eraseDisplay();
  T("pie2");

  //Calibrate();

  Delete(sFileName, nIoResult);
  nFileSize = 21 * 10000;  // room for 10,000 entries
  OpenWrite(  hFileHandle, nIoResult, sFileName, nFileSize);
  if (nIoResult != ioRsltSuccess)
  {
    T("Open failed");
    writeDebugStream("nAvailFlash: %d", nAvailFlash);
    StopAllTasks();
  }

  PressBumperToContinue();

  FollowSegment();

  motor[LEFT] = 0;
  motor[RIGHT] = 0;

  LLsleep(LINELEADER);
  Close(hFileHandle, nIoResult);
  PlaySound(soundFastUpwardTones);
  wait1Msec(2000);
}
