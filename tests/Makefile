
CC = /opt/homebrew/opt/llvm/bin/clang

.PHONY: all
all: run_test_vectors fuzz round_trip_test

run_test_vectors: run_test_vectors.c ../litevectors.c ../litevectors_util.c
	$(CC) -fprofile-instr-generate -fcoverage-mapping -W -Wall -g -fsanitize=address -fsanitize=undefined -fno-omit-frame-pointer -O1 -o run_test_vectors run_test_vectors.c ../litevectors.c ../litevectors_util.c -I..

round_trip_test: round_trip_test.c ../litevectors.c ../litevectors_util.c
	$(CC) -W -Wall -g -fsanitize=address -fsanitize=undefined -fno-omit-frame-pointer -O1 -o round_trip_test round_trip_test.c ../litevectors.c ../litevectors_util.c -I..

fuzz: fuzz.c ../litevectors.c
	$(CC) -g -O1 -fsanitize=fuzzer,address fuzz.c -o fuzz ../litevectors.c -I..

clean:
	rm -rf run_test_vectors round_trip_test fuzz *.dSYM
