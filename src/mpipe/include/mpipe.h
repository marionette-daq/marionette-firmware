#ifndef _MPIPE_H_
#define _MPIPE_H_

#include <stdbool.h>

extern mailbox_t mpipe_adc2_mb;
extern mailbox_t mpipe_adc3_mb;
extern mailbox_t mpipe_can_mb;

typedef struct {
  BaseSequentialStream  *chp;
} MPipeConfig;

#ifdef __cplusplus
extern "C" {
#endif

void mpipeInit(void);
void mpipeStart(const MPipeConfig * cfg);
void mpipeStop(void);

#ifdef __cplusplus
}
#endif

#endif
