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

void mcardInit(void);
thread_t *mcardCreateInThread(BaseSequentialStream * chp, size_t size, tprio_t prio);
thread_t *mcardCreateInThreadStatic(BaseSequentialStream *chp, void *wsp, size_t size, tprio_t prio);
thread_t *mcardCreateOutThread(BaseSequentialStream * chp, size_t size, tprio_t prio);
thread_t *mcardCreateOutThreadStatic(BaseSequentialStream *chp, void *wsp, size_t size, tprio_t prio);

#ifdef __cplusplus
}
#endif

#endif
