#include <stdint.h>
#include <string.h>
#include <stdio.h>

#include "litevectors.h"

// Assertion: LITTLE ENDIAN ARCHITECTURE

// Size assertions for the unfixed types we're using
_Static_assert(sizeof(uint8_t) == 1, "unexpected uint8_t datatype size");
_Static_assert(sizeof(bool) == 1, "unexpected bool datatype size");
_Static_assert(sizeof(float) == 4, "unexpected float datatype size");
_Static_assert(sizeof(double) == 8, "unexpected double datatype size");

// Element lengths in bytes
static const int ltv_type_sizes[] = {0, 0, 0, 0, 1, 1, 1, 2, 4, 8, 1, 2, 4, 8, 4, 8};
                                    
////////////////////////////////////////////////////////////////////////////////
// Encoder
////////////////////////////////////////////////////////////////////////////////

void ltv_encoder_init(ltv_encoder_t *e, ltv_writer writer, void *user_data) {
    e->writer = writer;
    e->user_data = user_data;
    e->offset = 0;
    e->status = 0;
}

void ltv_write(ltv_encoder_t *e, const uint8_t *buf, size_t count) {
    if (e->status != 0) {
        return;
    }

    e->offset += count;
    e->status = e->writer(buf, count, e->user_data);
}

void ltv_write_byte(ltv_encoder_t *e, uint8_t value) {
    ltv_write(e, &value, 1);
}

void ltv_write_nop(ltv_encoder_t *e) {
    ltv_write_byte(e, LTV_NOP_TAG);
}

void ltv_write_tag(ltv_encoder_t *e, uint8_t type_code, uint8_t size_code) {
    ltv_write_byte(e, (type_code << 4) | size_code); 
}

void ltv_nil(ltv_encoder_t *e) {
    ltv_write_tag(e, LTV_NIL, LTV_SINGLE);
}

void ltv_bool(ltv_encoder_t *e, bool value) {
    ltv_write_tag(e, LTV_BOOL, LTV_SINGLE);
    ltv_write_byte(e, value);
}

void ltv_i8(ltv_encoder_t *e, int8_t val) {
    ltv_write_tag(e, LTV_I8, LTV_SINGLE);
    ltv_write(e, (const uint8_t*) &val, sizeof(int8_t));
}

void ltv_i16(ltv_encoder_t *e, int16_t val) {
    ltv_write_tag(e, LTV_I16, LTV_SINGLE);
    ltv_write(e, (const uint8_t*) &val, sizeof(int16_t));
}

void ltv_i32(ltv_encoder_t *e, int32_t val) {
    ltv_write_tag(e, LTV_I32, LTV_SINGLE);
    ltv_write(e, (const uint8_t*) &val, sizeof(int32_t));
}

void ltv_i64(ltv_encoder_t *e, int64_t val) {
    ltv_write_tag(e, LTV_I64, LTV_SINGLE);
    ltv_write(e, (const uint8_t*) &val, sizeof(int64_t));
}

void ltv_u8(ltv_encoder_t *e, uint8_t val) {
    ltv_write_tag(e, LTV_U8, LTV_SINGLE);
    ltv_write(e, (const uint8_t*) &val, sizeof(uint8_t));
}

void ltv_u16(ltv_encoder_t *e, uint16_t val) {
    ltv_write_tag(e, LTV_U16, LTV_SINGLE);
    ltv_write(e, (const uint8_t*) &val, sizeof(uint16_t));
}

void ltv_u32(ltv_encoder_t *e, uint32_t val) {
    ltv_write_tag(e, LTV_U32, LTV_SINGLE);
    ltv_write(e, (const uint8_t*) &val, sizeof(uint32_t));
}

void ltv_u64(ltv_encoder_t *e, uint64_t val) {
    ltv_write_tag(e, LTV_U64, LTV_SINGLE);
    ltv_write(e, (const uint8_t*) &val, sizeof(uint64_t));
}

void ltv_f32(ltv_encoder_t *e, float val) {
    ltv_write_tag(e, LTV_F32, LTV_SINGLE);
    ltv_write(e, (const uint8_t*) &val, sizeof(float));
}

void ltv_f64(ltv_encoder_t *e, double val) {
    ltv_write_tag(e, LTV_F64, LTV_SINGLE);
    ltv_write(e, (const uint8_t*) &val, sizeof(double));
}

void ltv_struct_start(ltv_encoder_t *e) {
    ltv_write_tag(e, LTV_STRUCT, LTV_SINGLE);
}

void ltv_struct_end(ltv_encoder_t *e) {
    ltv_write_tag(e, LTV_END, LTV_SINGLE);
}

void ltv_list_start(ltv_encoder_t *e) {
    ltv_write_tag(e, LTV_LIST, LTV_SINGLE);
}

void ltv_list_end(ltv_encoder_t *e) {
    ltv_write_tag(e, LTV_END, LTV_SINGLE);
}

void ltv_write_vector(ltv_encoder_t *e, uint8_t type_code, const uint8_t* buf, size_t count) {

    int typeSize = ltv_type_sizes[type_code];
    size_t len = count * typeSize;

    // Compute the number of bytes 2^n needed to store len
    int exp = 3;
    if(len < INT8_MAX) {
        exp = 0;
    } else if (len < INT16_MAX) {
        exp = 1;
    } else if (len < INT32_MAX) {
        exp = 2;
    } 

    int size_code = LTV_SIZE_1 + exp;
    size_t lenSize = 1 << exp;

    // Insert NOPs to align arrays to type size alignment.
#ifdef LTV_VECTOR_ALIGNMENT
    int alignmentDelta = (e->offset + 1 + lenSize) & (typeSize - 1);
    if (alignmentDelta != 0) {
        int paddingLen = typeSize - alignmentDelta;
        for(int i=0; i < paddingLen; i++) {
            ltv_write_nop(e);
        }
    }
#endif

    ltv_write_tag(e, type_code, size_code);
    ltv_write(e, (const uint8_t*) &len, lenSize);
    ltv_write(e, buf, len);
}

void ltv_string(ltv_encoder_t *e, const char* val) {
    size_t len = strlen(val);
    ltv_write_vector(e, LTV_STRING, (const uint8_t*) val, len);
}

void ltv_bool_vec(ltv_encoder_t *e, bool *val, size_t count) {
    ltv_write_vector(e, LTV_BOOL, (const uint8_t*) val, count);
}

void ltv_i8_vec(ltv_encoder_t *e, int8_t *val, size_t count) {
    ltv_write_vector(e, LTV_I8, (const uint8_t*) val, count);
}

void ltv_i16_vec(ltv_encoder_t *e, int16_t *val, size_t count) {
    ltv_write_vector(e, LTV_I16, (const uint8_t*) val, count );
}

void ltv_i32_vec(ltv_encoder_t *e, int32_t *val, size_t count) {
    ltv_write_vector(e, LTV_I32, (const uint8_t*) val, count);
}

void ltv_i64_vec(ltv_encoder_t *e, int64_t *val, size_t count) {
    ltv_write_vector(e, LTV_I64, (const uint8_t*) val, count);
}

void ltv_u8_vec(ltv_encoder_t *e, uint8_t *val, size_t count) {
    ltv_write_vector(e, LTV_U8, (const uint8_t*) val, count);
}

void ltv_u16_vec(ltv_encoder_t *e, uint16_t *val, size_t count) {
    ltv_write_vector(e, LTV_U16, (const uint8_t*) val, count);
}

void ltv_u32_vec(ltv_encoder_t *e, uint32_t *val, size_t count) {
    ltv_write_vector(e, LTV_U32, (const uint8_t*) val, count);
}

void ltv_u64_vec(ltv_encoder_t *e, uint64_t *val, size_t count) {
    ltv_write_vector(e, LTV_U64, (const uint8_t*) val, count);
}

void ltv_f32_vec(ltv_encoder_t *e, float *val, size_t count) {
    ltv_write_vector(e, LTV_F32, (const uint8_t*) val, count);
}

void ltv_f64_vec(ltv_encoder_t *e, double *val, size_t count) {
    ltv_write_vector(e, LTV_F64, (const uint8_t*) val, count);
}


////////////////////////////////////////////////////////////////////////////////
// UTF-8 Validation
////////////////////////////////////////////////////////////////////////////////

// Copyright (c) 2008-2010 Bjoern Hoehrmann <bjoern@hoehrmann.de>
// 
// Permission is hereby granted, free of charge, to any person obtaining 
// a copy of this software and associated documentation files (the "Software"), 
// to deal in the Software without restriction, including without limitation the 
// rights to use, copy, modify, merge, publish, distribute, sublicense, and/or sell 
// copies of the Software, and to permit persons to whom the Software is furnished 
// to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included in all 
// copies or substantial portions of the Software.

// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, 
// INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A 
// PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT 
// HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF 
// CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE 
// OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.

// See http://bjoern.hoehrmann.de/utf-8/decoder/dfa/ for details.
//
// Note:
// This implementation was chosen for its simplicity and code compactness.
// If you're looking for superior performance on desktop/server 
// class machines and are able to use modern C++, I recommend taking a look 
// at https://github.com/simdutf/is_utf8

#ifdef LTV_VALIDATE_UTF_8

#define UTF8_ACCEPT 0
#define UTF8_REJECT 12

static const uint8_t utf8d[] = {
    // The first part of the table maps bytes to character classes that
    // to reduce the size of the transition table and create bitmasks.
    0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,  0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
    0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,  0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
    0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,  0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
    0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,  0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
    1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,  9,9,9,9,9,9,9,9,9,9,9,9,9,9,9,9,
    7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,  7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,
    8,8,2,2,2,2,2,2,2,2,2,2,2,2,2,2,  2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,
    10,3,3,3,3,3,3,3,3,3,3,3,3,4,3,3, 11,6,6,6,5,8,8,8,8,8,8,8,8,8,8,8,

    // The second part is a transition table that maps a combination
    // of a state of the automaton and a character class to a state.
    0,12,24,36,60,96,84,12,12,12,48,72, 12,12,12,12,12,12,12,12,12,12,12,12,
    12, 0,12,12,12,12,12, 0,12, 0,12,12, 12,24,12,12,12,12,12,24,12,24,12,12,
    12,12,12,12,12,12,12,24,12,12,12,12, 12,24,12,12,12,12,12,12,12,24,12,12,
    12,12,12,12,12,12,12,36,12,36,12,12, 12,36,12,12,12,12,12,36,12,36,12,12,
    12,36,12,12,12,12,12,12,12,12,12,12
};

// Check whether a string is valid UTF-8
bool is_valid_utf8(const uint8_t *buf, size_t buf_len) {

    uint32_t state = UTF8_ACCEPT;
    for(size_t i=0; i < buf_len; i++){
        uint32_t type = utf8d[buf[i]];
        state = utf8d[256 + state + type];
    }
    return state == UTF8_ACCEPT;
}
#endif 

////////////////////////////////////////////////////////////////////////////////
// Decoder
////////////////////////////////////////////////////////////////////////////////

void ltv_decoder_init(ltv_decoder_t *d, const uint8_t* buf, size_t buf_len) {
    d->buf = buf;
    d->buf_len = buf_len;
    d->idx = 0;
    d->nest_depth = 0;
}


// Returns true if x + y > bound, with overflow check.
bool is_out_of_bounds(size_t x, size_t y, size_t bound) {
    size_t sum = x + y;
    return sum < x || sum > bound;
}

int ltv_next(ltv_decoder_t *d, ltv_data_t *data) {
    
    // Load pass-through elements
    int8_t i8;
    int16_t i16;
    int32_t i32;
    int64_t i64;

    // Bounds check
    if (d->idx == d->buf_len) {
        return d->nest_depth == 0 ? LTV_DECODE_EOF : LTV_DECODE_UNEXPECTED_EOF;
    }

    // Skip NOP tags
    while(d->buf[d->idx] == LTV_NOP_TAG) {
        d->idx++;
        if (d->idx == d->buf_len) {
            return d->nest_depth == 0 ? LTV_DECODE_EOF : LTV_DECODE_UNEXPECTED_EOF;
        }
    }

    // Initialize the data element
    memset(data, 0, sizeof(ltv_data_t));
    data->type_code = d->buf[d->idx] >> 4;
    data->size_code = d->buf[d->idx] & 0x0F;
    d->idx++;

    // Validate size code
    if (data->size_code > LTV_SIZE_8 || (data->type_code <= LTV_END && data->size_code != LTV_SINGLE)) {
        return LTV_DECODE_INVALID_SIZE_CODE;
    }

    // Struct structure
    if (d->nest_depth > 0) {

        // Toggle struct/end tags to keep track of what is expected.
        if (d->nest_stack[d->nest_depth-1] == LTV_STRUCT) {
            if (data->type_code != LTV_STRING && data->type_code != LTV_END) {
                return LTV_DECODE_INVALID_STRUCT_KEY;
            }
            d->nest_stack[d->nest_depth-1] = LTV_END;
        } else if (d->nest_stack[d->nest_depth-1] == LTV_END) {
            if (data->type_code == LTV_END) {
                return LTV_DECODE_EXPECTED_STRUCT_VALUE;
            }
             d->nest_stack[d->nest_depth-1] = LTV_STRUCT;
        }
    }

    // Track struct/list nesting
    if (data->type_code == LTV_STRUCT || data->type_code == LTV_LIST) {
        if (d->nest_depth >= LTV_MAX_NESTING_DEPTH) {
            return LTV_DECODE_MAX_DEPTH_REACHED;
        }
        d->nest_stack[d->nest_depth] = data->type_code;
        d->nest_depth++;
    }

    // Pop nest element on an end tag.
    if (data->type_code == LTV_END) {
        if (d->nest_depth == 0) {
            return LTV_DECODE_NEST_MISMATCH;
        }
        d->nest_depth--;
    }

    // Standalone tag
    if (data->type_code <= LTV_END) {
        return LTV_SUCCESS;
    }

    size_t typeSize = ltv_type_sizes[data->type_code];

    // Process single element.
    if (data->size_code == LTV_SINGLE) {

        data->length = typeSize;
        if (is_out_of_bounds(d->idx, data->length, d->buf_len)) {
            return LTV_DECODE_UNEXPECTED_EOF;
        }

        memcpy(&data->val.v_raw, &d->buf[d->idx], data->length);

        switch(data->type_code) {
            case LTV_STRING:
                data->val.v_buffer = &d->buf[d->idx];
                break;

            // Integer types upsized with sign extension.
            case LTV_I8:
                memcpy(&i8, &d->buf[d->idx], data->length);
                data->val.v_int = i8;
                break;

            case LTV_I16:
                memcpy(&i16, &d->buf[d->idx], data->length);
                data->val.v_int = i16;
                break;

            case LTV_I32:
                memcpy(&i32, &d->buf[d->idx], data->length);
                data->val.v_int = i32;
                break;

            case LTV_I64:
                memcpy(&i64, &d->buf[d->idx], data->length);
                data->val.v_int = i64;
                break;

            case LTV_F32:
                memcpy(&data->val.v_float32, &d->buf[d->idx], data->length);
                break;

            case LTV_F64:
                memcpy(&data->val.v_float64, &d->buf[d->idx], data->length);
                break;

            default:
                // Everything else is copied across to the raw buffer backing the union
                memcpy(&data->val.v_raw, &d->buf[d->idx], data->length);
                break;
        }

        d->idx += data->length;
        return LTV_SUCCESS;
    } 

    int lenSize = 1 << (data->size_code - LTV_SIZE_1);

    // Check bounds for reading the buffer length
    if (is_out_of_bounds(d->idx, lenSize, d->buf_len)){
        return LTV_DECODE_UNEXPECTED_EOF;
    }

    // Pick up the length
    memcpy(&data->length, &d->buf[d->idx], lenSize);
    d->idx += lenSize;

    // Check buffer length to make sure it is suitable for the contained datatype.
    // Because typeSize is divisible by 2, subtracting 1 gives exactly the bitmask 
    // needed to test if data->length is evenly divisible by typeSize.
    if ((data->length & (typeSize-1)) != 0) {
        return LTV_DECODE_INVALID_VECTOR_LENGTH;
    }

    // Check to see if the full buffer has been loaded
    if (is_out_of_bounds(d->idx, data->length , d->buf_len)) {
        return LTV_DECODE_UNEXPECTED_EOF;
    }

    // Take a reference to the vector within the buffer.
    data->val.v_buffer = &d->buf[d->idx];
    d->idx += data->length;

    // UTF-8 string validation
#ifdef LTV_VALIDATE_UTF_8
    if (data->type_code == LTV_STRING && !is_valid_utf8(data->val.v_buffer, data->length)) {
        return LTV_DECODE_INVALID_UTF8;
    }
#endif

    return LTV_SUCCESS;
}
