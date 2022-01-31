<a id="library"></a>

# gbj\_appeeprom
This is an application library, which is used usually as a project library for particular PlatformIO project. It encapsulates the functionality of an `EEPROM` (Electrically Rrasable Programmable Read-Only Memory). The encapsulation provides following advantages:

* Functionality is hidden from the main sketch.
* The library follows the principle `separation of concern`.
* The library is reusable for various projects without need to code the EEPROM management.
* Update in library is valid for all involved projects.
* It specifies (inherits from) the parent application library `gbj_appcore`.
* It utilizes funcionality and error handling from the parent class.
* The library is not intended for direct instantiation, just as a parent class of project specific libraries (e.g., app\_params), because of hiding parameters definition as private variables and parameter definition structure as protected resource. This library as well as project specific libraries provide just setters and getters for individual parameters.


## Fundamental functionality
* Library works strictly with **byte application parameters**. If longer parameters are needed, e.g., integer ones, they have to be divided to two (or more) parameters for individual bytes (least and most significant ones). Then corresponding setter and getter should ensure calculation to desired parameter.
* All application parameters, generic as well as defined by derived child project libraries, are read from EEPROM to the cache in RAM only in their `begin` method within `setup` function of a sketch as an initial configuration at start of a microcontroller. Then only new values of parameters are written to EEPROM, but reading is always from the cache. EEPROM serves as the persistent parameters mirroring.


<a id="generics"></a>

## Generic application parameters
Library provides following generic application parameters that are utilized almost in every project.

#### mcuRestarts
Number of restarts of a microcontroller. It is utilized as the MCU restarts counter. The value `255` is reserved for the _wrong_ value (factory default for EEPROMs) and can be declared like `-1` as well.
* Minimum: 0
* Maximum: 254
* Default: 0

#### periodPublish
Time period in milliseconds for publishing telemetry data to an IoT platform.
* Minimum: 5
* Maximum: 30
* Default: 12


<a id="dependency"></a>

## Dependency

#### Arduino platform
* **Arduino.h**: Main include file for the Arduino SDK.
* **inttypes.h**: Integer type conversions. This header file includes the exact-width integer definitions and extends them with additional facilities provided by the implementation.

#### Espressif ESP8266 platform
* **Arduino.h**: Main include file for the Arduino platform.

#### Espressif ESP32 platform
* **Arduino.h**: Main include file for the Arduino platform.

#### Particle platform
* **Particle.h**: Includes alternative (C++) data type definitions.


<a id="constants"></a>

## Constants
* **VERSION**: Name and semantic version of the library.

Other constants, enumerations, result codes, and error codes are inherited from the parent library.


<a id="interface"></a>

## Custom data types
* [Handler](#handler)
* [**Parameter**](#parameter)

## Interface
* [gbj_appeeprom()](#gbj_appeeprom)
* [reset()](#reset)
* [**begin()**](#begin)

### Getters
* [getPrmStart()](#getPrmStart)
* [getPrmCount()](#getPrmCount)
* [getParameter()](#getParameter)

### Setters
* [**setParameter()**](#setParameter)
* [**storeParameter()**](#storeParameter)

### Methods for generic application parameters
* [setMcuRestarts()](#setMcuRestarts)
* [getMcuRestarts()](#getMcuRestarts)
* [setPeriodPublish()](#setPeriodPublish)
* [getPeriodPublish()](#getPeriodPublish)

> The methods and types in bold are protected and accessible only for derived child classes.


<a id="handler"></a>

## Handler

#### Description
The template or the signature of a callback function, which is called after every real storing a parameter value to the EEPROM. It can be utilized for signalling a parameter change as well.
* A handler is just a function with one input parameter and returning nothing.
* A handler can be declared just as `void` type.

#### Syntax
    typedef void Handler(byte)

#### Parameters
* Injected index of a saved parameter to a handler implementation callback function. This index is defined by the order of a parameter in definition array (list) of EEPROM parameters' pointers in a derived child class of the project specific library (e.g., app\_params).
  * *Valid values*: 0 ~ 255
  * *Default value*: none

#### Returns
None

#### See also
[gbj_appeeprom](#gbj_appeeprom)

[Back to interface](#interface)


<a id="parameter"></a>

## Parameter

#### Description
The structure with members and member methods as a template of an application parameter.
* An application parameter as an instance of this structure acts as a parameter cache for the EEPROM.
* Only changed parameter values are really written to the EEPROM.
* A parameter value is read only from the cache.
* The parameter cache is initialized by reading parameter value from EEPROM just at setup of an application.

#### Members
* byte **val**: Parameter value as a cache.
* byte **idx**: Sequence order of a parameter in an array of parameter pointers.
* bool **chg**: Flag about changing of the value. It is utilized for launching handler and written to the EEPROM.
* const byte **min**: Minimal valid value of a parameter. It is initialized at a parameter definition.
* const byte **max**: Maximal valid value of a parameter. It is initialized at a parameter definition.
* const byte **dft**: Default value of a parameter. It is initialized at a parameter definition. It is used when the provided value for storing to the EEPROM or read from it is outside valid range (min ~ max).

#### Member methods
* byte **get()**: Getter of cached parameter value. It is always constrained to valid range. If cached value is lower than minimum, this minimum is returned. If cached value is greater than maximum, this maximum is returned.
* byte **set(byte value)**: Setter of cache parameter value as an argument. Provided value is sanitized with valid range. If it is lower than minimum or greater than maximum, the default value is cached. If this sanitized value differs from already cached value, the change flag _chg_ is set. The setter returns cached value by calling its getter.
* byte **cycleUp()**: Increaser of cache parameter value. If cached value is equal to maximum, it returns minimum, else it returns increased cached value by 1.
* byte **cycleDown()**: Decreaser of cache parameter value. If cached value is equal to minimum, it returns maximum, else it returns decreased cached value by 1.
* bool **change()**: Getter of member _chg_. Suitable for using in project specific libraries without need to know name of the member.
* byte **index()**: Getter of member _idx_. Suitable for using in project specific libraries without need to know name of the member.

#### Example
This example presents the template for defining parameters. Generic ones in protected section of this library and application ones in private section of project specific libraries.
```cpp
Parameter mcuRestarts = { .min = 0, .max = 254, .dft = 0 };
Parameter periodPublish = { .min = 5, .max = 30, .dft = 12 };
```

#### See also
[Generic application parameters](#generics)

[Back to interface](#interface)


<a id="gbj_appeeprom"></a>

## gbj_appeeprom()

#### Description
Constructor creates the class instance object and initiates internal resources.

#### Syntax
    gbj_appeeprom(unsigned int prmStart, byte prmCount, Handler *onSave)

#### Parameters
* **prmStart**: The start position in EEPROM for saving application parameters.
The method adjusts the start position towards beginning of the EEPROM to
store all application parameters for corresponding EEPROM capacity of
related platform.
  * *Valid values*: non-negative integer
  * *Default value*: none, but usually 0 defined by a child class


* **prmCount**: The number of byte application parameters to process.
  * *Valid values*: 0 ~ 255
  * *Default value*: none


* **onSave**: Pointer to a callback function that receives one byte parameter as a parameter index and returns no value, which is called right after storing that parameter to the EEPROM.
  * *Valid values*: system address range
  * *Default value*: 0

#### Returns
Object enabling EEPROM management.

#### See also
[Handler](#handler)

[Back to interface](#interface)


<a id="begin"></a>

## begin()

#### Description
The initialization method, which should be located in the begin method of a derived class of a project specific library (e.g, app\_params) called in the setup section of a sketch.
- The method indexes parameters from provided list of parameter pointers.
- It reads values from the EEPROM for all provided parameters and caches them.

#### Syntax
    ResultCodes begin(Parameter **prmPointers)

#### Parameters
* **prmPointers**: Pointer to array of pointers with application parameters definitions.
  * *Valid values*: system address range
  * *Default value*: none

#### Returns
Some of [result or error codes](#constants) from the parent class.

[Back to interface](#interface)


<a id="reset"></a>

## reset()

#### Description
The method sets all application parameters to their default values. It is suitable for unknown or changed order of those parameters in the EEPROM, e.g., at using an already used EEPROM taken from another project. So, the method should be used once after aforementioned change.

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
The method returns starting position in the EEPROM for storing application parameters. This position is initiated and sanitized in the [constructor](#gbj_appeeprom).

#### Syntax
    unsigned int getPrmStart()

#### Parameters
None

#### Returns
The starting position in the EEPROM for storing application parameters.

#### See also
[getPrmCount()](#getPrmCount)

[Back to interface](#interface)


<a id="getPrmCount"></a>

## getPrmCount()

#### Description
The method returns number of application parameters that should be stored in the EEPROM. This number is initiated in the [constructor](#gbj_appeeprom).

#### Syntax
    byte getPrmCount()

#### Parameters
None

#### Returns
The number of byte application parameters in EEPROM.

#### See also
[getPrmStart()](#getPrmStart)

[Back to interface](#interface)


<a id="getParameter"></a>

## getParameter()

#### Description
The method returns value of a parameter from the cache, which is defined by a provided index of that parameter.

#### Syntax
    byte getParameter(byte idx)

#### Parameters
* **idx**: Index of a parameter in a definition array (list) of EEPROM parameters in a derived child class of a project specific library (e.g., app\_params).
  * *Valid values*: 0 ~ 255
  * *Default value*: none

#### Returns
The value of a parameter from EEPROM cache.

#### See also
[Parameter](#parameter)

[Back to interface](#interface)


<a id="setParameter"></a>

## setParameter()

#### Description
The overloaded method caches provided value of identified application parameter.
* A parameter is identified either by the pointer to its definition or its index.
* If provided value after sanitizing differs from already cached value, it is cached, its change flag is set, and the method calls method for storing parameter value to the EEPROM.

#### Syntax
    void setParameter(Parameter *prmPointer, byte value)
    void setParameter(byte idx, byte value)

#### Parameters
* **prmPointer**: Pointer of a parameter.
  * *Valid values*: system address range
  * *Default value*: none


* **idx**: Index of a parameter in a definition array (list) of EEPROM parameters in a derived child class of a project specific library (e.g., app\_params).
  * *Valid values*: 0 ~ 255
  * *Default value*: none


* **value**: Value of a parameter to be cached and potentially stored in the EEPROM.
  * *Valid values*: 0 ~ 255
  * *Default value*: none

#### Returns
None

#### See also
[Parameter](#parameter)

[storeParameter()](#storeParameter)

[getParameter()](#getParameter)

[Back to interface](#interface)


<a id="storeParameter"></a>

## storeParameter()

#### Description
The overloaded method writes cached value of identified application parameter to the EEPROM.
* A parameter is identified either by the pointer to its definition or its index.
* If cached value has changed by the setter and the handler is defined, it is written to the EEPROM alongside with calling a handler, if it is defined.

#### Syntax
    void storeParameter(Parameter *prmPointer)
    void storeParameter(byte idx)

#### Parameters
* **prmPointer**: Pointer of a parameter.
  * *Valid values*: system address range
  * *Default value*: none


* **idx**: Index of a parameter in a definition array (list) of EEPROM parameters in a derived child class of a project specific library (e.g., app\_params).
  * *Valid values*: 0 ~ 255
  * *Default value*: none

#### Returns
None

#### See also
[Parameter](#parameter)

[Handler](#handler)

[setParameter()](#setParameter)

[Back to interface](#interface)


<a id="setMcuRestarts"></a>

## setMcuRestarts()

#### Description
The method sanitizes and saves the provided number of the microcontroller restarts. It is usually utilized by the WiFi connection management of a project.
* Method sanitizes provided value by setting it to the default value, if it is outside of range from minimal to maximal value.
* Method stores the sanitized value in the parameter cache.
* Method really saves the sanitized value to the EEPROM only if it is new, i.e., it differs from value stored in the cache before.

#### Syntax
    void setMcuRestarts(byte value)

#### Parameters
* **value**: Number of MCU restarts. Value `255` is reserved as a _wrong_ parameter value, which is usually factory default value of EEPROMs. No provided value resets the restart counter.
  * *Valid values*: 0 ~ 254
  * *Default value*: 0

#### Returns
None

#### See also
[getMcuRestarts()](#getMcuRestarts)

[Back to interface](#interface)


<a id="getMcuRestarts"></a>

## getMcuRestarts()

#### Description
The method returns current number of the microcontroller restarts. It is usually utilized for initialization of a WiFi connection management.
* The value is taken from the parameter cache in RAM, not read from EEPROM directly.
* The value (alongside with other parameters) is read from EEPROM just at the beginning of a sketch at microcontroller start.

#### Syntax
    byte getMcuRestarts()

#### Parameters
None

#### Returns
Number of MCU restarts.

#### See also
[setMcuRestarts()](#setMcuRestarts)

[Back to interface](#interface)


<a id="setPeriodPublish"></a>

## setPeriodPublish()

#### Description
The method sanitizes and saves the provided time period in milliseconds for publishing, usually to an IoT platform.
* Method sanitizes provided value by setting it to the default value, if it is outside of range from minimal to maximal value.
* Method stores the sanitized value in the parameter cache.
* Method really saves the sanitized value to the EEPROM only if it is new, i.e., it differs from value stored in the cache before.

#### Syntax
    void setPeriodPublish(byte value)

#### Parameters
* **value**: Time period in milliseconds for publishing.
  * *Valid values*: 5 ~ 30
  * *Default value*: 12

#### Returns
None

#### See also
[getPeriodPublish()](#getPeriodPublish)

[Back to interface](#interface)


<a id="getPeriodPublish"></a>

## getPeriodPublish()

#### Description
The method returns current time period in milliseconds for publishing.
* The value is taken from the parameter cache in RAM, not read from EEPROM directly.
* The value (alongside with other parameters) is read from EEPROM just at the beginning of a sketch at microcontroller start.

#### Syntax
    byte getPeriodPublish()

#### Parameters
None

#### Returns
Time period for publishing in milliseconds.

#### See also
[setPeriodPublish()](#setPeriodPublish)

[Back to interface](#interface)
