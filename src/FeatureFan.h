#include "Arduino.h"
#include <Wire.h>
#include <NewPing.h>
#include <Servo.h>
#include <LiquidCrystal.h>
#include "def.h"


#define MAXANGLE 179
#define MAX_DISTANCE 35
#define STEP 3


class FeatureFan {
  public:
    FeatureFan(uint8_t power_motor, uint8_t control_b1, uint8_t control_b2, uint8_t control_b3, uint8_t pin_servo,
               uint8_t trig_pin, uint8_t echo_pin, uint8_t joystick_y,uint8_t joystick_x, uint8_t joystick_button, uint8_t rsLcd,
               uint8_t eLcd, uint8_t d4Lcd, uint8_t d5Lcd, uint8_t d6Lcd, uint8_t d7Lcd):

      power_motor(power_motor),
      control_b1(control_b1),
      control_b2(control_b2),
      control_b3(control_b3),
      pin_servo(pin_servo),
      trig_pin(trig_pin),
      echo_pin(echo_pin),
      joystick_y(joystick_y),
      joystick_button(joystick_button),
      sonar(trig_pin, echo_pin, MAX_DISTANCE),
      lcd(rsLcd, eLcd, d4Lcd, d5Lcd, d6Lcd, d7Lcd)
    {


      button1.state = LOWB;
      button2.state = MIDB;
      button3.state = HIGHB;

      pinMode(control_b1, INPUT);
      pinMode(control_b2, INPUT);
      pinMode(control_b3, INPUT);

      pinMode(power_motor, OUTPUT);

      pinMode(trig_pin, OUTPUT);
      pinMode(echo_pin, INPUT);

      digitalWrite(trig_pin, LOW);
      pinMode(joystick_button, INPUT_PULLUP);
      servo.attach(pin_servo);

      Wire.begin();
      Wire.beginTransmission(MPU_ADDR);
      Wire.write(0x6B); // PWR_MGMT_1 (Power Management) register
      Wire.write(0); // Internal 8MHz oscillator
      Wire.endTransmission(true);//send a stop message, releasing the bus after transmission

      lcd.begin(16, 2);
      displayMode(0, "OFF");

    }


    //functions
    /**
       * @brief  If the mode of fan is "CUSTOM" or "CUSTOM_AUTO",
       * Serial.available get the number of bytes (characters) available for reading from the serial port.
       * Serial.read reads incoming char from the serial port.
       * @param  None.
       * @retval None.
       */

    virtual void customMode()
    {
      if (powerMod.mod == "CUSTOM" || powerMod.mod == "CUSTOM_AUTO") {
        int b = Serial.available();
        if (b) {
          for (int i = 0; i < b; i++) {
            char ch = Serial.read();
            if ((ch == '\n') || (ch == '\r')) {
              processCommand(command);
              command = "";
            } else {
              command += ch;
            }
          }
        }
      }
    }
    /**
       * @brief  If the mode of the fan is "AUTO" or "CUSTOM_AUTO", the auto mode is managed.
       * If the power is changed, the motor is actived with it and the display is uploaded.
       * @param  None.
       * @retval None.
       */
    virtual void manageAutoMode()
    {
      if (powerMod.mod == "AUTO" || powerMod.mod == "CUSTOM_AUTO") {
        autoMode();

        if (powerMod.powerAuto != powerMod.power) {
          powerMotorMode(powerMod.power);
          displayMode(powerMod.power, powerMod.mod);
        }
      }
    }
    /**
    * @brief  Manage the servo rotation.
    * The rotation is controlled by joystick and return true, if it is actived, otherwise return false.
    * @param  None.
    * @retval None.
    */
    virtual bool joystickControl()
    {
      if ((millis() - joystick.debounceJoystick) >= 100) {
        joystick.debounceJoystick = millis();
        joystick.button = !digitalRead(joystick_button);

        if (joystick.button == 1 && joystick.buttonLast == 0) {
          joystick.stateJ = !(joystick.stateJ);
        }
        joystick.buttonLast = joystick.button;
        
        
        if (joystick.stateJ) {
          int y = analogRead(joystick_y);
          int x = analogRead(joystick_x);
          int angley = map(y, 0, 1023, 0, 180);
          int anglex = map(x, 0, 1023, 0, 180);
          
          if ((angley > 120 || angley < 106 )&& (anglex > 95 || anglex < 87)) {
            servo.write(angley);
          }
          joystickActive = 1;
        } else {
          joystickActive = 0;

        }


      }
    }

    /**
       * @brief If the distance is greater than MAX_DISTANCE or equal to 0, the servo arm is rotated.
       * @param  None.
       * @retval None.
       */
    virtual void sr04()
    {
      int distanza = sonar.ping_cm();
      if (distanza < MAX_DISTANCE && distanza != 0) {
        edgeTolerance = 0;
      }
      if (distanza == 0 || distanza > MAX_DISTANCE) {
        edgeTolerance++;
      }

      if ((distanza == 0 || distanza > MAX_DISTANCE) && edgeTolerance >= 5)  {

        if (currentAngle <= 0) {
          directionRotation = !directionRotation;
          currentAngle += STEP;
        } else if (currentAngle >= MAXANGLE) {
          directionRotation = !directionRotation;
          currentAngle -= STEP;
        } else if (directionRotation) {
          currentAngle -= STEP;
        } else {
          currentAngle += STEP;
        }
        servo.write(currentAngle);
        edgeTolerance = 0;
      }
    }
    /**
       * @brief  Active the features of auto mode.
       * @param  None.
       * @retval None.
       */
    virtual void activeAutoMode()
    {
      useAutoMode = 1;
    }
    /**
       * @brief  Active the features of custom mode.
       * @param  None.
       * @retval None.
       */
    virtual void activeCustomMode()
    {
      useCustomMode = 1;
    }
    /**
       * @brief  Use the command(obtained by the combination of three buttons) to choses the power of the motor.
       * @param  None.
       * @retval None.
       */
    virtual int read_buttons()
    {
      button1.readpin = digitalRead(control_b1);
      button2.readpin = digitalRead(control_b2);
      button3.readpin = digitalRead(control_b3);

      readB(&button1);
      readB(&button2);
      readB(&button3);


    }

    /**
    * @brief  The command given by the button is checked.
    * @param  None.
    * @retval The power chosen for the motor.
    */
    virtual int checkStateB()
    {
     
      if (stateB != 0) {
        changePowerMod();
        displayMode(powerMod.power, powerMod.mod);
        
      }
      stateB = 0;
      return powerMod.power;
    }
    /**
       * @brief  Active the motor with a custom power.
       * @param  The power chosen for the motor.
       * @retval None.
       */
    virtual void powerMotorMode(int power)
    {
      analogWrite(power_motor, power);
    }

    virtual String currentFanMode()
    {
      return powerMod.mod;
    }

    virtual int isJoystickActive()
    {
      return joystickActive;
    }
  protected:
    void displayMode(int p, String m);
    void readB(buttonState *buttonState) ;
    void processCommand(String command);
    int String_to_int(String s);
    void autoMode();
    void changePowerMod();

    //variables

    uint8_t power_motor;
    uint8_t control_b1;
    uint8_t control_b2;
    uint8_t control_b3;
    uint8_t pin_servo;
    uint8_t trig_pin;
    uint8_t echo_pin;
    uint8_t joystick_y;
    uint8_t joystick_x;
    uint8_t joystick_button;
    
    
    const int MPU_ADDR = 0x68;


    byte stateB = 0;

    buttonState button1;
    buttonState button2;
    buttonState button3;
    manageJoystick joystick;
    powerModFan powerMod;

    int useCustomMode = 0;
    int useAutoMode = 0;
    int joystickActive = 0;

    String command = "";
    
    
    int  currentAngle = 0;
    int directionRotation = 0;
    int edgeTolerance = 0;
    
    Servo servo;
    NewPing sonar;
    LiquidCrystal lcd;
};
