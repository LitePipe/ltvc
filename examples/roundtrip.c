// An example of directly serializing a chunk of data to a file.
// The ltv_json example code is then used to print the output file.

#include "litevectors.h"
#include "ltv_json.h"

#include <stdlib.h>
#include <stdio.h>
#include <float.h>
#include <time.h>
#include <math.h>
#include <limits.h>

// Our function that handles writing LiteVector encoded data to file.
int file_writer(const uint8_t *buf, size_t len, void* user_data) {
    if (fwrite(buf, len, 1, user_data) != 1) {
        printf("Unable to complete file write\n");
        exit(1);
    }
    return 0;
}

void write_test_buf(const char* filename) {
    FILE *f = fopen(filename, "wb");

    if (f == NULL) {
        printf("Unable to create file: %s\n", filename);
        exit(1);
    }

    ltv_encoder_t c;
    ltv_encoder_init(&c, file_writer, f);

    ltv_struct_start(&c);

    ltv_string(&c, "nil"); ltv_nil(&c);

    ltv_string(&c, "bool_false"); ltv_bool(&c, false);
    ltv_string(&c, "bool_true"); ltv_bool(&c, true);

    ltv_string(&c, "i8"); ltv_i8(&c, -123);
    ltv_string(&c, "u8"); ltv_u8(&c, 225);

    ltv_string(&c, "i16"); ltv_i16(&c, 1234);
    ltv_string(&c, "u16"); ltv_u16(&c, 50000);

    ltv_string(&c, "i32"); ltv_i32(&c, -40);
    ltv_string(&c, "u32"); ltv_u32(&c, -1);

    ltv_string(&c, "i64"); ltv_i64(&c, -123456);
    ltv_string(&c, "u64"); ltv_u64(&c, 99);

    ltv_string(&c, "f32"); ltv_f32(&c, 123.45678901234566789);
    ltv_string(&c, "f64"); ltv_f64(&c, 123.45678901234566789);

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

    fclose(f);
}

int main() {
    printf("Writing example lite vector to c_data.ltv\n");
    write_test_buf("./c_data.ltv");

    printf("Printing example file c_data.ltv\n");
    print_ltv_file("./c_data.ltv");
}
