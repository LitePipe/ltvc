#include "litevectors.h"
#include "litevectors_util.h"

#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include <time.h>
#include <math.h>
#include <float.h>
#include <inttypes.h>

#include <stdlib.h>

// Min/max integers representable in JSON/JavaScript
// See: https://developer.mozilla.org/en-US/docs/Web/JavaScript/Reference/Global_Objects/Number/MAX_SAFE_INTEGER
#define JS_MAX_SAFE_INT  9007199254740991
#define JS_MIN_SAFE_INT -9007199254740991

// Element lengths in bytes
static const int ltv_type_sizes[] = {0, 0, 0, 0, 1, 1, 1, 2, 4, 8, 1, 2, 4, 8, 4, 8};

void print_indent(int level) {
    int indent = level * 4;
    for(int i=0; i < indent; i++) {
        putchar(' ');
    }
}

void print_string(const uint8_t *buf, int len) {
    putchar('"');
    printf("%.*s", len, buf);
    putchar('"');
}

void print_base64(const uint8_t *buf, size_t len) {
    
    static const unsigned char base64_table[65] ="ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";

    putchar('"');
    const unsigned char *end, *in;
    size_t olen;

    olen = 4*((len + 2) / 3); /* 3-byte blocks to 4-byte */
    if (olen < len) {
        return;
    }

    end = buf + len;
    in = buf;
    while (end - in >= 3) {
        putchar(base64_table[in[0] >> 2]);
        putchar(base64_table[((in[0] & 0x03) << 4) | (in[1] >> 4)]);
        putchar(base64_table[((in[1] & 0x0f) << 2) | (in[2] >> 6)]);
        putchar(base64_table[in[2] & 0x3f]);
        in += 3;
    }

    if (end - in) {
        putchar(base64_table[in[0] >> 2]);
        if (end - in == 1) {
            putchar(base64_table[(in[0] & 0x03) << 4]);
            putchar('=');
        }
        else {
            putchar(base64_table[((in[0] & 0x03) << 4) | (in[1] >> 4)]);
            putchar(base64_table[(in[1] & 0x0f) << 2]);
        }
        putchar('=');
    }

    putchar('"');
}

void print_int64(int64_t v) {
    if (v < JS_MIN_SAFE_INT || v > JS_MAX_SAFE_INT) {
        // Wrap 'unsafe' integers in quotes
        printf("\"%" PRId64 "\"", v);          
    }
    else {
        printf("%" PRId64, v);
    }
}

void print_uint64(uint64_t v) {
    if (v > JS_MAX_SAFE_INT) {
        // Wrap 'unsafe' integers in quotes
        printf("\"%" PRIu64 "\"", v);          
    }
    else {
        printf("%" PRIu64, v);
    }
}

void print_float64(double v) {
    if (isnan(v)) {
        printf("\"NaN\"");
        return;
    }

    if (isinf(v)) {
        if (v < -FLT_MAX) {
            printf("\"-Infinity\"");   
        } else {
            printf("\"Infinity\"");
        }
        return;
    }

    printf("%g", v);
}


void print_array(ltv_data_t *data) {
    // Passthrough values
    int16_t i16;
    int32_t i32;
    int64_t i64;
    uint16_t u16;
    uint32_t u32;
    uint64_t u64;
    float f32;
    double f64;

    bool firstElement = true;
    size_t typeSize = ltv_type_sizes[data->type_code];
    const uint8_t *buf = data->val.v_buffer;

    printf("[");
    for(size_t i=0; i < data->length; i += typeSize) {
        if (!firstElement) {
            printf(", ");
        }
        firstElement = false;

        switch(data->type_code) {
            case LTV_BOOL:
                printf(buf[i] ? "true" : "false");
                break;

            case LTV_U8:
                printf("%hhu", buf[i]);
                break;

            case LTV_U16:
                memcpy(&u16, buf+i, typeSize);
                printf("%hu", u16);
                break;

            case LTV_U32:
                memcpy(&u32, buf+i, typeSize);
                printf("%u", u32);
                break;

            case LTV_U64:
                memcpy(&u64, buf+i, typeSize);
                print_uint64(u64);
                break;

            case LTV_I8:
                printf("%hhd", buf[i]);
                break;

            case LTV_I16:
                memcpy(&i16, buf+i, typeSize);
                printf("%hd", i16);
                break;

            case LTV_I32:
                memcpy(&i32, buf+i, typeSize);
                printf("%d", i32);
                break;

            case LTV_I64:
                memcpy(&i64, buf+i, typeSize);
                print_int64(i64);
                break;

            case LTV_F32:
                memcpy(&f32, buf+i, typeSize);
                print_float64(f32);
                break;

            case LTV_F64:
                memcpy(&f64, buf+i, typeSize);
                print_float64(f64);
                break;
        }
    }

    printf("]");
}

int print_ltv_r(ltv_decoder_t *d, int level, bool inMap, bool firstPrint) {
    int64_t ret = 0;
    ltv_data_t data;
    int mapValue = 0;
    bool firstValue = true;

    for(;;) {

        // Check to see if we're finished
        if (d->idx == d->buf_len) {
            return LTV_SUCCESS;
        }

        // Read the next value
        ret = ltv_next(d, &data);
        if (ret != LTV_SUCCESS) {
            break;
        }

        // Map/List close
        if (data.type_code == LTV_END) {
            return ret;
        }

        // Value formatting
        if(!mapValue) {
            if (!firstValue) {
                printf(",");
            } 
            firstValue = false;

            if (!firstPrint) {
                printf("\n");      
            }
            firstPrint = false;

            print_indent(level);
        } else {
            printf(": ");
        }

        if (inMap) {
            mapValue = !mapValue;
        }

        ////////////////////////////////////////////////////////////////////////////////
        // Single Values

        if (data.size_code == LTV_SINGLE) {

            // Map
            if (data.type_code == LTV_STRUCT) {
                putchar('{');
                ret = print_ltv_r(d, level+1, true, firstPrint);
                putchar('\n');

                if (ret != LTV_SUCCESS) {
                    continue;
                }

                print_indent(level);
                putchar('}');
                continue;
            }

            // List
            if (data.type_code == LTV_LIST) {
                putchar('[');
                ret = print_ltv_r(d, level+1, false, firstPrint);
                putchar('\n');

                if (ret != LTV_SUCCESS) {
                    continue;
                }

                print_indent(level);
                putchar(']');
                continue;
            }
            
            // Numbers
            switch(data.type_code) {
                
                case LTV_NIL:
                    printf("null");
                    break;

                case LTV_BOOL: 
                    printf(data.val.v_bool ? "true" : "false");
                    break;

                case LTV_U8:
                case LTV_U16:
                case LTV_U32:
                case LTV_U64:
                    print_uint64(data.val.v_uint);
                    break;

                case LTV_I8:
                case LTV_I16:
                case LTV_I32:
                case LTV_I64:
                    print_int64(data.val.v_int);
                    break;

                case LTV_F32:
                    print_float64(data.val.v_float32);
                    break; 

                case LTV_F64:
                    print_float64(data.val.v_float64);
                    break;

                default:
                    printf("?");
            }

            continue;
        }

        ////////////////////////////////////////////////////////////////////////////////
        // Buffers

        // String
        if (data.type_code == LTV_STRING) {
            print_string(data.val.v_buffer, data.length);
            continue;
        }

        // Arrays
        if (data.length > 0) {
            print_array(&data);
        }
    }

    // Only print errors at the base level.
    if (level > 0 || ret == LTV_SUCCESS || ret == LTV_DECODE_EOF) {
        return ret;
    }

    printf("%s\n", ltv_status_text(ret));
    return ret;
}

int64_t print_ltv(const uint8_t *buf, size_t bufLen) {
    ltv_decoder_t d;
    ltv_decoder_init(&d, buf, bufLen);

    return print_ltv_r(&d, 0, false, true);
}


void print_ltv_file(const char* filename) {
    FILE *fd = fopen(filename, "rb");
    if (fd == NULL) {
        printf("Error: Unable to open data file\n");
        return;
    }

    // Get the file size
    fseek(fd, 0L, SEEK_END);
    size_t fileSize = ftell(fd);
    fseek(fd, 0, SEEK_SET);

    // Create a buffer for the file contents
    uint8_t *buf = malloc(fileSize);
    if (buf == NULL) {
        printf("Error: Unable to allocate buffer for file (size %zu)\n", fileSize);
    }

    // Read the file into memory
    size_t nread = fread(buf, 1, fileSize, fd);
    fclose(fd);
    
    if (nread < fileSize) {
        if (feof(fd))
            printf("Error: unexpected end of file\n");
        else if (ferror(fd)) {
            perror("Error: reading");
        }

        return;
    }

    // Print the vector
    print_ltv(buf, nread);
    printf("\n");
}
