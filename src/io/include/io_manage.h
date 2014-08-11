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

#include "adc_lld.h"

#include "io_manage_defs.h"

#include "board.h"

#ifdef __cplusplus
extern "C" {
#endif





		// query state
//io_manage_query(port, pad);
		// state accounting 
//bool io_manage_set(port, pad, mode, alloc);
		// do palSetMode here
		// see gpio
		// reset to defaults
//void io_manage_reset(port, pad);

#ifdef __cplusplus
}
#endif

#endif


//! @}


