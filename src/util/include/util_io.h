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

typedef struct port_pin
{
  ioportid_t  port;
  uint32_t    pin;
} port_pin_t;

typedef struct alt_pin_mode
{
  const ioportid_t  port;
  const uint32_t    pin;
  const uint32_t    mode;
} alt_pin_mode_t;


void set_status_led(bool r, bool g, bool b);

bool set_alternate_mode( ioportid_t port, uint32_t pin );

bool set_alternate_mode_ext( ioportid_t port, uint32_t pin, uint32_t pupdr, uint32_t otype, uint32_t ospeed);

bool reset_alternate_mode( ioportid_t port, uint32_t pin );

#ifdef __cplusplus
}

#endif

#endif

//! @}

