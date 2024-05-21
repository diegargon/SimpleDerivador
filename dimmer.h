#ifndef DIMMER_H
#define DIMMER_H

#include <Arduino.h>
#include <dimmable_light_linearized.h>
#include "simple_derivador.h"

void init_dimmer(void);
void setDimmerPower(int derivation);

#endif
