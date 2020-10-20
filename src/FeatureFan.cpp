#include "FeatureFan.h"



/**
   * @brief  Begins a transmission to the I2C.
   *  The reading of the registers  0x41 (TEMP_OUT_H) and 0x42 (TEMP_OUT_L) is requested.
   *  The temperature is calculated using an equation taken from the documentation (MPU-6000/MPU-6050 Register Map and Description, p.30).
   * @param  None.
   * @retval None.
   */
void FeatureFan::autoMode()
{
  Wire.beginTransmission(MPU_ADDR);
  Wire.write(0x41);
  Wire.endTransmission(false);
  Wire.requestFrom(MPU_ADDR, 2, true); // request a total of 2 registers


  temperature = Wire.read() << 8 | Wire.read();

  powerMod.temp = (temperature / 340 + 36);
  if (powerMod.temp >= 25) {
    powerMod.power = PHIGHB;
  } else if (powerMod.temp >= 21) {
    powerMod.power = PMIDB;
  } else {
    powerMod.power = PLOWB;
  }

}

/**
   * @brief  Show on the display the power of the motor and the mode of the fan.
   * @param  The power of the motor and the mode of the fan.
   * @retval None.
   */
void FeatureFan::displayMode(int p, String m)
{
  String endS = " V";
  if (m == "CUSTOM_AUTO" || m == "AUTO") {
    endS += ";" + String(powerMod.temp) + char(0xDF) + "C";
  }
  if (m == "CUSTOM_AUTO") {
    m = "CUSTOM";
  }
  String s1 = "Mod: " + m ;
  String s2 = "Power: " + String((p * 9) / 255) + endS;
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print(s1);
  lcd.setCursor(0, 1);
  lcd.print(s2);
}

/**
   * @brief  Change the power of the motor and the mode of the fan according to the parameter state.
   * @param  Number indicating the desired combination of power of the motor and mode of the fan.
   * @retval None.
   */
void FeatureFan::changePowerMod(byte state)
{
  switch (stateB) {
    case 1: powerMod.power = PLOWB; powerMod.mod = "LOW"; break;
    case 2: powerMod.power = PMIDB; powerMod.mod = "MID"; break;
    case 4: powerMod.power = PHIGHB; powerMod.mod = "HIGH"; break;
    case 3: if (useAutoMode) {
        autoMode();
        powerMod.mod = "AUTO";
      } break;
    case 5: if (useCustomMode) {
        powerMod.power = 0;
        powerMod.mod = "CUSTOM";
      } break;
    case 6: powerMod.power = 0; powerMod.mod = "OFF";  break;

  }
}

/**
   * @brief  ultimoTempoDebounce = millis is updated if the new pin reading is different from the last one.
   * If since the last update of ultimoTempoDebounce has passed a time equal to at least attesaDebounce,
   * it is evaluated if the value just read is different from pin_state and if it is HIGH,
   * at that point it means that the button has been pressed and the stateB command is updated and pin_state is updated  with the value just read.
   * @param  None.
   * @retval None.
   */
void FeatureFan::readB(buttonDebounce *buttonState)
{
  if (buttonState->readpin != buttonState->ultimaLettura) {
    buttonState->ultimoTempoDebounce = millis();
  }

  if ((millis() - buttonState->ultimoTempoDebounce) > buttonState->attesaDebounce) {
    if (buttonState->readpin != buttonState->pin_state and buttonState->readpin == HIGH) {
      stateB = stateB | buttonState->state;
    }

    buttonState->pin_state = buttonState->readpin;
  }
  buttonState->ultimaLettura = buttonState->readpin;

}


/**
   * @brief  Change the power of the motor in according to the parameter command and the display is uploaded.
   * @param  A string that can be: "LOW", "MID", "HIGH", "AUTO", "OFF".
   * @retval None.
   */

void FeatureFan::processCommand(String command)
{
  int r = String_to_int(command);
  switch (r) {
    case 242: powerMod.power = PLOWB; break;
    case 218: powerMod.power = PMIDB; break;
    case 288: powerMod.power = PHIGHB; break;
    case 313: if (useAutoMode) {
        powerMod.power = 0;
        powerMod.mod = "CUSTOM_AUTO";
      } break;
    case 219: powerMod.power = 0; powerMod.mod = "OFF"; break;
  }
  displayMode(powerMod.power, powerMod.mod);
}
/**
   * @brief Sums the ascii code of each character of a string.
   * @param String.
   * @retval The sum of the ASCII code of each character of a string.
   */

int FeatureFan::String_to_int(String s)
{
  int r = 0;
  for (int i = 0; i < s.length(); i++) {
    r += int (s[i]);
  }
  return r;
}
