/*! \file util_version.h
 */

#ifndef UTIL_VERSION_H_
#define UTIL_VERSION_H_

#define         MAX_VERSION_LENGTH              50

typedef struct VERSIONData
{
	char byte[MAX_VERSION_LENGTH];
} VERSIONData;

void util_version(struct VERSIONData * ver_data);

#endif

