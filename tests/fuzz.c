#include "litevectors.h"

extern int LLVMFuzzerTestOneInput(const uint8_t *Data, size_t Size) {

    int status;
    ltv_decoder_t dec;
    ltv_data_t data;

    ltv_decoder_init(&dec, Data, Size);
    do {
        status = ltv_next(&dec, &data);
    } while(status == LTV_SUCCESS);

    return 0;
}