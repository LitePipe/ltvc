#include "litevectors.h"
#include "ltv_json.h"

#include <stdio.h>


int main(int argc, char** argv) {
    if (argc < 2) {
        printf("usage: ltb2json filename");
    }
    print_ltv_file(argv[1]);
}
