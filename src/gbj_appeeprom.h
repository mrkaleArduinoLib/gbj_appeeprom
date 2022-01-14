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

  typedef void Handler(byte);

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
    onSave - Pointer to a callback function that receives index of a parameter,
      which has been currently stored to EEPROM and returns no value.
      - Data type: Handler
      - Default value: 0
      - Limited range: system address range

    RETURN: object
  */
  inline explicit gbj_appeeprom(unsigned int prmStart,
                                byte prmCount,
                                Handler *onSave = 0)
  {
    unsigned int eeprom = 4096;
#if defined(__AVR_ATmega328P__)
    eeprom = 1024;
#elif defined(PARTICLE)
    eeprom = 2047;
#endif
    prmStart_ = min(prmStart, eeprom - prmCount);
    prmCount_ = prmCount;
    onSave_ = onSave;
  }

  // Reset all parameters to default values
  void reset()
  {
    SERIAL_TITLE("reset");
    for (byte i = 0; i < prmCount_; i++)
    {
      setParameter(i, 0xFF);
#ifndef SERIAL_NODEBUG
      String msg = "[" + String(i) + "]: " + String(getPrmValue(i));
      SERIAL_LOG1(msg);
#endif
    }
  }

  // Setters for generic parameters
  inline void setMcuRestarts(byte value = 0)
  {
    SERIAL_VALUE("setMcuRestarts", value);
    setParameter(&mcuRestarts, value);
  }
  inline void setPeriodPublish(byte value = 0)
  {
    SERIAL_VALUE("setPeriodPublish", value);
    setParameter(&periodPublish, value);
  }

  // Getters
  inline unsigned int getPrmStart() { return prmStart_; }
  inline byte getPrmCount() { return prmCount_; }
  inline byte getPrmValue(byte idx) { return prmPointers_[idx]->get(); }
  // Generic parameters
  inline byte getMcuRestarts() { return mcuRestarts.get(); }
  inline byte getPeriodPublish() { return periodPublish.get(); }

protected:
  struct Parameter
  {
    byte val;
    byte idx;
    bool chg;
    const byte min;
    const byte max;
    const byte dft;
    byte get() { return constrain(val, min, max); }
    byte set(byte value)
    {
      val = (value < min || value > max) ? dft : value;
      chg = (val != value);
      return get();
    }
    byte cycle()
    {
      val++;
      val = val > max ? min : val;
      chg = true;
      return get();
    }
    byte cycleDown()
    {
      val--;
      val = val < min ? max : val;
      chg = true;
      return get();
    }
    bool change() { return chg; }
  };
  Parameter **prmPointers_;

  // Generic parameters (255 (0xFF, -1) is factory value)
  Parameter mcuRestarts = { .min = 0, .max = 254, .dft = 0 };
  Parameter periodPublish = { .min = 5, .max = 30, .dft = 15 };

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
  inline ResultCodes begin(Parameter **prmPointers)
  {
    prmPointers_ = prmPointers;
    SERIAL_TITLE("begin");
#if defined(ESP8266) || defined(ESP32)
    EEPROM.begin(constrain(prmCount_, 4, 4096));
#endif
    // Read parameters from EEPROM
    for (byte i = 0; i < prmCount_; i++)
    {
      prmPointers[i]->idx = i;
      prmPointers[i]->set(EEPROM.read(prmStart_ + i));
#ifndef SERIAL_NODEBUG
      String msg = "[" + String(i) + "]: " + String(prmPointers_[i]->get());
      SERIAL_LOG1(msg);
#endif
    }
    return setLastResult();
  }

  // Parameter setter
  inline void setParameter(Parameter *prmPointer, byte value)
  {
    if (prmPointer->get() != prmPointer->set(value))
    {
      storeParameter(prmPointer);
    }
  }
  inline void setParameter(byte idx, byte value)
  {
    setParameter(prmPointers_[idx], value);
  }

  // Save parameter to EEPROM
  inline void storeParameter(Parameter *prmPointer)
  {
    EEPROM.write(prmStart_ + prmPointer->idx, prmPointer->get());
#if defined(ESP8266) || defined(ESP32)
    EEPROM.commit();
#endif
    if (onSave_ && prmPointer->change())
    {
      onSave_(prmPointer->idx);
    }
  }
  inline void storeParameter(byte idx) { storeParameter(prmPointers_[idx]); }

private:
  unsigned int prmStart_;
  byte prmCount_;
  Handler *onSave_;
};

#endif
