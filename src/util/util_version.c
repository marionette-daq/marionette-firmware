/*! \file util_version.c
 * Create facility to query current version of firmware
 */

#include <string.h>

#include "util_version.h"

/* GIT_COMMIT_VERSION is inserted by the build system, generated in
 * common/marionette.mk
 */
void util_fwversion(struct VERSIONData * ver_data)
{
#ifndef GIT_COMMIT_VERSION
#define GIT_COMMIT_VERSION "Unknown"
#endif
	strncpy(ver_data->firmware, GIT_COMMIT_VERSION, MAX_FW_VERSION_LENGTH);
}

/* 
 * HW Version
 * 4 32 bit registers: base address: 0x1FFF 7A10
 * ST Ref. STM32f40x Page 1399 section 34 'Device Electronic Signature'
 */
void util_hwversion(struct VERSIONData * ver_data)
{
		ver_data->hardware.id_low = 0x1fff7a10;

}

