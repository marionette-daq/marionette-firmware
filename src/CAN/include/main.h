
/*! \file main.h
 *
 */

#ifndef _MAIN_H
#define _MAIN_H
/*!
 * \addtogroup main Marionette main
 *
 * @{
 */
/*! \mainpage marionette Marionette
*
* <hr>
* \section intro Introduction
*
* Marionette is a configurable data aquisition device. Protocols supported
* include:
* 
* \verbatim
* GPIO
* ADC
* DAC
* SPI
* I2C
* \endverbatim
*
* <hr>
* \section filestructure File Tree
\verbatim

 tree -d -I ChibiOS-RT
.
├── Doc
│   └── doxydocs
│       └── html
│           └── search
├── hardware
│   ├── notes
│   ├── stdiscovery
│   └── waveshare
├── library
│   └── python
├── notes
├── requirements
│   └── QFD
├── src
│   ├── ARMCM4-STM32F407-DISCOVERY-ADC
│   ├── ARMCM4-STM32F407-DISCOVERY-MEMS
│   ├── boards
│   │   ├── ST_STM32F4_DISCOVERY
│   │   │   └── cfg
│   │   └── WAVESHARE_CORE407I
│   │       └── cfg
│   ├── devices
│   │   └── include
│   ├── fetch
│   │   └── include
│   ├── io
│   │   └── include
│   ├── marionette
│   │   ├── build
│   │   │   ├── lst
│   │   │   └── obj
│   │   ├── conf
│   │   └── include
│   ├── marionette-stdiscovery
│   │   ├── conf
│   │   └── include
│   ├── mshell
│   │   └── include
│   ├── usb
│   │   └── include
│   └── util
│       └── include
├── test
│   └── devtest
│       └── __pycache__
├── toolchain
│   └── openocd
└── windows_driver

\endverbatim
* <hr>
* \section requirements Requirements
* <hr>
* \section copyright Copyright Notes
* <hr>
* \section notes Notes
*
* arm-psas-eabi-gcc compiler
* ChibiOS source
* Waveshare Core407I board
* <hr>
*
*/

#endif

//! @}

