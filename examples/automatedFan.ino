#include "FeatureFan.h"

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

void setup()
{
  Serial.begin(115200);
  fan = new FeatureFan(POWER_MOTOR, CONTROL_B1, CONTROL_B2, CONTROL_B3, PIN_SERVO, TRIG_PIN, ECHO_PIN, JOYSTICK_Y, JOYSTICK_BUTTON,12,11,6,5,4,2);

}


void loop()
{
  fan -> activeAutoMode();
  fan -> activeCustomMode();
  fan -> off_on();
  power = fan -> read_buttons();
  fan -> powerMotorMode(power);
  delay(10);
}
