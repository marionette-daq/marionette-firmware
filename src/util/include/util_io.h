/*! \file io_manaage.h
 *
 * @addtogroup io_manaage
 * @{
 */

#ifndef UTIL_IO_H
#define UTIL_IO_H

#ifdef __cplusplus
extern "C" {
#endif

#include "ch.h"
#include "hal.h"

#ifdef __cplusplus
extern "C" {
#endif

#define INVALID_PIN 99

typedef struct str_pin_map
{
	const char      * string;
	const uint32_t    pin;
} str_pin_map_t;

typedef struct str_port_map
{
	const char      * string;
	const ioportid_t  port;
} str_port_map_t;

typedef struct port_pin
{
  ioportid_t  port;
  uint32_t    pin;
} port_pin_t;

typedef struct str_port_pin_map
{
  const char * string;
  port_pin_t port_pin;
} str_port_pin_map_t;

typedef struct alt_pin_mode
{
  const ioportid_t  port;
  const uint32_t    pin;
  const uint32_t    mode;
  const bool        state;
} alt_pin_mode_t;

void set_status_led(bool r, bool g, bool b);

#ifdef __cplusplus
}

#endif

#endif

//! @}

