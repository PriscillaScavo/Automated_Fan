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
Task readButtonsTask(10, TASK_FOREVER, readButtons);
Task runMotorTask(100, TASK_FOREVER, runMotor);

Task sr04Task(10, TASK_FOREVER, sr04);
Task joystickTask(100, TASK_FOREVER, joystickControl);

Task customModeTask(10, TASK_FOREVER, customMode);
Task manageAutoModeTask(10, TASK_FOREVER, autoMode);


void runMotor()
{
  power = fan -> checkStateB();
  fan -> powerMotorMode(power);
}

void autoMode()
{
  fan -> activeAutoMode();
  fan -> manageAutoMode();
}

void customMode()
{
  fan -> activeCustomMode();
  fan -> customMode();
}
void readButtons()
{
  if (fan -> currentFanMode() != "OFF") {
    joystickTask.enable();
    if (!fan -> isJoystickActive()) {
      sr04Task.enable();
    } else {
      sr04Task.disable();
    }
    customModeTask.enable();
    manageAutoModeTask.enable();
  } else {
    joystickTask.disable();
    sr04Task.disable();
    customModeTask.disable();
    manageAutoModeTask.disable();
  }

  fan -> read_buttons();
}
void joystickControl()
{
  fan -> joystickControl();
}
void sr04()
{
  fan -> sr04();
}



void setup()
{
  Serial.begin(115200);
  fan = new FeatureFan(POWER_MOTOR, CONTROL_B1, CONTROL_B2, CONTROL_B3, PIN_SERVO, TRIG_PIN, ECHO_PIN, JOYSTICK_Y, JOYSTICK_BUTTON, 12, 11, 6, 5, 4, 2);
  runner.init();
  runner.addTask(readButtonsTask);
  readButtonsTask.enable();

  runner.addTask(runMotorTask);
  runMotorTask.enable();

  runner.addTask(joystickTask);
  runner.addTask(sr04Task);
  runner.addTask(customModeTask);
  runner.addTask(manageAutoModeTask);

}


void loop()
{
  runner.execute();


}
