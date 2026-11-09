#ifndef LED_STATUS_H
#define LED_STATUS_H

#include "app_state.h"

void led_status_init(void);

void led_status_update(alarma_estado_t state);

#endif