/*!
 * \file    mshell_sync.h
 * \brief   Semaphore and synchronization for the CLI shell header.
 *
 * \addtogroup mshell_sync
 * @{
 */
#ifndef _MSHELL_SYNC_H_
#define _MSHELL_SYNC_H_

#ifdef __cplusplus
extern "C" {
#endif

#include "chbsem.h"

extern 		binary_semaphore_t    		mshell_io_sem;

void mshell_io_sem_init(void) ;

#ifdef __cplusplus
}
#endif

#endif

/** @} */
