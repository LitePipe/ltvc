#ifndef _LTV_JSON_H
#define _LTV_JSON_H

#include "litevectors.h"

// Print a LiteVector, returns the number of bytes consumed.
int64_t print_ltv(const uint8_t *buf, size_t bufSize);

// Print a LiteVector file
void print_ltv_file(const char* filename);

#endif