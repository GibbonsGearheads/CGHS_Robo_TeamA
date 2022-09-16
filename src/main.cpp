/*----------------------------------------------------------------------------*/
/*                                                                            */
/*    Module:       main.cpp                                                  */
/*    Author:       getom                                                     */
/*    Created:      Mon Sep 12 2022                                           */
/*    Description:  V5 project                                                */
/*                                                                            */
/*----------------------------------------------------------------------------*/

#include "vex.h"

using namespace vex;

// A global instance of competition
competition Competition;
bool isInCompetition;

brain Brain;
controller Controller;

// Controller Screen Variables
int drawControllerInterval = 0;
bool drawControllerBool = false;

// Left Motors and Motor Group
motor MotorLeftFront = motor(PORT1, ratio18_1, true);
motor MotorLeftBack = motor(PORT2, ratio18_1, true);
motor_group MotorGroupLeft = motor_group(MotorLeftFront, MotorLeftBack);

// Right Motors and Motor Group
motor MotorRightFront = motor(PORT3, ratio18_1, false);
motor MotorRightBack = motor(PORT4, ratio18_1, false);
motor_group MotorGroupRight = motor_group(MotorRightFront, MotorRightBack);

// Controller Variable Raw and Processed
float axisLeft_Raw;
float axisRight_Raw;
float axisStrafe_Raw;
float axisRotate_Raw;

float axisLeft;
float axisRight;
float axisStrafe;
float axisRotate;

float slowRotateConst = 0.2;

void pre_auton(void) {
  // All activities that occur before the competition starts
  // Example: clearing encoders, setting servo positions, ...

  // Check if in actual Competition
  isInCompetition = Competition.isCompetitionSwitch();
}


float ProcessAxis(float input) {

  float result = input;
  //   set to zero if close enough to zero (for sensitive controllers)
  if (result <= 5 && result >= -5)
  {
    result = 0;
  }
  // set to max if basically at max
  if (result <= -95)
  {
    result = -100;
  }
  if (result >= 95)
  {
    result = 100;
  }
  
  return result;
}

void DrawControllerScreen() {

  if (!drawControllerBool)
  {
    return;
  }
  
  // clear screen
  Controller.Screen.clearScreen();
  // display left axis percent
  Controller.Screen.setCursor(1,1);
  Controller.Screen.print("Left Axis: ");
  Controller.Screen.print(axisLeft);
  // display right axis percent
  Controller.Screen.setCursor(2,1);
  Controller.Screen.print("Right Axis: ");
  Controller.Screen.print(axisRight);
}


void autonomous(void) {
  // ..........................................................................
  // Insert autonomous user code here.
  // ..........................................................................
}

void usercontrol(void) {
  // User control code here, inside the loop
  while (1) {
    
    // Get raw controller Input
    axisLeft_Raw = Controller.Axis3.position();
    axisRight_Raw = Controller.Axis2.position();
    axisStrafe_Raw = Controller.Axis4.position();
    axisRotate_Raw = Controller.Axis1.position();
    
    // Process Input
    axisLeft = ProcessAxis(axisLeft_Raw);
    axisRight = ProcessAxis(axisRight_Raw);
    axisStrafe = ProcessAxis(axisStrafe_Raw);
    axisRotate = ProcessAxis(axisRotate_Raw);

    
    // Apply to motors
    if (axisRotate != 0) {
      // if slowly rotating
      MotorGroupLeft.setVelocity(axisRotate * slowRotateConst, percent);
      MotorGroupRight.setVelocity(axisRotate * slowRotateConst * -1, percent); 
    } else {
      // If nothing else, set velocity as normal
      MotorGroupLeft.setVelocity(axisLeft, percent);
      MotorGroupRight.setVelocity(axisRight, percent);
    }
    MotorGroupLeft.spin(fwd);
    MotorGroupRight.spin(fwd);

    // draw controller
    if (drawControllerInterval >= 2)
    {
      DrawControllerScreen();
      drawControllerInterval = 0;
    }
    drawControllerInterval++;

    wait(20, msec); // Sleep the task for a short amount of time to
                    // prevent wasted resources.
  }
}

// Main will set up the competition functions and callbacks.

int main() {
  // Set up callbacks for autonomous and driver control periods.
  Competition.autonomous(autonomous);
  Competition.drivercontrol(usercontrol);

  //test
  // Run the pre-autonomous function.
  pre_auton();

  // Check if in actual competition
  Brain.Screen.clearScreen();
  Brain.Screen.setCursor(1, 1);
  Brain.Screen.print("IN COMPETITION? = ");
  if (!isInCompetition)
  {
    usercontrol();
  }

  // Prevent main from exiting with an infinite loop.
  while (true) {
    wait(100, msec);
  }
}
