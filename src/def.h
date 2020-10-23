#include "Arduino.h"


typedef enum {
  LOWB = 1,
  MIDB = 2,
  HIGHB = 4
} stateMotor;


typedef struct {
  int pin_state = LOW;
  int ultimaLettura = LOW;
  stateMotor state;
  int readpin;
} buttonState;



typedef struct {
  int power = 0;
  String mod = "OFF";
  int temp = 0;
  int powerAuto = 0;
} powerModFan;


typedef enum {
  PLOWB = 85,
  PMIDB = 114,
  PHIGHB = 142
} powerStateMotor;




typedef struct {
  int debounceJoystick = 0;
  int buttonLast = 0;
  int button = 0;
  int stateJ = 0;
} manageJoystick;
