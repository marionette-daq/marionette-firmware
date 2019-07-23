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

extern binary_semaphore_t mshell_sync_sem;

void mshell_sync_init(void);
void mshell_sync_acquire(void);
void mshell_sync_release(void);

#ifdef __cplusplus
}
#endif

#endif

/** @} */
