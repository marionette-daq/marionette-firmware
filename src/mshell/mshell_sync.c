/*!
 * \file    mshell_sync.c
 *
 *  Synchronization support for the mshell
 *
 * @defgroup mshell_sync Marionette Shell Synchronization
 * @{
 */


#include <string.h>

#include "ch.h"
#include "hal.h"
#include "chprintf.h"
#include "chbsem.h"

#include "mshell_sync.h"

binary_semaphore_t mshell_sync_sem;

/*! \brief Initialize the Binary Semaphore for the Terminal */
void mshell_sync_init() {
	chBSemObjectInit(&mshell_sync_sem, 0);
}

void mshell_sync_acquire()
{
  chBSemWait( &mshell_sync_sem );
}

void mshell_sync_release()
{
	chBSemSignal( &mshell_sync_sem );
}

/** @} */

