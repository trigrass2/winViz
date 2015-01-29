#pragma config(Sensor, S1,     Bumper1,	sensorTouch)
#pragma config(Sensor, S2,     MSIMU,   sensorI2CCustomFastSkipStates)
#pragma config(Sensor, S4,     XBee,    sensorHighSpeed)
#pragma config(Motor,  motorA, M1,      tmotorNXT, PIDControl, driveRight, encoder)
#pragma config(Motor,  motorC, M2,      tmotorNXT, PIDControl, driveLeft, encoder)
//*!!Code automatically generated by 'ROBOTC' configuration wizard               !!*//

#pragma DebuggerWindows("debugStream")
#include "drivers/mindsensors-imu.h"

/* notes
57600
xbee on PC source addr 4 dest 3
xbee on nxt source 3 dest 4

1 touch
2 Mindsensor Absolute IMU
4 xbee (485)

A M1 driveRight
C M2 driveLeft

*/

#define WDS writeDebugStream

void WaitMotorsIdle()
{
	while (nMotorRunState[M1] != runStateIdle && nMotorRunState[M1] != runStateIdle)
		wait1Msec(20);
}

task main()
{
	clearDebugStream();
	WDS("tachoTest.c\n");

	nSyncedMotors = synchCA;
	nMotorPIDSpeedCtrl[M1] = mtrSpeedReg;
	nMotorPIDSpeedCtrl[M2] = mtrSpeedReg;

	time1[T1] = 0;

	// make a move
	motor[M2] = 30;
	wait1Msec(2000);
	motor[M2] = 0;
	WaitMotorsIdle();

	WDS("Done\n");
	wait1Msec(100);
	stopAllTasks();
}