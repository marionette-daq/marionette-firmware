
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

.
├── ChibiOS-RT    // Submodule of ChibiOS Operating System
│   ├── boards
│   ├── demos
│   ├── docs
│   ├── ext
│   ├── os
│   ├── test
│   └── testhal
├── Doc           // Documentation for doxygen and other miscellaneous documentation
│   └── doxydocs
├── hardware      // Information about the hardware
│   ├── notes
│   ├── stdiscovery
│   └── waveshare
├── library      // User routines and scripts
│   └── python
├── notes        // development notes
├── requirements // Requirements capture and tracking
│   └── QFD
├── src          // Firmware source subtree
│   ├── ARMCM4-STM32F407-DISCOVERY-ADC
│   ├── ARMCM4-STM32F407-DISCOVERY-MEMS
│   ├── boards
│   ├── devices
│   ├── fetch
│   ├── marionette
│   ├── marionette-stdiscovery
│   ├── mshell
│   ├── usb
│   └── util
├── test        // Testing and demonstration scripts
│   └── devtest
├── toolchain   // Toolchain support for build and flash programming
│   └── openocd
└── windows_driver  // APDM signed windows driver

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

