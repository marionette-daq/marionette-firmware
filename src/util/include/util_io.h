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

#define MAX_PIN_STRLEN  8
#define MAX_PORT_STRLEN 8

typedef enum io_pin {
	PIN0 = 0,
	PIN1,
	PIN2,
	PIN3,
	PIN4,
	PIN5,
	PIN6,
	PIN7,
	PIN8,
	PIN9,
	PIN10,
	PIN11,
	PIN12,
	PIN13,
	PIN14,
	PIN15,
	INVALID_PIN=99
} iopin_t;

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
  const ioportid_t  port;
  const uint32_t    pin;
} port_pin_t;


ioportid_t string_to_port( char * str);
iopin_t string_to_pin( char * str);
const char * port_to_string( ioportid_t port );

#ifdef __cplusplus
}

#endif

#endif

//! @}

