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
  const char *VERSION = "GBJ_APPEEPROM 1.2.0";

  struct Parameter
  {
    byte val;
    unsigned int mem;
    const byte min;
    const byte max;
    const byte dft;
    byte get() { return constrain(val, min, max); }
    byte set(byte value)
    {
      value = (value < min || value > max) ? dft : value;
      if (val != value)
      {
        val = value;
        save();
      };
      return get();
    }
    byte cycleUp()
    {
      if (val == max)
      {
        val = min;
      }
      else
      {
        val++;
      }
      save();
      return get();
    }
    byte cycleDown()
    {
      if (val == min)
      {
        val = max;
      }
      else
      {
        val--;
      }
      save();
      return get();
    }
    void save()
    {
      EEPROM.write(mem, get());
#if defined(ESP8266) || defined(ESP32)
      EEPROM.commit();
#endif
    }
  };

  /*
    Constructor.

    PARAMETERS:
    prmStart - The start position in EEPROM for saving application parameters.
      - Data type: non-negative integer
      - Default value: none
      - Limited range: 0 ~ 65535

    RETURN: object
  */
  inline explicit gbj_appeeprom(unsigned int prmStart)
  {
    prmStart_ = prmStart;
  }

  // Reset all parameters to default values
  void reset()
  {
    SERIAL_TITLE("reset");
    for (byte i = 0; i < prmPointers_.size(); i++)
    {
      prmPointers_[i]->set(0xFF);
#ifndef SERIAL_NODEBUG
      String msg = "[" + String(i) + "]: " + String(prmPointers_[i]->get());
      SERIAL_LOG1(msg);
#endif
    }
  }

  // Getters
  inline unsigned int getPrmStart() { return prmStart_; }
  inline byte getPrmCount() { return prmPointers_.size(); }

protected:
  std::vector<Parameter *> prmPointers_;
  /*
    Initialization.

    DESCRIPTION:
    The method should be called in the SETUP section of a sketch. It reads
    parameters from EEPROM.

    PARAMETERS:
    prmPointers - Array of pointers to parameters' structures.
      - Data type: pointer to Parameter
      - Default value: none
      - Limited range: none

    RETURN: Result code.
  */
  inline ResultCodes begin(const std::vector<Parameter *>& prmPointers)
  {
    prmPointers_ = prmPointers;
    SERIAL_TITLE("begin");
    unsigned int eeprom = 4096;
#if defined(__AVR_ATmega328P__)
    eeprom = 1024;
#elif defined(PARTICLE)
    eeprom = 2047;
#endif
#if defined(ESP8266) || defined(ESP32)
    EEPROM.begin(constrain(prmPointers_.size(), 4, 4096));
#endif
    prmStart_ = min(prmStart_, eeprom - prmPointers_.size());
    // Read parameters from EEPROM
    for (byte i = 0; i < prmPointers_.size(); i++)
    {
      prmPointers_[i]->mem = prmStart_ + i;
      prmPointers_[i]->val = EEPROM.read(prmPointers_[i]->mem);
      prmPointers_[i]->set(prmPointers_[i]->val);
#ifndef SERIAL_NODEBUG
      String msg = "[" + String(i) + "]: " + String(prmPointers_[i]->get());
      SERIAL_LOG1(msg);
#endif
    }
      return setLastResult();
  }

private:
  unsigned int prmStart_;
};

#endif
