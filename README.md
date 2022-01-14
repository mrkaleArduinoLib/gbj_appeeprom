<a id="library"></a>

# gbj\_appeeprom
This is an application library for processing application parameters stored in EEPROM, which is used usually as a project library
for particular PlatformIO project. It provides common methods for project centric application libraries and acts as their parent class.

- Library has no virtual methods, so that it is useful for project libraries without internal timers.
- Library utilizes error handling from the parent class.
- Library works strictly with byte application parameters. If wider parameters are needed, e.g., integer ones, they have to be divided to two (or more) parameters for individual bytes (least and most significant ones). Then corresponding setter and getter should ensure calculation to desired parameter.
- All application parameters, generic as well as defined by derived child project libraries, are read from EEPROM to the cache in RAM only in their `begin` method within `setup` function of a sketch as an initial configuration at start of a microcontroller. Then only new values of parameters are written to EEPROM, but reading is always from the cache. EEPROM serves as the persistent parameters mirroring.

## Generic application parameters
Library provides following generic application parameters that are utilized usually in most of projects.

#### periodPublish
Time period in milliseconds for publishing telemetry data to IoT platform.
- Minimum: 5
- Maximum: 30
- Default: 15

#### mcuRestarts
Number of restarts of a microcontroller. It is utilized as the MCU restarts counter. The value `255` is reserved for the _wrong_ value and can declared like `-1` as well.
- Minimum: 0
- Maximum: 254
- Default: 0


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

- [gbj_appeeprom()](#gbj_appeeprom)
- [reset()](#reset)
- [getPrmStart()](#getPrmStart)
- [getPrmCount()](#getPrmCount)
- [getPrmValue()](#getPrmValue)


##### Methods for generic application parameters

- [setPeriodPublish()](#setPeriodPublish)
- [getPeriodPublish()](#getPeriodPublish)
- [setMcuRestarts()](#setMcuRestarts)
- [getMcuRestarts()](#getMcuRestarts)


## Custom data types

* [Handler(byte)](#handler)


<a id="handler"></a>

## Handler

#### Description
The template or the signature of a callback function, which is called after every real storing to the EEPROM. It can be utilized for signalling a parameter change as well.
* A handler is just a function with one input parameter and returning nothing.
* A handler can be declared just as `void` type.

#### Syntax
    typedef void Handler(byte)

#### Parameters

- Injected index of a saved parameter to a handler implementation callback function. This index is defined by the order of a parameter in definition array (list) of EEPROM parameters in a derived (child) application injected in its begin method.
  - *Valid values*: non-negative integer
  - *Default value*: none

#### Returns
None

[Back to interface](#interface)


<a id="gbj_appeeprom"></a>

## gbj_appeeprom()

#### Description
Constructor creates the class instance object and initiates internal resources.

#### Syntax
    gbj_appeeprom(unsigned int prmStart, byte prmCount, Handler *onSave)

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


* **onSave**: Pointer to a callback function that receives one byte parameter as a paramereter index and returns no value, which is called right after storing that parameter to EEPROM.
  * *Valid values*: system address range
  * *Default value*: 0


#### Returns
Object enabling EEPROM management.

#### See also
[Handler(byte)](#handler)

[Back to interface](#interface)


<a id="reset"></a>

## reset()

#### Description
The method sets all application parameters to their default values. It is suitable for unknown or changed order of those parameters in EEPROM, e.g., at using an already used EEPROM taken from another project. So, the method should be used once after aforementioned change.

#### Syntax
    void reset()

#### Parameters
None

#### Returns
None

[Back to interface](#interface)


<a id="getPrmStart"></a>

## getPrmStart()

#### Description
The method returns starting position in EEPROM for storing application parameters. This position is initiated and sanitized in the [constructor](#gbj_appeeprom).

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
The method returns number of application parameters that should be stored in EEPROM. This number is initiated in the [constructor](#gbj_appeeprom).

#### Syntax
    byte getPrmCount()

#### Parameters
None

#### Returns
The number of byte application parameters in EEPROM.

#### See also
[getPrmStart()](#getPrmStart)

[Back to interface](#interface)


<a id="getPrmValue"></a>

## getPrmValue()

#### Description
The method returns value of a parameter from the cache, which is defined by an index of that parameter.

#### Syntax
    byte getPrmValue(byte idx)

#### Parameters

- **idx**: Index of a parameter in a definition array (list) of EEPROM parameters in a derived (child) application.
  - *Valid values*: non-negative integer
  - *Default value*: none


#### Returns
The value of a parameter from EEPROM cache.

#### See also
[Handler(byte)](#handler)

[Back to interface](#interface)


<a id="setPeriodPublish"></a>

## setPeriodPublish()

#### Description
The method sanitizes and saves the provided time period in milliseconds for publishing to an IoT platform.
- Method sanitizes provided value by setting it to the default value, if it is outside of range from minimal to maximal value.
- Method stores the sanitized value in the parameter cache.
- Method really saves the sanitized value to EEPROM only if it is new, i.e., it differs from value stored in the cache before.

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
- The value is taken from the parameter cache in RAM, not read from EEPROM directly.
- The value (alongside with other parameters) is read from EEPROM just at the beginning of a sketch at microcontroller start.

#### Syntax
    byte getPeriodPublish()

#### Parameters
None

#### Returns
Time period to IoT platform in milliseconds.

#### See also
[setPeriodPublish()](#setPeriodPublish)

[Back to interface](#interface)


<a id="setMcuRestarts"></a>

## setMcuRestarts()

#### Description
The method sanitizes and saves the provided number of the microcontroller restarts. It is usually utilized by the WiFi connection management of a project.
- Method sanitizes provided value by setting it to the default value, if it is outside of range from minimal to maximal value.
- Method stores the sanitized value in the parameter cache.
- Method really saves the sanitized value to EEPROM only if it is new, i.e., it differs from value stored in the cache before.

#### Syntax
    void setMcuRestarts(byte value)

#### Parameters
- **value**: Number of MCU restarts. Value `255` is reserved as a _wrong_ parameter value. No provided value resets the restart counter.
  - *Valid values*: 0 ~ 254
  - *Default value*: 0

#### Returns
None

#### See also
[getMcuRestarts()](#getMcuRestarts)

[Back to interface](#interface)


<a id="getMcuRestarts"></a>

## getMcuRestarts()

#### Description
The method returns current number of the microcontroller restarts. It is usually utilized for initialization of a WiFi connection management.
- The value is taken from the parameter cache in RAM, not read from EEPROM directly.
- The value (alongside with other parameters) is read from EEPROM just at the beginning of a sketch at microcontroller start.

#### Syntax
    byte getMcuRestarts()

#### Parameters
None

#### Returns
Number of MCU restarts.

#### See also
[setMcuRestarts()](#setMcuRestarts)

[Back to interface](#interface)
