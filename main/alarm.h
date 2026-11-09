#ifndef ALARM_H
#define ALARM_H

#include "app_state.h"

const char* alarm_state_to_string(alarma_estado_t state);

void alarm_init(void);

void alarm_toggle(void);

void alarm_trigger(void);

alarma_estado_t alarm_get_state(void);

#endif