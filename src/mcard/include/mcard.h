#ifndef __MCARD_H
#define __MCARD_H

#include <stdbool.h>

#ifdef __cplusplus
extern "C" {
#endif

extern mailbox_t mcard_adc2_mb;
extern mailbox_t mcard_adc3_mb;
extern mailbox_t mcard_can_mb;
extern mailbox_t mcard_event_mb;

void mcard_init(void);
void mcard_start(void);
void mcard_stop(void);

#ifdef __cplusplus
}
#endif

#endif
