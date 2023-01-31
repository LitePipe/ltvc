#include "litevectors.h"
#include "litevectors_util.h"

#include <string.h>

#define ARRAY_LEN(x) sizeof(x)/sizeof(x[0])

////////////////////////////////////////////////////////////////////////////////
// ltv_status_text
////////////////////////////////////////////////////////////////////////////////

const char* ltv_status_text(int status_code) {
    switch(status_code) {
        case LTV_SUCCESS: return "LTV_SUCCESS";
        case LTV_DECODE_EOF: return "LTV_DECODE_EOF: The end of the buffer was reached (no error)";
        case LTV_DECODE_UNEXPECTED_EOF: return "LTV_DECODE_UNEXPECTED_EOF: The decoder encountered the end of stream, but expected more data.";
        case LTV_DECODE_INVALID_SIZE_CODE: return "LTV_DECODE_INVALID_SIZE_CODE: An invalid size code was encountered";
        case LTV_DECODE_INVALID_VECTOR_LENGTH: return "LTV_DECODE_INVALID_VECTOR_LENGTH: A vector was found with a length that was not a multiple of its type size";
        case LTV_DECODE_INVALID_STRUCT_KEY: return "LTV_DECODE_INVALID_STRUCT_KEY: A string struct key was expected, but another type was found";
        case LTV_DECODE_EXPECTED_STRUCT_VALUE: return "LTV_DECODE_EXPECTED_STRUCT_VALUE: A struct value was found, but not found";
        case LTV_DECODE_MAX_DEPTH_REACHED: return "LTV_DECODE_MAX_DEPTH_REACHED: The incoming structure is nested deeper than the decoder is able to track.";
        case LTV_DECODE_NEST_MISMATCH: return "LTV_DECODE_NEST_MISMATCH: An unexpected END tag was found.";
        case LTV_DECODE_INVALID_UTF8: return "LTV_DECODE_INVALID_UTF8: A string was found that was not valid UTF-8";
        default: return "Unknown status code";
    }
}

////////////////////////////////////////////////////////////////////////////////
// helper functions
////////////////////////////////////////////////////////////////////////////////

bool is_string_eq(const ltv_data_t *v, const char *value) {
    return v->type_code == LTV_STRING && strncmp(value, (const char *)v->val.v_buffer, v->length) == 0;
}

bool is_uint(ltv_data_t *v) {
    return v->size_code == LTV_SINGLE && v->type_code >= LTV_U8 && v->type_code <= LTV_U64;
}

bool is_int(ltv_data_t *v) {
    return v->size_code == LTV_SINGLE && v->type_code >= LTV_I8 && v->type_code <= LTV_I64;
}

bool is_float(ltv_data_t *v) {
    return v->size_code == LTV_SINGLE && v->type_code == LTV_F32;
}

bool is_double(ltv_data_t *v) {
    return v->size_code == LTV_SINGLE && v->type_code == LTV_F64;
}

bool is_int_bound(ltv_data_t *v, int64_t min, int64_t max) {
    if(is_int(v)){
        return v->val.v_int >= min && v->val.v_int <= max;
    } else if (is_uint(v)) {
        if (max < 0 || v->val.v_uint > (uint64_t) INT64_MAX) {
            return false;
        }
        uint64_t umin = min < 0 ? 0 : (uint64_t) min;
        uint64_t umax = max < 0 ? 0 : (uint64_t) max;
        return v->val.v_uint >= umin && v->val.v_uint <= umax;
    }
    return false;
}

bool is_uint_bound(ltv_data_t *v, uint64_t min, uint64_t max) {
    if(is_int(v)) {
        if (v->val.v_int < 0) {
            return false;
        }

        int64_t imin = min > (uint64_t) INT64_MAX ? INT64_MAX : (int64_t) min;
        int64_t imax = max > (uint64_t) INT64_MAX ? INT64_MAX : (int64_t) max;
        return v->val.v_int >= imin && v->val.v_int <= imax;
    } else if (is_uint(v)) {
        return  v->val.v_uint >= min && v->val.v_uint <= max;
    }

    return false;
}

////////////////////////////////////////////////////////////////////////////////
// static_buffer_writer
////////////////////////////////////////////////////////////////////////////////

int static_buffer_writer(const uint8_t *buf, size_t len, void* user_data) {

    // Cast the static buffer instance passed to us.
    static_buffer_t* static_buf = user_data;

    // Make sure there is enough space for new data
    if ((static_buf->size + len) >= ARRAY_LEN(static_buf->data)) {
        return -1;
    }

    // Copy data to the static buffer
    memcpy(&static_buf->data[static_buf->size], buf, len);
    static_buf->size += len;
    return 0;
}