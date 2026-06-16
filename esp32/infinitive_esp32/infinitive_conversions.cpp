#include "infinitive_conversions.h"

const char* rawModeToString(uint8_t mode) {
  switch(mode) {
    case 0: return "heat";
    case 1: return "cool";
    case 2: return "auto";
    case 3: return "electric";
    case 4: return "heatpump";
    case 5: return "off";
    default: return "unknown";
  }
}

uint8_t stringModeToRaw(const char* mode) {
  if (strcmp(mode, "heat") == 0) return 0;
  if (strcmp(mode, "cool") == 0) return 1;
  if (strcmp(mode, "auto") == 0) return 2;
  if (strcmp(mode, "electric") == 0) return 3;
  if (strcmp(mode, "heatpump") == 0) return 4;
  if (strcmp(mode, "off") == 0) return 5;
  return 5;  // Default to off
}

const char* rawFanModeToString(uint8_t mode) {
  switch(mode) {
    case 0: return "auto";
    case 1: return "low";
    case 2: return "med";
    case 3: return "high";
    default: return "unknown";
  }
}

uint8_t stringFanModeToRaw(const char* mode) {
  if (strcmp(mode, "auto") == 0) return 0;
  if (strcmp(mode, "low") == 0) return 1;
  if (strcmp(mode, "med") == 0) return 2;
  if (strcmp(mode, "high") == 0) return 3;
  return 0;  // Default to auto
}
