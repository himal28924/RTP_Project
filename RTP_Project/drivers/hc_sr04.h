#include <inttypes.h>
#include "stdbool.h"

void hc_sr04_init(void);

uint16_t hc_sr04_takeMeasurement(bool isFrontSensor);