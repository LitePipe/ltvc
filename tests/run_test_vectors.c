// Parse and process the LiteVector test vector suite.
//
#include "litevectors.h"
#include "litevectors_util.h"

#include <string.h>
#include <stdio.h>
#include <stdlib.h>

char descBuf[8192];
char dataBuf[8192];
uint8_t binBuf[sizeof(dataBuf)/2];

// Convert a hex character to a number.
uint8_t hex2num(char h) {
    if (h >= '0' && h <= '9') return h - '0';
    if (h >= 'a' && h <= 'f') return h - 'a' + 10;
    if (h >= 'A' && h <= 'F') return h - 'A' + 10;

    // Data error
    printf("Invalid hex character: %d\n", h);
    exit(1);
} 

// Dump a data value for diagnostic.
void printLtvData(ltv_data_t *d) {
    printf("Data\n");
    printf("  type_code: %d\n", d->type_code);
    printf("  size_code: %d\n", d->size_code);
    printf("  length:   %zu\n", d->length);
}

// Read and process a test vector file. 
// if positive is true, all vectors should parse successfully
// if positive if false, all vectors should throw an error
void processTestVector(const char* fileName, bool positive) {

    int status;
    ltv_decoder_t dec;
    ltv_data_t data;

    FILE *fd = fopen(fileName, "r");
    if (fd == NULL) {
        printf("Error: Unable to open file %s\n", fileName); 
        exit(-1);
    }

    while(fgets(descBuf, sizeof(descBuf), fd)) {
        fgets(dataBuf, sizeof(dataBuf), fd);

        // Decode hex from textBuf into dataBuf
        int dataLen = 0;
        for(unsigned long i=0; i < sizeof(binBuf) && dataBuf[i] != '\n' && dataBuf[i] != 0; i += 2, dataLen++) {
            binBuf[dataLen] = hex2num(dataBuf[i]) << 4 | hex2num(dataBuf[i+1]);
        }

        // Intialize the decoder
        ltv_decoder_init(&dec, binBuf, dataLen);

        // Parse through the data
        do {
            status = ltv_next(&dec, &data);
        } while(status == LTV_SUCCESS);

        if (positive && status != LTV_DECODE_EOF) {
            printf("Test: %s", descBuf);
            printf("Data: %s", dataBuf);
            printf("Unexpected error decoding positive vector: %s\n", ltv_status_text(status));
            printLtvData(&data);
            exit(-1);
        }
    
        if (!positive && status == LTV_DECODE_EOF) {
            printf("Test: %s", descBuf);
            printf("Data: %s", dataBuf);
            printf("Unflagged decode error in negative vector\n");
            printLtvData(&data);
            exit(-1);
        }
    }
}

int main() {
    processTestVector("litevectors_positive.txt", true);
    processTestVector("litevectors_negative.txt", false);
    printf("Tests Completed Successfully\n");
    return 0;
}
