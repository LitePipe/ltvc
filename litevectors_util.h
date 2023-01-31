#ifndef _LITEVECTORS_UTIL_H
#define _LITEVECTORS_UTIL_H

#include "litevectors.h"

////////////////////////////////////////////////////////////////////////////////
// LiteVectors Utilities
//
// These pieces can be useful when working with LiteVectors in a C codebase,
// but they are not used by the encoder or decoder, and are not required to
// use LiteVectors.
////////////////////////////////////////////////////////////////////////////////

// Get a text string associated with an error code from ltv_next.
const char* ltv_status_text(int status_code); 

// Tests whether ltv_data_t element is pointing to a given string.
bool is_string_eq(const ltv_data_t *v, const char *value);

// Test whether an element is a standalone unsigned integer.
bool is_uint(ltv_data_t *v);

// Test whether an element is a standalone signed integer.
bool is_int(ltv_data_t *v);

// Test whether an element is a 32-bit floating point number.
bool is_float(ltv_data_t *v);

// Test whether an element is a 64-bit floating point number.
bool is_double(ltv_data_t *v);

// Test whether an element is an integer within the given bounds.
bool is_int_bound(ltv_data_t *v, int64_t min, int64_t max);

// Test whether an element is an integer within the given bounds.
bool is_uint_bound(ltv_data_t *v, uint64_t min, uint64_t max);

// A static buffer is statically allocated buffer for holding serialized data.
// The size of the backing 'data' element can be changed to accommodate the 
// anticipated maximum data size to be serialized.
typedef struct  {
    uint8_t data[8192];
    size_t size;
} static_buffer_t;

// A function implementing the ltv_writer interface. This is used for a
// LiteVector serializer to write to a static_buffer.
int static_buffer_writer(const uint8_t *buf, size_t len, void* user_data);

#endif //_LITEVECTORS_UTIL_H