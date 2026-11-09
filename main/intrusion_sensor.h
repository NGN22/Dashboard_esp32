#ifndef INTRUSION_SENSOR_H
#define INTRUSION_SENSOR_H

#include <stdbool.h>

void intrusion_sensor_init(void);

bool intrusion_sensor_is_triggered(void);

#endif