/*! \file util_version.c
 * Create facility to query current version of firmware
 */

#include <string.h>

#include "util_version.h"

/* GIT_COMMIT_VERSION is inserted by the build system, generated in
 * common/marionette.mk
 */
void version(struct VERSIONData * ver_data)
{
#ifndef GIT_COMMIT_VERSION
#define GIT_COMMIT_VERSION "Unknown"
#endif
	strncpy(ver_data->byte, GIT_COMMIT_VERSION, sizeof(MAX_VERSION_LENGTH));
}

