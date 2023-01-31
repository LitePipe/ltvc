// An example of directly parsing LiteVector data from a stream.
// This does some basic data validation, but not as comprehensive
// as the LiteVector decoder. 
// 
// As an example using this, generate a data file:
//      ./roundtrip
// and then direct its output as a stream to ltvdump:
//      ./ltvdump < ./c_data.ltv

#include "litevectors.h"

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <float.h>
#include <math.h>
#include <time.h>
#include <inttypes.h>

#define UNUSED(x) (void)x

////////////////////////////////////////////////////////////////////////////////
//
// Value printing functions
//
typedef void (*ValuePrinter)(const uint8_t *buf, size_t len);

void print_bool(const uint8_t *buf, size_t len) {
    UNUSED(len);
    printf("%s", buf[0] ? "true" : "false");
}

void print_u8(const uint8_t *buf, size_t len) {
    UNUSED(len);
    printf("%hhu", buf[0]);
}

void print_u16(const uint8_t *buf, size_t len) {
    uint16_t val;
    memcpy(&val, buf, len);
    printf("%hu", val);
}

void print_u32(const uint8_t *buf, size_t len) {
    uint32_t val;
    memcpy(&val, buf, len);
    printf("%u", val);
}

void print_u64(const uint8_t *buf, size_t len) {
    uint64_t val;
    memcpy(&val, buf, len);
    printf("%" PRIu64, val);
}

void print_i8(const uint8_t *buf, size_t len) {
    UNUSED(len);
    printf("%hhd", buf[0]);
}

void print_i16(const uint8_t *buf, size_t len) {
    int16_t val;
    memcpy(&val, buf, len);
    printf("%hd", val);
}

void print_i32(const uint8_t *buf, size_t len) {
    int32_t val;
    memcpy(&val, buf, len);
    printf("%d", val);
}

void print_i64(const uint8_t *buf, size_t len) {
    int64_t val;
    memcpy(&val, buf, len);
    printf("%" PRId64, val);
}

void print_f64_ex(double v) {
    if (isnan(v)) {
        printf("NaN");
        return;
    }

    if (isinf(v)) {
        if (v < -FLT_MAX) {
            printf("-Infinity");   
        } else {
            printf("Infinity");
        }
        return;
    }

    printf("%g", v);
}

void print_f32(const uint8_t *buf, size_t len) {
    float val;
    memcpy(&val, buf, len);
    print_f64_ex(val);
}

void print_f64(const uint8_t *buf, size_t len) {
    double val;
    memcpy(&val, buf, len);
    print_f64_ex(val);
}

void print_hex(const uint8_t *buf, size_t len) {
    for(size_t idx=0; idx < len; idx++) {
        printf("%02X", buf[idx]);
    }
}

////////////////////////////////////////////////////////////////////////////////
//
// Protocol definitions
//

typedef struct ltbType {
    char const *name;
    const size_t size;
    ValuePrinter print;
}LtbType;

static const LtbType LTV_TYPES[] = {
    {"nil", 0, NULL},
    {"struct", 0, NULL},
    {"list", 0, NULL},
    {"end", 0, NULL},
    {"string", 1, NULL},
    {"bool", 1, print_bool},
    {"u8", 1, print_u8},
    {"u16", 2, print_u16},
    {"u32", 4, print_u32},
    {"u64", 8, print_u64},
    {"i8", 1, print_i8},
    {"i16", 2, print_i16},
    {"i32", 4, print_i32},
    {"i64", 8, print_i64},
    {"f32", 4, print_f32},
    {"f64", 8, print_f64},
};

////////////////////////////////////////////////////////////////////////////////
// Stream Facilities
//

typedef struct _stream {
    size_t offset;
    FILE *fd;
}Stream;

void init_stream(Stream *s, FILE* fd) {
    s->offset = 0;
    s->fd = fd;
}

int read_tag(Stream *s) {
    int tag = fgetc(s->fd);

    // Reading an EOF on a tag boundary is okay - we're done.
    if (tag == EOF) {
        exit(0);
    }

    s->offset++;
    return tag;
}

void read(Stream *s, uint8_t *buf, int len) {
    int nread = fread(buf, 1, len, s->fd);
    if (nread < len) {
        printf("ERROR: Unexpected EOF\n");
        exit(-1);
    }

    s->offset += nread;
}

void dump_string(Stream *s, int len) {
    uint8_t buf[1024];
    int blockSize = sizeof(buf);

    putchar('"');

    int toRead = len < blockSize ? len : blockSize;
    while(len > 0) {
        read(s, buf, toRead);
        printf("%.*s", toRead, buf);
        len -= toRead;
    }

    putchar('"');
}

void dump_bytes(Stream *s, int len) {
    uint8_t buf[1024];
    int blockSize = sizeof(buf);

    int toRead = len < blockSize ? len : blockSize;
    while(len > 0) {
        read(s, buf, toRead);
        print_hex(buf, toRead);
        len -= toRead;
    }
}

////////////////////////////////////////////////////////////////////////////////

void ltv_dump(Stream *s) {
    printf("Tag_Offset  Tag   Type     Flag   Length  Value_Offset  Value \n");

    for(;;) {

        size_t tagOffset = s->offset;
        int tag = read_tag(s);
        int type_code = tag >> 4;
        int size_code = (tag & 0x0F);

        printf("  %08zX   %02X", tagOffset, tag);

        if (tag == LTV_NOP_TAG) {
            printf("   NOP\n");
            continue;
        }

        printf("   %-10s  %d ", LTV_TYPES[type_code].name, size_code);



        // Validation: tag flag bounds
        if (size_code > LTV_SIZE_8) {
            printf("VALIDATION ERROR: Out of range size code\n");
            exit(-1);
        }

        size_t length = 0;
        if (type_code <= LTV_END || size_code == 0) {
            // Single element
            length = LTV_TYPES[type_code].size;
        } else {
            // Vector
            int lenSize = 1 << (size_code - LTV_SIZE_1);
            read(s, (uint8_t *) &length, lenSize);
        }

        // Length
        printf("%8zu      ", length);
        
        // Value offset
        printf("%08zX  ", s->offset);

        // Zero length structures
        if (type_code == LTV_NIL) {
            printf("(nil)");      
        } else if (type_code == LTV_STRUCT) {
            putchar('{');
        } else if (type_code == LTV_LIST) {
            putchar('[');
        } else if(type_code == LTV_END) {
            putchar(']');
        }

        if (type_code <= LTV_END) {
            printf("\n");
            continue;
        }

        // Validation: even length
        if(length % LTV_TYPES[type_code].size != 0) {
            printf("VALIDATION ERROR: Vector length of %zu found for elements of size %zu\n", length, LTV_TYPES[type_code].size);
            exit(-1);
        }

        // String
        if (type_code == LTV_STRING) {
            dump_string(s, length);
            printf("\n");
            continue;
        }

        // Vector start 
        if (size_code != 0) {
            putchar('[');
        }

        // Print value(s)
        uint8_t buf[16];
        bool first = true;
        for(size_t proc=0; proc < length; proc += LTV_TYPES[type_code].size) {
            read(s, buf, LTV_TYPES[type_code].size);

            if (!first) {
                printf(", ");
            }
            first = false;
            LTV_TYPES[type_code].print(buf, LTV_TYPES[type_code].size);
        }

        // Vector end
        if (size_code != 0) {
            putchar(']');
        }

        printf("\n");
    }    
}

int main() {
    Stream s;
    init_stream(&s, stdin);
    ltv_dump(&s);
}
