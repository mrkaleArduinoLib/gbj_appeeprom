/*
  NAME:
  gbj_appeeprom

  DESCRIPTION:
  Project library for processing an MCU built-in EEPROM for storing parameters.

  LICENSE:
  This program is free software; you can redistribute it and/or modify
  it under the terms of the license GNU GPL v3
  http://www.gnu.org/licenses/gpl-3.0.html (related to original code) and MIT
  License (MIT) for added code.

  CREDENTIALS:
  Author: Libor Gabaj
 */
#ifndef GBJ_APPEEPROM_H
#define GBJ_APPEEPROM_H

#if defined(__AVR__)
#include <Arduino.h>
#include <inttypes.h>
#elif defined(ESP8266)
#include <Arduino.h>
#elif defined(ESP32)
#include <Arduino.h>
#elif defined(PARTICLE)
#include <Particle.h>
#endif
#include "gbj_appcore.h"
#include "gbj_serial_debug.h"
#include <EEPROM.h>

#undef SERIAL_PREFIX
#define SERIAL_PREFIX "gbj_appeeprom"

class gbj_appeeprom : public gbj_appcore
{
public:
  static const String VERSION;

  /*
    Constructor.

    PARAMETERS:
    prmStart - The start position in EEPROM for saving application parameters.
      - Data type: non-negative integer
      - Default value: none
      - Limited range: 0 ~ 65535
    prmCount - The number of byte application parameters to process.
      - Data type: non-negative integer
      - Default value: none
      - Limited range: 0 ~ 255

    RETURN: object
  */
  inline explicit gbj_appeeprom(unsigned int prmStart, byte prmCount)
  {
    unsigned int eeprom = 4096;
#if defined(__AVR_ATmega328P__)
    eeprom = 1024;
#elif defined(PARTICLE)
    eeprom = 2047;
#endif
    prmStart_ = min(prmStart, eeprom - prmCount);
    prmCount_ = prmCount;
  }

  // Public setters
  inline void setPeriodPublish(byte value = 0)
  {
    SERIAL_VALUE("setPeriodPublish", value);
    setParameter(&periodPublish, value);
  }
  inline void setSmoothFactor(float value = 0.0)
  {
    // Smooth factor is stored in EEPROM as the 10 multiple rounded to byte.
    SERIAL_VALUE("setSmoothFactor", value);
    value = value * 10 + 0.5;
    setParameter(&smoothFactor, (byte)value);
  }

  // Public getters
  inline unsigned int getPrmStart() { return prmStart_; }
  inline byte getPrmCount() { return prmCount_; }
  inline byte getPeriodPublish() { return periodPublish.get(); }
  inline float getSmoothFactor() { return (float)smoothFactor.get() / 10.0; }

private:
  unsigned int prmStart_;
  byte prmCount_;

protected:
  struct Parameter
  {
    byte val;
    byte idx;
    const byte min;
    const byte max;
    const byte dft;
    byte get()
    {
      return val;
    }
    byte set(byte value)
    {
      val = (value < min || value > max) ? dft : value;
      return get();
    }
  };
  // Generic parameters
  Parameter periodPublish = {.min = 5, .max = 30, .dft = 15};
  Parameter smoothFactor = {.min = 1, .max = 10, .dft = 2};
  /*
    Initialization.

    DESCRIPTION:
    The method should be called in the SETUP section of a sketch. It reads
    parameters from EEPROM.

    PARAMETERS:
    pointers - Array of pointers to parameters structures.
      - Data type: pointer to Parameter
      - Default value: none
      - Limited range: none

    RETURN: Result code.
  */
  inline ResultCodes begin(Parameter *prmPointers[])
  {
    SERIAL_TITLE("begin");
#if defined(ESP8266) || defined(ESP32)
    EEPROM.begin(constrain(prmCount_, 4, 4096));
#endif
    // Read parameters from EEPROM
    for (byte i = 0; i < prmCount_; i++)
    {
      prmPointers[i]->set(EEPROM.read(prmStart_ + i));
    }
    return getLastResult();
  }
  // Parameter setter
  inline void setParameter(Parameter *prmPointer, byte value)
  {
    if (prmPointer->get() != prmPointer->set(value))
    {
      EEPROM.write(prmStart_ + prmPointer->idx, prmPointer->get());
#if defined(ESP8266) || defined(ESP32)
      EEPROM.commit();
#endif
    }
  }
};

#endif
