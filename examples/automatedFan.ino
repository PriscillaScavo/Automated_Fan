#include "FeatureFan.h"
#include "TaskScheduler.h"

#define POWER_MOTOR 3

#define CONTROL_B1 10
#define CONTROL_B2 8
#define CONTROL_B3 7

#define PIN_SERVO 9
#define TRIG_PIN  A0
#define ECHO_PIN     A1
#define JOYSTICK_Y A4
#define JOYSTICK_X A3
#define JOYSTICK_BUTTON 13

FeatureFan *fan;
int power;
String fanMode;

void runMotor();
void autoMode();
void customMode();
void readButtons();
void joystickControl();
void sr04();

Scheduler runner;

Task customModeTask(10, TASK_FOREVER, customMode);

Task readButtonsTask(10, TASK_FOREVER, readButtons);
Task runMotorTask(100, TASK_FOREVER, runMotor);


Task manageAutoModeTask(1000, TASK_FOREVER, autoMode);

Task joystickTask(100, TASK_FOREVER, joystickControl);
Task sr04Task(100, TASK_FOREVER, sr04);





void runMotor()
{
  power = fan -> checkStateB();
  fan -> powerMotorMode(power);
}

void autoMode()
{
  if (fan -> currentFanMode() != "OFF") {
      fan -> manageAutoMode();
  }
}

void customMode()
{
  if (fan -> currentFanMode() != "OFF") {
      fan -> customMode();
  }
  
}
void readButtons()
{
  fan -> read_buttons();
}
void joystickControl()
{
  if (fan -> currentFanMode() != "OFF") {
    fan -> joystickControl();
  }
}
void sr04()
{
  if (fan -> currentFanMode() != "OFF" &&  (!fan -> isJoystickActive()) ) {
    fan -> sr04();  
  }
  
}



void setup()
{
  Serial.begin(115200);
  fan = new FeatureFan(POWER_MOTOR, CONTROL_B1, CONTROL_B2, CONTROL_B3, PIN_SERVO, TRIG_PIN, ECHO_PIN, JOYSTICK_Y, JOYSTICK_X, JOYSTICK_BUTTON, 12, 11, 6, 5, 4, 2);
  fan -> activeAutoMode();
  fan -> activeCustomMode();
  
  runner.init();
  
  runner.addTask(readButtonsTask);
  readButtonsTask.enable();

  runner.addTask(runMotorTask);
  runMotorTask.enable();

  runner.addTask(manageAutoModeTask);
  manageAutoModeTask.enable();
  
  runner.addTask(customModeTask);
  customModeTask.enable();
  
  runner.addTask(joystickTask);
  joystickTask.enable();
  
  runner.addTask(sr04Task);
  sr04Task.enable();


}


void loop()
{
  runner.execute();


}
