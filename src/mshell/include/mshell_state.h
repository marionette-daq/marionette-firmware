/*!
 * \file    mshell_state.h
 * \brief   Track the state of the mshell
 *
 * \addtogroup mshell_state
 * @{
 */
#ifndef MSHELL_STATE_H_
#define MSHELL_STATE_H_

#ifdef __cplusplus
extern "C" {
#endif

#include <stdbool.h>

#include "ch.h"
#include "hal.h"
#include "ch.h"
#include "hal.h"
#include "chprintf.h"
#include "chbsem.h"

#include "mshell_sync.h"

#if !defined(MSHELL_MAX_PROMPT_LENGTH) || defined(__DOXYGEN__)
#define MSHELL_MAX_PROMPT_LENGTH       10
#endif

typedef struct mshell_status
{
	BaseSequentialStream    *   chp;
	char                        prompt[MSHELL_MAX_PROMPT_LENGTH];
	bool                        visibleprompt;
} Mshell_status;

bool getMShellVisiblePrompt(void) ;
BaseSequentialStream * getMShellStreamPtr(void) ;

void setMShellStreamPtr(BaseSequentialStream * ptr) ;
void mshell_putprompt(void) ;
void setMShellPrompt(char * s) ;
bool mshell_stream_put(BaseSequentialStream * chp, uint8_t c) ;
void mshell_stream_putstr(BaseSequentialStream * chp, char * fmt, ...) ;
void setMShellVisiblePrompt(bool b) ;
void mshell_putnewline(void) ;

#ifdef __cplusplus
}
#endif

#endif

/** @} */

