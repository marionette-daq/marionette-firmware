/*! \file util_version.h
 */

#ifndef UTIL_VERSION_H_
#define UTIL_VERSION_H_

#include <stdint.h>

#define         MAX_FW_VERSION_LENGTH              50

typedef struct HW_Version {
	uint32_t id_high; 
	uint32_t id_center; 
	uint32_t id_low; 
}HW_Version;

typedef struct VERSIONData {
	char         firmware[MAX_FW_VERSION_LENGTH];
	HW_Version   hardware;
} VERSIONData;

void util_fwversion(struct VERSIONData * ver_data);
void util_hwversion(struct VERSIONData * ver_data);

#endif

