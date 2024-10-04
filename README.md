<a id="library"></a>

# gbj\_appeeprom
This is an application library, which is used usually as a project library for particular PlatformIO project. It encapsulates the functionality of an <abbr title='Electrically Erasable Programmable Read-Only Memory'>EEPROM</abbr>. The encapsulation provides following advantages:

* Functionality is hidden from the main sketch.
* The library follows the principle `separation of concerns`.
* The library is reusable for various projects without need to code the EEPROM management.
* Update in library is valid for all involved projects.
* It specifies (inherits from) the parent application library `gbj_appcore`.
* It utilizes funcionality and error handling from the parent class.
* The library is not intended for direct instantiation, just as a parent class of project specific libraries, e.g., `app_eeprom`.


## Fundamental functionality
* Library works strictly with **byte application parameters only**. If longer parameters are needed, e.g., integer ones, they have to be divided to two (or more) parameters for individual bytes (least and most significant ones).
* All application parameters are read from EEPROM to the cache in RAM only in their `begin` method within `setup` function of a sketch as an initial configuration at start of a microcontroller. Then only new values of parameters are written to EEPROM, but reading is always from the cache. EEPROM serves as the persistent parameters mirroring.
* The saving changed parameters values are delayed by a predefined or set time interval in order not to prevent useless writing to the EEPROM in quick tempo, e.g., by cycling a parameter's value. It should be realized by the method [run()](#run).
* All parameters are defined in a project specific library and passed to this application parent class as a vector of pointers to those parameters.


<a id="internals"></a>

## Internal parameters
Internal parameters are hard-coded in the library as enumerations and none of them have setters or getters associated.

* **Default delay store period** (`3 seconds`): It is a time period since recent change of a parameter's value, after which that change is saved to the EEPROM.


<a id="dependency"></a>

## Dependency
* **gbj\_appcore**: Parent library for all application libraries loaded from the file `gbj_appcore.h`.
* **gbj\_serial\_debug**: Auxilliary library for debug serial output loaded from the file `gbj_serial_debug.h`. It enables to exclude serial outputs from final compilation.
* **EEPROM**: System library for EEPROM management loaded from the file `EEPROM.h`.

#### Arduino platform
* **Arduino.h**: Main include file for the Arduino SDK.
* **inttypes.h**: Integer type conversions. This header file includes the exact-width integer definitions and extends them with additional facilities provided by the implementation.

#### Espressif ESP8266 platform
* **Arduino.h**: Main include file for the Arduino platform.

#### Espressif ESP32 platform
* **Arduino.h**: Main include file for the Arduino platform.

#### Particle platform
* **Particle.h**: Includes alternative (C++) data type definitions.


<a id="interface"></a>

## Custom data types
* [Parameter](#parameter)

## Interface
* [gbj_appeeprom()](#gbj_appeeprom)
* [begin()](#begin)
* [reset()](#reset)
* [run()](#run)

### Getters and Setters
* [getPrmStart()](#getPrmStart)
* [getPrmCount()](#getPrmCount)
* [getPeriod()](#getPeriod)
* [setPeriod()](#setPeriod)


<a id="parameter"></a>

## Parameter

#### Description
The structure with members and member methods as a template of an application parameter.
* An application parameter as an instance of this structure acts as a parameter cache for the EEPROM.
* Only changed parameter values are really written to the EEPROM.
* A parameter value is normally read from the cache only.
* The parameter cache is initialized by reading parameter value from EEPROM just at setup of an application.

#### Members
* **byte val**: Parameter value as a cache.
* **unsigned long tsSet**: Elapsed time in milliseconds from the boot, at wich was a parameter's value change recently.
* **unsigned int mem**: Byte position of a parameter in the EEPROM.
* **const byte min**: Minimal valid value of a parameter. It is initialized at a parameter definition.
* **const byte max**: Maximal valid value of a parameter. It is initialized at a parameter definition.
* **const byte dft**: Default value of a parameter. It is initialized at a parameter definition. It is used when the provided value for storing to the EEPROM or read from it is outside valid range (min ~ max).

#### Member methods
* **byte get()**: Getter of cached parameter value. It is always constrained to valid range. If cached value is lower than minimum, this minimum is returned. If cached value is greater than maximum, this maximum is returned.
* **byte set(byte value)**: Setter of cache parameter value as an argument. Provided value is sanitized with valid range. If it is lower than minimum or greater than maximum, the default value is cached. If this sanitized value differs from already cached value, the new value is written to the EEPROM. The setter returns cached value by calling its getter.
* **byte cycleUp()**: Increaser of cache parameter value. If cached value is equal to maximum, it returns minimum, else it returns increased cached value by 1.
* **byte cycleDown()**: Decreaser of cache parameter value. If cached value is equal to minimum, it returns maximum, else it returns decreased cached value by 1.
* **void save()**: Method physically stores the cached value to the EEPROM.

#### Example
This example presents the template for defining parameters in a project specific library.
```cpp
Parameter periodPublish = { .min = 5, .max = 30, .dft = 12 };
```

[Back to interface](#interface)


<a id="gbj_appeeprom"></a>

## gbj_appeeprom()

#### Description
Constructor creates the class instance object and initiates internal resources.

#### Syntax
    gbj_appeeprom(unsigned int prmStart)

#### Parameters
* **prmStart**: The start position in EEPROM for saving application parameters.
The method adjusts the start position towards beginning of the EEPROM to
store all application parameters for corresponding EEPROM capacity of
related platform.
  * *Valid values*: non-negative integer
  * *Default value*: none, but usually 0 defined by a child project specific class

#### Returns
Object enabling EEPROM management.

[Back to interface](#interface)


<a id="begin"></a>

## begin()

#### Description
The initialization method, which should be located in the begin method of a derived class of a project specific library called in the setup section of a sketch.
- The method indexes parameters from provided vector of parameter pointers.
- It reads values from the EEPROM for all provided parameters and caches them.

#### Syntax
    ResultCodes begin(const std::vector<Parameter *>& prmPointers)

#### Parameters
* **prmPointers**: Pointer to a vector of pointers with application parameters definitions.
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


<a id="run"></a>

## run()

#### Description
The method save parameters' values that have been changed not sooner than current delayed store period until it is not forced immediately.
* The method should be called frequently, usually in the loop function of a main sketch.

#### Syntax
    void run(bool flForce)

#### Parameters
* **flForce**: Flag about immediate saving to EEPROM. It is needed right before MCU restart.
  * *Valid values*: true, false
  * *Default value*: false


#### Returns
None

#### See also
[setPeriod()](#setPeriod)

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

[Back to interface](#interface)


<a id="getPrmCount"></a>

## getPrmCount()

#### Description
The method returns number of application parameters that should be stored in the EEPROM. This number is derived from a vector of parameters..

#### Syntax
    byte getPrmCount()

#### Parameters
None

#### Returns
The number of byte application parameters in EEPROM.

#### See also
[begin()](#begin)

[Back to interface](#interface)


<a id="getPeriod"></a>

## getPeriod()

#### Description
The method returns current delayed store period.

#### Syntax
    unsigned long getPeriod()

#### Parameters
None

#### Returns
Current delayed store period in milliseconds.

#### See also
[setPeriod()](#setPeriod)

[Back to interface](#interface)


<a id="setPeriod"></a>

## setPeriod()

#### Description
The overloaded method sets a new delayed store period in milliseconds or seconds.
* The method with numerical input argument is aimed for input in milliseconds.
* The method with textual input argument is aimed for input in seconds. It is useful with conjunction with a project data hub, which data has always string data type.
* No input argument sets the [internal default period](#internals), which is setter's default value.


#### Syntax
    void setPeriod(unsigned long period)
    void setPeriod(String periodSec)

#### Parameters
* **period**: Duration of the waiting period in milliseconds.
  * *Valid values*: 0 ~ 2^32 - 1
  * *Default value*: 3000


* **periodSec**: Duration of the waiting period in seconds declared as string.
  * *Valid values*: String
  * *Default value*: none

#### Returns
None

#### See also
[getPeriod()](#getPeriod)

[Back to interface](#interface)
