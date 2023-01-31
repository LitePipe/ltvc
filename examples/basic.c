// This is an example demonstrating round trip serialization and deserialization of 
// a fairly simple data structure. It uses the static_buffer_t utility, which
// works out of a single, statically defined memory buffer.
//
// Deserializing a buffer back into a structure is demonstrated, along with
// some techniques for validating the incoming data and handling data that isn't
// exactly the same type.

#include <stdio.h>
#include <string.h>
#include "litevectors.h"
#include "litevectors_util.h"

#define ARRAY_LEN(x) sizeof(x)/sizeof(x[0])

// The hero of our story, a humble everyday data structure.
struct MyData {
    char Name[32];
    uint32_t Setting_A;
    int32_t Setting_B;
    float Setting_C;
    float CalibrationVector[16];
};

// A utility function to print out our heroic data structure.
void MyData_print(struct MyData *d) {
    printf("Name : %s\n", d->Name);
    printf("Set_A: %d\n", d->Setting_A);
    printf("Set_B: %d\n", d->Setting_B);
    printf("Set_C: %f\n", d->Setting_C);
    
    printf("Cali : [");
    for(size_t i=0; i < ARRAY_LEN(d->CalibrationVector); i++) {
        if (i != 0) {
            printf(", ");
        }
        printf("%f", d->CalibrationVector[i]);
    }
    printf("]\n");
}

// Function to serialize the data structure into a LiteVector.
void MyData_serialize(ltv_encoder_t *enc, struct MyData *d) {

    // Use a struct for name/value pairs
    ltv_struct_start(enc);

    ltv_string(enc, "Name"); ltv_string(enc, d->Name);
    ltv_string(enc, "Setting_A"); ltv_u64(enc, d->Setting_A);
    ltv_string(enc, "Setting_B"); ltv_i32(enc, d->Setting_B);
    ltv_string(enc, "Setting_C"); ltv_f32(enc, d->Setting_C);
    ltv_string(enc, "CalibrationVector"); ltv_f32_vec(enc, d->CalibrationVector, ARRAY_LEN(d->CalibrationVector));

    ltv_struct_end(enc);
}

// Function to deserialize an instance of our data structure from buffer containing LiteVector data.
bool MyData_deserialize(ltv_decoder_t *dec,  struct MyData *d) {
    
    ltv_data_t k, v;

    // Initialize MyData structure.
    // Optionally: Set application default field values
    memset(d, 0, sizeof(struct MyData));

    // This implements a loose structure parsing - 
    // the struct is a collection of key value pairs, and when a
    // key is found, it sets the corresponding struct field.
    
    // Example field validation is performed. 
    // Validation is usually quite application specific, so 
    // this should be tailored to your application's needs.

    // Read the leading struct tag
    if (ltv_next(dec, &k) != LTV_SUCCESS) return false;
    if (k.type_code != LTV_STRUCT) return false;

    // Read next key or struct end
    if (ltv_next(dec, &k) != LTV_SUCCESS) return false;

    // Traverse the structure, deserializing our fields until we hit the end
    while(k.type_code != LTV_END) {

        // Read value
        if (ltv_next(dec, &v) != LTV_SUCCESS) return false;

        // Switch on struct key
        if (is_string_eq(&k, "Name")) {

            // Validate type
            if  (v.type_code != LTV_STRING) {
                printf("Unexpected type for MyData.Name\n");
                return false;
            }
        
            // Validate length
            if (v.length > sizeof(d->Name)-1) {
                printf("MyData.Name value is too long\n");
                return false;
            }

            memcpy(d->Name, v.val.v_buffer, v.length);
            d->Name[v.length] = 0;

        } else if (is_string_eq(&k, "Setting_A")) {
            
            // Validate Setting_A - can be any valid value
            if (!is_uint_bound(&v, 0, UINT32_MAX)) {
                printf("Setting_A is out of bounds!\n");
                return false;
            }

            d->Setting_A = (uint32_t)v.val.v_uint; 

        } else if (is_string_eq(&k, "Setting_B")) {

            // Validate Setting_B- must be between -10 and 50 (for example)
            if (!is_int_bound(&v, -10, 50)) {
                printf("Setting_B is out of bounds!\n");
                return false;
            }

            d->Setting_B = (int32_t)v.val.v_int;

        } else if (is_string_eq(&k, "Setting_C")) {

            // Validate Setting_C - any number is taken
            if (is_float(&v)) {
                d->Setting_C = v.val.v_float32;
            } else if (is_double(&v)) {
                d->Setting_C = (float)v.val.v_float64;
            } else if (is_int(&v)) {
                d->Setting_C = (float)v.val.v_int;
            } else if (is_uint(&v)) {
                d->Setting_C = (float)v.val.v_uint;
            } else {
                // If no number is found, then fail.
                printf("Setting_C is not a valid number!\n");
                return false;
            }

        } else if (is_string_eq(&k, "CalibrationVector")) {
            // Require a vector of 32-bit floating point numbers.
            if  (!(v.type_code == LTV_F32 && v.size_code > LTV_SINGLE)) {
                printf("Invalid type for CalibrationVector\n");
                return false;
            }

            // Apply whatever length is given.
            int len = v.length > sizeof(d->CalibrationVector) ? sizeof(d->CalibrationVector) : v.length;
            memcpy(d->CalibrationVector, v.val.v_buffer, len);
        }

        //  Read next key or struct end
        if (ltv_next(dec, &k) != LTV_SUCCESS) return false;
    }

    return true;
}

int main() {
    printf("LiteVector Basic Example\n\n");

    struct MyData data = {
        .Name = "SensorMcSenseFace",
        .Setting_A = 5,
        .Setting_B = 5,
        .Setting_C = 7.89,
    };

    for(size_t i=0; i < ARRAY_LEN(data.CalibrationVector); i++) {
        data.CalibrationVector[i] = i + 0.123f + ( (float)i * 2);
    }

    printf("Original MyData:\n");
    MyData_print(&data);

    printf("\n");

    // Setup a static buffer to receive the serialized output.
    static_buffer_t staticBuf = {.size=0};

    // Create a LiteVector serializer
    ltv_encoder_t enc;
    ltv_encoder_init(&enc, static_buffer_writer, &staticBuf);

    // Serialize 'data' into the buffer.
    MyData_serialize(&enc, &data);

    // Print the contents of the StaticBuffer to see what's there
    printf("Serialized Output: \n");
    for(size_t i=0; i < staticBuf.size; i++) {
        printf("%02x", staticBuf.data[i]);
    }
    printf("\n\n");

    struct MyData data2;
    ltv_decoder_t dec;
    ltv_decoder_init(&dec, staticBuf.data, staticBuf.size);
    if (MyData_deserialize(&dec, &data2)) {
        printf("Deserialized MyData:\n\n");
        MyData_print(&data2);
    } else {
        printf("Unable to deserialize MyData\n");
    }
}