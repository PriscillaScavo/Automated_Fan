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
               uint8_t trig_pin, uint8_t echo_pin, uint8_t joystick_y, uint8_t joystick_button, uint8_t rsLcd, 
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
      lcd(rsLcd,eLcd,d4Lcd, d5Lcd, d6Lcd, d7Lcd)
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
      Wire.write(0x6B); // PWR_MGMT_1 register
      Wire.write(0); // set to zero (wakes up the MPU-6050)
      Wire.endTransmission(true);

      lcd.begin(16, 2);

    }
    //functions
    /**
      * @brief  If mode of the fan is different from "OFF", 
      * the servo rotation is managed and if the custom mode and the auto mode are actived, they are managed.
       * @param  None.
       * @retval None.
       */
    virtual void off_on()
    {
      if (powerMod.mod != "OFF") {
        servoControl();
        customMode();
        manageAutoMode();

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
       * @retval The power chosen for the motor.
       */
    virtual int read_buttons()
    {
      button1.readpin = digitalRead(control_b1);
      button2.readpin = digitalRead(control_b2);
      button3.readpin = digitalRead(control_b3);

      readB(&button1);
      readB(&button2);
      readB(&button3);

      checkStateB();
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


  protected:
    void displayMode(int p, String m);
    void checkStateB();
    void readB(buttonDebounce *buttonState) ;
    void processCommand(String command);
    int String_to_int(String s);
    void sr04();
    void customMode();
    void manageAutoMode();
    void servoControl();
    void autoMode();
    void changePowerMod(byte state);

    //variables
    uint8_t power_motor;
    uint8_t control_b1;
    uint8_t control_b2;
    uint8_t control_b3;
    uint8_t pin_servo;
    uint8_t trig_pin;
    uint8_t echo_pin;
    uint8_t joystick_y;
    uint8_t joystick_button;
    
    /*uint8_t rsLcd; 
    uint8_t eLcd;
    uint8_t d4Lcd,; 
    uint8_t d5Lcd;
    uint8_t d6Lcd; 
    uint8_t d7Lcd; 
    */
    
    int powerAuto = 0;
    const int MPU_ADDR = 0x68; 
    int16_t temperature;




    unsigned long ultimoDebounceStateB = 0;
    byte stateB = 0;
    unsigned long debounceDistanza = millis();

    buttonDebounce button1;
    buttonDebounce button2;
    buttonDebounce button3;


    int useCustomMode = 0;
    int useAutoMode = 0;


    String command = "";
    int  currentAngle = 0;
    int directionRotation = 0;
    manageJoystick joistick;
    powerModFan powerMod;
    int countZero = 0;
    Servo servo;
    NewPing sonar;
    LiquidCrystal lcd;
};
