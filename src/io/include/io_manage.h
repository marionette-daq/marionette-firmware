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

	bool io_manage_query_fn_avail(ioportid_t port, uint32_t pad, IO_alloc request_alloc);
	bool io_manage_set_default_mode(ioportid_t port, uint32_t pad);
	bool io_manage_set_mode(ioportid_t port, uint32_t pad, iomode_t new_mode, IO_alloc request_alloc);
	void io_manage_table_to_defaults(void);
	void io_manage_query_pin(BaseSequentialStream * chp, ioportid_t port, uint32_t pad) ;

#ifdef __cplusplus
}

#endif

#endif

//! @}

