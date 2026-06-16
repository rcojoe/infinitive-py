#ifndef INFINITIVE_CONVERSIONS_H
#define INFINITIVE_CONVERSIONS_H

#include <Arduino.h>

// Mode conversions
const char* rawModeToString(uint8_t mode);
uint8_t stringModeToRaw(const char* mode);

// Fan mode conversions
const char* rawFanModeToString(uint8_t mode);
uint8_t stringFanModeToRaw(const char* mode);

#endif
