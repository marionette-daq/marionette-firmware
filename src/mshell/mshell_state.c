/*!
 * \file    mshell_state.c
 *
 * @defgroup mshell_state Marionette Shell State 
 * @{
 */
#include <string.h>
#include "ch.h"
#include "hal.h"
#include "chprintf.h"
#include "chbsem.h"

#include "mshell_sync.h"
#include "mshell_state.h"

Mshell_status                   mshell_state;

bool mshell_stream_put(BaseSequentialStream * chp, uint8_t c) {
	int ret;
	chBSemWait( &mshell_io_sem );
    ret = chSequentialStreamPut(chp, c) ;
	chBSemSignal( &mshell_io_sem );
	return ret;
}	

void mshell_stream_putstr(BaseSequentialStream * chp, char * fmt, ...) {
	va_list argList;
	va_start(argList, fmt);

	chBSemWait( &mshell_io_sem );
	chvprintf(chp, fmt, argList);
	chBSemSignal( &mshell_io_sem );
	va_end(argList);
}

bool getMShellVisiblePrompt() {
		return(mshell_state.visibleprompt);
}

BaseSequentialStream * getMShellStreamPtr() {
		return(mshell_state.chp);
}


void setMShellStreamPtr(BaseSequentialStream * ptr) {
		mshell_state.chp = ptr;
}

void setMShellPrompt(char * s) {
	strncpy(mshell_state.prompt, s, MSHELL_MAX_PROMPT_LENGTH);
  mshell_state.prompt[MSHELL_MAX_PROMPT_LENGTH-1] = '\0';
}

void mshell_putprompt() {
	mshell_stream_putstr(getMShellStreamPtr(), "%s", mshell_state.prompt);
}

void mshell_putnewline() {
	mshell_stream_putstr(getMShellStreamPtr(), "\r\n");
}

void setMShellVisiblePrompt(bool b) {
	mshell_state.visibleprompt = b;
};

/** @} */

