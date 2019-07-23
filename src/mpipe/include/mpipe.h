#ifndef _MPIPE_H_
#define _MPIPE_H_

#include <stdbool.h>

extern mailbox_t mpipe_adc2_mb;
extern mailbox_t mpipe_adc3_mb;
extern mailbox_t mpipe_can_mb;

typedef struct {
  BaseAsynchronousChannel * channel;
} mpipe_config_t;

#ifdef __cplusplus
extern "C" {
#endif

void mpipe_init(void);
void mpipe_start(const mpipe_config_t * cfg);
void mpipe_stop(void);

#ifdef __cplusplus
}
#endif

#endif
