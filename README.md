<a id="library"></a>

# gbj\_appeeprom
This is an application library for processing application parameters stored in EEPROM, which is used usually as a project library
for particular PlatformIO project. It provides common methods for project centric application libraries and acts as their parent class.

- Library has no virtual methods, so that it is useful for project libraries without internal timers.
- Library utilizes error handling from the parent class.
- Library works strictly with byte application parameters.
- All application parameters, generic as well as defined by derived child project libraries are read from EEPROM in their `begin` method within `setup` function of a sketch.

## Generic application parameters
Library provides following generic application parameters that are utilized usually in most of projects.

#### periodPublish
Time period in millisecond for publishing telemetry data to IoT platform, usually _ThingsBoard_.
- Minimum: 5
- Maximum: 30
- Default: 15

#### smoothFactor
Factor for statistical smoothing of measured values by exponential filtering stored as the 10 multiple in order to store it as a byte. So that it is rounded to 1 decimal place. The conversion ensures corresponding [setter](#setPeriodPublish) and [getter](#getPeriodPublish).
- Minimum: 0.1
- Maximum: 1.0
- Default: 0.2


<a id="dependency"></a>

## Dependency

#### Arduino platform
- **Arduino.h**: Main include file for the Arduino SDK.

#### Espressif ESP8266 platform
- **Arduino.h**: Main include file for the Arduino platform.

#### Espressif ESP32 platform
- **Arduino.h**: Main include file for the Arduino platform.

#### Particle platform
- **Particle.h**: Includes alternative (C++) data type definitions.


<a id="constants"></a>

## Constants

- **gbj\_appeeprom::VERSION**: Name and semantic version of the library.


<a id="interface"></a>

## Interface

- [gbj_appeeprom()](#constructor)
- [getPrmStart()](#getPrmStart)
- [getPrmCount()](#getPrmCount)


##### Methods for generic application parameters

- [setPeriodPublish()](#setPeriodPublish)
- [getPeriodPublish()](#getPeriodPublish)
- [setSmoothFactor()](#setSmoothFactor)
- [getSmoothFactor()](#getSmoothFactor)


<a id="constructor"></a>
## gbj_appeeprom()

#### Description
Constructor creates the class instance object and initiates internal resources.

#### Syntax
    gbj_appeeprom(unsigned int prmStart, byte prmCount)

#### Parameters

- **prmStart**: The start position in EEPROM for saving application parameters.
The method adjusts the start position towards beginning of the EEPROM to
store all application parameters for corresponding EEPROM capacity of
related platform.
  - *Valid values*: non-negative integer
  - *Default value*: none, but usually 0 defined by a child class


- **prmCount**: The number of byte application parameters to process.
  - *Valid values*: 0 ~ 255
  - *Default value*: none

#### Returns
Object enabling EEPROM management.

[Back to interface](#interface)


<a id="getPrmStart"></a>

## getPrmStart()

#### Description
The method returns starting position in EEPROM for storing application parameters. This position is initiated and sanitized in the [constructor](#constructor).

#### Syntax
    unsigned int getPrmStart()

#### Parameters
None

#### Returns
The starting position in EEPROM for storing application parameters.

#### See also
[getPrmCount()](#getPrmCount)

[Back to interface](#interface)


<a id="getPrmCount"></a>

## getPrmCount()

#### Description
The method returns number of application parameters that should be stored in EEPROM. This number is initiated in the [constructor](#constructor).

#### Syntax
    byte getPrmCount()

#### Parameters
None

#### Returns
The number of byte application parameters in EEPROM.

#### See also
[getPrmStart()](#getPrmStart)

[Back to interface](#interface)


<a id="setPeriodPublish"></a>

## setPeriodPublish()

#### Description
The method sanitizes and saves to EEPROM the provided time period in milliseconds for publishing to IoT platform.
- Method sanitizes provided value by limiting it to the range from minimal to maximal value.
- Method stores the sanitized value in the parameter cache.
- Method really saves the sanitized value to EEPROM only if it is new, i.e., it differs from value stored in cache before.

#### Syntax
    void setPeriodPublish(byte value)

#### Parameters
- **value**: Time period in milliseconds for publishing to IoT platform.
  - *Valid values*: 5 ~ 30
  - *Default value*: 15

#### Returns
None

#### See also
[getPeriodPublish()](#getPeriodPublish)

[Back to interface](#interface)


<a id="getPeriodPublish"></a>

## getPeriodPublish()

#### Description
The method returns current time period in milliseconds for publishing to IoT platform.
- The value is taken from the parameter cache in RAM, not read from EEPROM.

#### Syntax
    byte getPeriodPublish()

#### Parameters
None

#### Returns
Time period to IoT platform in milliseconds.

#### See also
[setPeriodPublish()](#setPeriodPublish)

[Back to interface](#interface)


<a id="setSmoothFactor"></a>

## setSmoothFactor()

#### Description
The method sanitizes and saves to EEPROM the provided smoothing factor for statistical smoothing by exponential filtering.
- The factor is stored in EEPROM as the 10 multiple of real factor value and rounded to 1 decimal place in order to put it into one byte.
- Method sanitizes provided value by limiting it to the range from minimal to maximal value.
- Method really saves the sanitized value to EEPROM only if it is new, i.e., it differs from value stored in cache before.

#### Syntax
    void setSmoothFactor(float value)

#### Parameters
- **value**: Smoothing factor for exponential filtering.
  - *Valid values*: 0.1 ~ 1.0
  - *Default value*: 0.2

#### Returns
None

#### See also
[getSmoothFactor()](#getSmoothFactor)

[Back to interface](#interface)


<a id="getSmoothFactor"></a>

## getSmoothFactor()

#### Description
The method returns smoothing factor for statistical smoothing by exponential filtering.
- The value is taken from the parameter cache in RAM, not read from EEPROM.

#### Syntax
    float getSmoothFactor()

#### Parameters
None

#### Returns
Smoothing factor for exponential filtering.

#### See also
[setSmoothFactor()](#setSmoothFactor)

[Back to interface](#interface)