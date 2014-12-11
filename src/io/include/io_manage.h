/*! \file io_manaage.h
 *
 * @addtogroup io_manaage
 * @{
 */

#ifndef IO_MANAGE_H
#define IO_MANAGE_H

#ifdef __cplusplus
extern "C" {
#endif

#include "ch.h"
#include "hal.h"

#include "io_manage_defs.h"

#ifdef __cplusplus
extern "C" {
#endif

ioportid_t string_to_port( char * str);
iopin_t string_to_pin( char * str);
const char * port_to_string( ioportid_t port );

void io_manage_set_all_to_defaults( void );
bool io_manage_set_default_mode(ioportid_t port, uint32_t pin, iomode_t mode_filter );
bool io_manage_set_mode( ioportid_t port, uint32_t pin, iomode_t new_mode, io_alloc_t request_alloc );

const char * io_manage_get_alloc_name(io_alloc_t alloc);
iomode_t   io_manage_get_current_mode( ioportid_t port, uint32_t pin );
io_alloc_t io_manage_get_current_alloc( ioportid_t port, uint32_t pin );
uint32_t   io_manage_get_available_alloc( ioportid_t port, uint32_t pin );

#ifdef __cplusplus
}

#endif

#endif

//! @}

