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

bool io_manage_query_fn_avail( ioportid_t port, uint32_t pin, io_alloc_t request_alloc );
bool io_manage_set_default_mode( ioportid_t port, uint32_t pin );
bool io_manage_set_mode( ioportid_t port, uint32_t pin, iomode_t new_mode, io_alloc_t request_alloc );
void io_manage_table_to_defaults( void );
io_alloc_t io_manage_query_current_fn( ioportid_t port, uint32_t pin );
const char * io_manage_query_pin_current_fn_name( ioportid_t port, uint32_t pin );

#ifdef __cplusplus
}

#endif

#endif

//! @}

