#include "litevectors.h"
#include "litevectors_util.h"

#include <string.h>
#include <stdio.h>
#include <stdio.h>
#include <float.h>
#include <math.h>
#include <limits.h>
#include <stdlib.h>

// Write a bunch of LiteVector data to a buffer.
void serialize(static_buffer_t *buf) {
    ltv_encoder_t c;
    ltv_encoder_init(&c, static_buffer_writer, buf);

    ltv_struct_start(&c);

    ltv_string(&c, "nil"); ltv_nil(&c);

    ltv_string(&c, "bool_false"); ltv_bool(&c, false);
    ltv_string(&c, "bool_true"); ltv_bool(&c, true);

    ltv_string(&c, "i8"); ltv_i8(&c, -123);
    ltv_string(&c, "u8"); ltv_u8(&c, 225);

    ltv_string(&c, "i16"); ltv_i16(&c, 1234);
    ltv_string(&c, "u16"); ltv_u16(&c, 50000);

    ltv_string(&c, "i32"); ltv_i32(&c, -40);
    ltv_string(&c, "u32"); ltv_u32(&c, 3000000000);

    ltv_string(&c, "i64"); ltv_i64(&c, -123456);
    ltv_string(&c, "u64"); ltv_u64(&c, 99);

    ltv_string(&c, "f32"); ltv_f32(&c, 123.45678901234566789);
    ltv_string(&c, "f64"); ltv_f64(&c, 123.45678901234566789);

    ltv_string(&c, "f64_nan"); ltv_f64(&c, NAN);
    ltv_string(&c, "f64_infinity"); ltv_f64(&c, INFINITY);
    ltv_string(&c, "f64_neg_infinity"); ltv_f64(&c, -INFINITY);

    ltv_string(&c, "string"); ltv_string(&c, "HOOP! (There it is)");
    ltv_string(&c, "string_non_ascii"); ltv_string(&c, "𝐋ṍ𝒓ḝм ℹꝑȿ𝘂м ԁ𝙤ŀ𝖔𝒓 𝘴𝝸ť 𝒂ᵯ𝕖ṯ");

    bool bools[] = { false, true, true, false, true, false, false, true };
    ltv_string(&c, "bool[]"); ltv_bool_vec(&c, bools, 8);

    int8_t i8_nums[] = { 1, 2, 3, 4, 5, -1, INT8_MIN, INT8_MAX };
    ltv_string(&c, "i8[]"); ltv_i8_vec(&c, i8_nums, 8);

    uint8_t u8_nums[] = { 1, 2, 3, 7, 8, 9, 0, UINT8_MAX };
    ltv_string(&c, "u8[]"); ltv_u8_vec(&c, u8_nums, 8);

    int16_t i16_nums[] = { 123, -123, 7, 8, 9, -1, INT16_MIN, INT16_MAX };
    ltv_string(&c, "i16[]"); ltv_i16_vec(&c, i16_nums, 8);

    uint16_t u16_nums[] = { 123, 456, 789, 1011, 1213, 0, 1, UINT16_MAX };
    ltv_string(&c, "u16[]"); ltv_u16_vec(&c, u16_nums, 8);

    int32_t i32_nums[] = { 123, 456, 789, 101112, 131415, -1, INT32_MIN, INT32_MAX };
    ltv_string(&c, "i32[]"); ltv_i32_vec(&c, i32_nums, 8);

    uint32_t u32_nums[] = { 123, 456, 789, 101112, 131415, 0, 1, UINT32_MAX };
    ltv_string(&c, "u32[]"); ltv_u32_vec(&c, u32_nums, 8);

    int64_t i64_nums[] = { 123, 456, 789, 101112, 131415, -1, INT64_MIN, INT64_MAX };
    ltv_string(&c, "i64[]"); ltv_i64_vec(&c, i64_nums, 8);

    uint64_t u64_nums[] = { 123, 456, 789, 101112, 131415, 0, 1, UINT64_MAX };
    ltv_string(&c, "u64[]"); ltv_u64_vec(&c, u64_nums, 8);

    float f32_nums[] = { 1.23, 4.56, 7.89, 1.01112, 1.31415, -1.0, FLT_MIN, FLT_MAX};
    ltv_string(&c, "f32[]"); ltv_f32_vec(&c, f32_nums, 8);

    double f64_nums[] = { 1.23, 4.56, 7.89, 1.01112, 1.31415, -1.0, DBL_MIN, DBL_MAX };
    ltv_string(&c, "f64[]"); ltv_f64_vec(&c, f64_nums, 8);

    ltv_string(&c, "list"); 
    ltv_list_start(&c);
        ltv_u32(&c, 123456789);
        ltv_nil(&c);
        ltv_bool(&c, true);
        ltv_string(&c, "A string");
    ltv_list_end(&c);

    ltv_string(&c, "map");
    ltv_struct_start(&c);
        ltv_string(&c, "level"); ltv_i8(&c, 1);
        ltv_string(&c, "nested"); ltv_bool(&c, 1);
        ltv_string(&c, "next");
        ltv_struct_start(&c);
            ltv_string(&c, "level"); ltv_i8(&c, 2);
            ltv_string(&c, "nested"); ltv_bool(&c, 1);
        ltv_struct_end(&c);
    ltv_struct_end(&c);

    ltv_string(&c, "boundaries");
    ltv_struct_start(&c);
        ltv_string(&c, "int8_MIN"); ltv_i8(&c, SCHAR_MIN);
        ltv_string(&c, "int16_MIN"); ltv_i16(&c, SHRT_MIN);
        ltv_string(&c, "int32_MIN"); ltv_i32(&c, INT_MIN);
        ltv_string(&c, "int64_MIN"); ltv_i64(&c, LLONG_MIN);

        ltv_string(&c, "int8_MAX"); ltv_i8(&c, SCHAR_MAX);
        ltv_string(&c, "int16_MAX"); ltv_i16(&c, SHRT_MAX);
        ltv_string(&c, "int32_MAX"); ltv_i32(&c, INT_MAX);
        ltv_string(&c, "int64_MAX"); ltv_i64(&c, LLONG_MAX);

        ltv_string(&c, "uint8_MAX"); ltv_u8(&c, UCHAR_MAX);
        ltv_string(&c, "uint16_MAX"); ltv_u16(&c, USHRT_MAX);
        ltv_string(&c, "uint32_MAX"); ltv_u32(&c, UINT_MAX);
        ltv_string(&c, "uint64_MAX"); ltv_u64(&c, ULLONG_MAX);

        ltv_string(&c, "float32_MIN"); ltv_f32(&c, FLT_MIN);
        ltv_string(&c, "float32_MAX"); ltv_f32(&c, FLT_MAX);
        ltv_string(&c, "float32_pos_zero"); ltv_f32(&c, +0.0f);
        ltv_string(&c, "float32_neg_zero"); ltv_f32(&c, -0.0f);
        ltv_string(&c, "float32_pos_infinity"); ltv_f32(&c, INFINITY);
        ltv_string(&c, "float32_neg_infinity"); ltv_f32(&c, -INFINITY);
        ltv_string(&c, "float32_nan"); ltv_f32(&c, NAN);

        ltv_string(&c, "float64_MIN"); ltv_f64(&c, DBL_MIN);
        ltv_string(&c, "float64_MAX"); ltv_f64(&c, DBL_MAX);
        ltv_string(&c, "float64_pos_zero"); ltv_f64(&c, +0.0);
        ltv_string(&c, "float64_neg_zero"); ltv_f64(&c, -0.0);
        ltv_string(&c, "float64_pos_infinity"); ltv_f64(&c, INFINITY);
        ltv_string(&c, "float64_neg_infinity"); ltv_f64(&c, -INFINITY);
        ltv_string(&c, "float64_nan"); ltv_f64(&c, NAN);
    ltv_struct_end(&c);

    ltv_struct_end(&c);

    if (c.status != 0) {
        printf("Encode error writing to static buffer: %d\n", c.status);
        exit(1);
    }
}

void assert_type(ltv_data_t *d, int expected) {
    if (d->type_code != expected) {
        printf("type code mismatch, expected: %d, got %d\n", expected, d->type_code);
        exit(1);
    }
}

void assert_len(ltv_data_t *d, size_t expected) {
    if (d->length != expected) {
        printf("length mismatch, expected: %zu, got %zu\n", expected, d->length);
        exit(1);
    }
}

void ex_type(ltv_decoder_t *dec, int expected) {
    ltv_data_t d;
    ltv_next(dec, &d);
    assert_type(&d, expected);
}

void ex_string(ltv_decoder_t *dec, const char *expected) {
    ltv_data_t d;
    ltv_next(dec, &d);

    assert_type(&d, LTV_STRING);
    if (!is_string_eq(&d, expected)) {
        printf("string expected: \"%s\", found \"%s\"\n", expected, d.val.v_buffer);
        exit(1);
    }
}

void ex_bool(ltv_decoder_t *dec, bool expected) {
    ltv_data_t d;
    ltv_next(dec, &d);

    assert_type(&d, LTV_BOOL);
    if (d.val.v_bool != expected) {
        printf("bool expected: %s, found %s\n", expected ? "TRUE" : "FALSE", d.val.v_bool ? "TRUE" : "FALSE");
        exit(1);
    }
}

void ex_int(ltv_decoder_t *dec, int64_t expected) {
    ltv_data_t d;
    ltv_next(dec, &d);

    if (!(d.type_code == LTV_I8 || d.type_code == LTV_I16 || d.type_code == LTV_I32 || d.type_code == LTV_I64)) {
        printf("type mismatch, expected: integer, got %d\n", d.type_code);
        exit(1);
    }

    if(d.val.v_int != expected) {
        printf("integer expected: %lld, got %lld\n", expected, d.val.v_int);
        exit(1);
    }
}

void ex_uint(ltv_decoder_t *dec, uint64_t expected) {
    ltv_data_t d;
    ltv_next(dec, &d);

    if (!(d.type_code == LTV_U8 || d.type_code == LTV_U16 || d.type_code == LTV_U32 || d.type_code == LTV_U64)) {
        printf("type mismatch, expected: unsigned integer, got: %d\n", d.type_code);
        exit(1);
    }

    if(d.val.v_uint != expected) {
        printf("integer expected: %llu, got: %llu\n", expected, d.val.v_int);
        exit(1);
    }
}

void ex_f32(ltv_decoder_t *dec, float expected) {
    ltv_data_t d;
    ltv_next(dec, &d);

    assert_type(&d, LTV_F32);

    if (isnan(expected)) {
        if (!isnan(d.val.v_float32)) {
            printf("f32 expected: NaN, got: %f\n", d.val.v_float32);
            exit(1);
        }
    } else if(d.val.v_float32 != expected) {
        printf("f32 expected: %f, got: %f\n", d.val.v_float32, expected);
        exit(1);
    }
}

void ex_f64(ltv_decoder_t *dec, double expected) {
    ltv_data_t d;
    ltv_next(dec, &d);

    assert_type(&d, LTV_F64);

    if (isnan(expected)) {
        if (!isnan(d.val.v_float64)) {
            printf("f64 expected: NaN, got: %f\n", d.val.v_float32);
            exit(1);
        }
    } else if(d.val.v_float64 != expected) {
        printf("f64 expected: %f, got: %f\n", d.val.v_float64, expected);
        exit(1);
    }
}

void ex_vec(ltv_decoder_t *dec, int expected_type, void* expected, size_t len) {
    ltv_data_t d;
    ltv_next(dec, &d);

    assert_type(&d, expected_type);
    assert_len(&d, len);

    if (memcmp(d.val.v_buffer, expected, len) != 0) {
        printf("vector mismatch!\n");
        exit(1);
    }
}

// Validate written data to make sure it's what we expected to see.
void validate(static_buffer_t *buf) {
    ltv_decoder_t dec;
    ltv_decoder_init(&dec, buf->data, buf->size);

    ex_type(&dec, LTV_STRUCT);
    ex_string(&dec, "nil"); ex_type(&dec, LTV_NIL);
    ex_string(&dec, "bool_false"); ex_bool(&dec, false);
    ex_string(&dec, "bool_true"); ex_bool(&dec, true);

    ex_string(&dec, "i8"); ex_int(&dec, -123);
    ex_string(&dec, "u8"); ex_uint(&dec, 225);
    
    ex_string(&dec, "i16"); ex_int(&dec, 1234);
    ex_string(&dec, "u16"); ex_uint(&dec, 50000);

    ex_string(&dec, "i32"); ex_int(&dec, -40);
    ex_string(&dec, "u32"); ex_uint(&dec, 3000000000);

    ex_string(&dec, "i64"); ex_int(&dec, -123456);
    ex_string(&dec, "u64"); ex_uint(&dec, 99);

    ex_string(&dec, "f32"); ex_f32(&dec, 123.45678901234566789);
    ex_string(&dec, "f64"); ex_f64(&dec, 123.45678901234566789);

    ex_string(&dec, "f64_nan"); ex_f64(&dec, NAN);
    ex_string(&dec, "f64_infinity"); ex_f64(&dec, INFINITY);
    ex_string(&dec, "f64_neg_infinity"); ex_f64(&dec, -INFINITY);

    ex_string(&dec, "string"); ex_string(&dec, "HOOP! (There it is)");
    ex_string(&dec, "string_non_ascii"); ex_string(&dec, "𝐋ṍ𝒓ḝм ℹꝑȿ𝘂м ԁ𝙤ŀ𝖔𝒓 𝘴𝝸ť 𝒂ᵯ𝕖ṯ");

    bool bools[] = { false, true, true, false, true, false, false, true };
    ex_string(&dec, "bool[]"); ex_vec(&dec, LTV_BOOL, bools, sizeof(bools));

    int8_t i8_nums[] = { 1, 2, 3, 4, 5, -1, INT8_MIN, INT8_MAX };
    ex_string(&dec, "i8[]"); ex_vec(&dec, LTV_I8, i8_nums, sizeof(i8_nums));

    uint8_t u8_nums[] = { 1, 2, 3, 7, 8, 9, 0, UINT8_MAX };
    ex_string(&dec, "u8[]"); ex_vec(&dec, LTV_U8, u8_nums, sizeof(u8_nums));

    // uint8_t i16_nums[] = { 1, 2, 3, 7, 8, 9, 0, UINT8_MAX };
    // ex_string(&dec, "u8[]"); ex_vec(&dec, LTV_U8, u8_nums, sizeof(u8_nums));

}

int main() {
    static_buffer_t buf = {.size=0};
    serialize(&buf);
    validate(&buf);

    printf("Round trip test finished successfully\n");
    return 0;
}
