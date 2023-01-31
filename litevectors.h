#ifndef _LITEVECTORS_H
#define _LITEVECTORS_H

#include <stddef.h>
#include <stdint.h>
#include <stdbool.h>

////////////////////////////////////////////////////////////////////////////////
// Configurations
////////////////////////////////////////////////////////////////////////////////

// Comment this out to omit UTF-8 validation.
// It is a good idea to validate strings that will be used as data, but on 
// resource constrained systems that are only using strings as keys, this may
// be omitted to save code space.
#define LTV_VALIDATE_UTF_8 

// Comment this out to omit automatic vector alignment.
// Alignment inserts NOP tags to make vectors align on type size boundaries.
// This may be omitted to conserve output space.
#define LTV_VECTOR_ALIGNMENT 

////////////////////////////////////////////////////////////////////////////////
// Protocol Definitions
////////////////////////////////////////////////////////////////////////////////

// Type Codes
#define LTV_NIL       0
#define LTV_STRUCT    1
#define LTV_LIST      2
#define LTV_END       3
#define LTV_STRING    4
#define LTV_BOOL      5
#define LTV_U8        6
#define LTV_U16       7
#define LTV_U32       8
#define LTV_U64       9
#define LTV_I8       10
#define LTV_I16      11
#define LTV_I32      12
#define LTV_I64      13
#define LTV_F32      14
#define LTV_F64      15

// Buffer Lengths
#define LTV_SINGLE    0
#define LTV_SIZE_1    1
#define LTV_SIZE_2    2
#define LTV_SIZE_4    3
#define LTV_SIZE_8    4

#define LTV_NOP_TAG   0xFF


////////////////////////////////////////////////////////////////////////////////
// Encoder
////////////////////////////////////////////////////////////////////////////////

// Function provide by the client that handles writing serialized data.
// The function is responsible for writing all len bytes from buffer 
// before returning.
// 
// A return value of 0 indicates success. Any other return value is an
// application specific error code indicating a write failure.
// The encoder will store the value in its 'status' field.
typedef int (*ltv_writer)(const uint8_t *buf, size_t len, void* user_data);

typedef struct {
    // A user supplied write function.
    ltv_writer writer;

    // The opaque pointer passed to the writer function.
    void *user_data;

    // Current offset in the stream.
    size_t offset;

    // Holds the first non-zero status code returned from writer.
    int status;
} ltv_encoder_t;

// Initialize an encoder with a ltv_writer callback function.
// 'user_data' is passed to the ltv_writer function. 
void ltv_encoder_init(ltv_encoder_t *e, ltv_writer writer, void* user_data);

// Write single values to an encoder.
void ltv_nil(ltv_encoder_t *e);
void ltv_bool(ltv_encoder_t *e, bool val);
void ltv_i8(ltv_encoder_t *e, int8_t val);
void ltv_i16(ltv_encoder_t *e, int16_t val);
void ltv_i32(ltv_encoder_t *e, int32_t val);
void ltv_i64(ltv_encoder_t *e, int64_t val);
void ltv_u8(ltv_encoder_t *e, uint8_t val);
void ltv_u16(ltv_encoder_t *e, uint16_t val);
void ltv_u32(ltv_encoder_t *e, uint32_t val);
void ltv_u64(ltv_encoder_t *e, uint64_t val);
void ltv_f32(ltv_encoder_t *e, float val);
void ltv_f64(ltv_encoder_t *e, double val);

void ltv_struct_start(ltv_encoder_t *e);
void ltv_struct_end(ltv_encoder_t *e);
void ltv_list_start(ltv_encoder_t *e);
void ltv_list_end(ltv_encoder_t *e);

// Generic vector writer
void ltv_write_vector(ltv_encoder_t *e, uint8_t type_code, const uint8_t* buf, size_t count);

// Typed wrappers for ltv_write_vector
void ltv_string(ltv_encoder_t *e, const char* val);
void ltv_bool_vec(ltv_encoder_t *e, bool *val, size_t count);
void ltv_i8_vec(ltv_encoder_t *e, int8_t *val, size_t count);
void ltv_i16_vec(ltv_encoder_t *e, int16_t *val, size_t count);
void ltv_i32_vec(ltv_encoder_t *e, int32_t *val, size_t count);
void ltv_i64_vec(ltv_encoder_t *e, int64_t *val, size_t count);
void ltv_u8_vec(ltv_encoder_t *e, uint8_t *val, size_t count);
void ltv_u16_vec(ltv_encoder_t *e, uint16_t *val, size_t count);
void ltv_u32_vec(ltv_encoder_t *e, uint32_t *val, size_t count);
void ltv_u64_vec(ltv_encoder_t *e, uint64_t *val, size_t count);
void ltv_f32_vec(ltv_encoder_t *e, float *val, size_t count);
void ltv_f64_vec(ltv_encoder_t *e, double *val, size_t count);

////////////////////////////////////////////////////////////////////////////////
// Decoder
////////////////////////////////////////////////////////////////////////////////

// A value was successfully parsed from the decoder stream.
#define LTV_SUCCESS                        0

// The end of the buffer was reached (no error).
#define LTV_DECODE_EOF                     1

// The decoder encountered the end of stream, but expected more data.
#define LTV_DECODE_UNEXPECTED_EOF          2

// An invalid size code was encountered
#define LTV_DECODE_INVALID_SIZE_CODE       3

// A vector was found with a length that was not a multiple of its type size
#define LTV_DECODE_INVALID_VECTOR_LENGTH   4

// A string struct key was expected, but another type was found
#define LTV_DECODE_INVALID_STRUCT_KEY      5

// A struct value was found, but not found
#define LTV_DECODE_EXPECTED_STRUCT_VALUE   6

// A string was found that was not valid UTF-8
#define LTV_DECODE_INVALID_UTF8            7

// The incoming structure is nested deeper than the decoder is able to track.
#define LTV_DECODE_MAX_DEPTH_REACHED       8 

// An unexpected END tag was found.
#define LTV_DECODE_NEST_MISMATCH           9

// The maximum struct/list nesting depth supported.
#define LTV_MAX_NESTING_DEPTH             32

typedef struct {
    uint8_t type_code;
    uint8_t size_code;
    size_t length;
    union {
        bool v_bool;
        int64_t v_int;
        uint64_t v_uint;
        float v_float32;
        double v_float64;
        const uint8_t* v_buffer;
        uint8_t v_raw[8];
    } val;
} ltv_data_t;

typedef struct {
    const uint8_t *buf;
    size_t buf_len;
    size_t idx;
    uint8_t nest_stack[LTV_MAX_NESTING_DEPTH];
    size_t nest_depth;
} ltv_decoder_t;

// Initialize a decoder for a buffer of data.
void ltv_decoder_init(ltv_decoder_t *d, const uint8_t* buf, size_t buf_len);

// Get the next value from a LiteVector stream.
int ltv_next(ltv_decoder_t *d, ltv_data_t *data);

#endif //_LITEVECTORS_H