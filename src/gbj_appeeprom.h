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

#include <Arduino.h>
#if defined(__AVR__)
  #include <inttypes.h>
#elif defined(ESP32)
  #include <vector>
#endif
#include "gbj_appcore.h"
#include "gbj_serial_debug.h"
#include <EEPROM.h>

#undef SERIAL_PREFIX
#define SERIAL_PREFIX "gbj_appeeprom"

class gbj_appeeprom : public gbj_appcore
{
public:
  // Don't use any member initialization values in order to initialize instances
  // by braced initialization lists later.
  struct Parameter
  {
    byte val;
    unsigned long tsSet;
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
        tsSet = millis();
        val = value;
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
      tsSet = millis();
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
      tsSet = millis();
      return get();
    }
    void save()
    {
      EEPROM.write(mem, get());
#if defined(ESP8266) || defined(ESP32)
      EEPROM.commit();
#endif
      tsSet = 0;
      SERIAL_LOG4("Saved <", mem, ">: ", get())
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
  inline explicit gbj_appeeprom(unsigned int prmStart) { prmStart_ = prmStart; }

  // Reset all parameters to default values
  inline void reset()
  {
    for (Parameter *prm : prmPointers)
    {
      prm->set(0xFF);
    }
    list();
  }

  /*
      Delayed writting to EEPROM.

      DESCRIPTION:
      The method stores those parameters to EEPROM, which has been changes not
      sooner than preddefined time period.
      - The method should be called in the LOOP section of a sketch.

      PARAMETERS:
      flForce - Flag about immediate saving to EEPROM. It is needed right before
      MCU restart.

      RETURN: none
  */
  inline void run(bool flForce = false)
  {
    for (Parameter *prm : prmPointers)
    {
      if (flForce || (prm->tsSet > 0 && millis() - prm->tsSet > interval_))
      {
        prm->save();
      }
    }
  }

  // Setters & Getter

  // Set store delay period as unsigned long in milliseconds
  inline void setPeriod(unsigned long period = Timing::INTERVAL_SAVE)
  {
    interval_ = period;
  }
  // Set store delay interval as String in seconds
  inline void setPeriod(String periodSec) { interval_ = periodSec.toInt(); }
  // Return start position in the EEPROM
  inline unsigned int getPrmStart() { return prmStart_; }
  // Return number of parameters in the EEPROM
  inline byte getPrmCount() { return prmPointers.size(); }
  // Return time interval in milliseconds for delayed savings to the EEPROM
  inline unsigned long getPeriod() { return interval_; }

protected:
  std::vector<Parameter *> prmPointers;
  inline void begin()
  {
    unsigned int eeprom = 4096;
#if defined(__AVR_ATmega328P__)
    eeprom = 1024;
#endif
#if defined(ESP8266) || defined(ESP32)
    EEPROM.begin(constrain(prmPointers.size(), 4, 4096));
#endif
    // Read parameters from EEPROM
    prmStart_ = min(prmStart_, eeprom - prmPointers.size());
    unsigned int pos = prmStart_;
    for (Parameter *prm : prmPointers)
    {
      prm->mem = pos++;
      prm->val = EEPROM.read(prm->mem);
      prm->set(prm->val);
    }
    setPeriod();
    list();
  }

private:
  enum Timing : unsigned long
  {
    // Default store delay interval
    INTERVAL_SAVE = 3000,
  };
  unsigned int prmStart_;
  unsigned long interval_;
  // The method outputs all parameters' values from EEPROM if debug is enabled.
  inline void list()
  {
#ifndef SERIAL_NODEBUG
    String msg;
    for (byte i = 0; i < prmPointers.size(); i++)
    {
      msg = "[" + String(i) + "]: " + String(prmPointers[i]->get());
      SERIAL_LOG1(msg);
    }
#endif
  }
};

#endif
