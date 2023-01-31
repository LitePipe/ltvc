
.PHONY : all
all : basic roundtrip ltv2json ltvdump

basic: basic.c ../litevectors.c ../litevectors_util.c
	cc -W -Wall -g -fsanitize=address -fno-omit-frame-pointer -o basic basic.c ../litevectors.c ../litevectors_util.c -I..

roundtrip: roundtrip.c ltv_json.c ../litevectors.c 
	cc -W -Wall -g -fsanitize=address -fno-omit-frame-pointer -o roundtrip roundtrip.c ltv_json.c ../litevectors.c ../litevectors_util.c -I..

ltv2json: ltv2json.c ltv_json.c ../litevectors.c 
	cc -W -Wall -g -fsanitize=address -fno-omit-frame-pointer -o ltv2json ltv_json.c ltv2json.c ../litevectors.c ../litevectors_util.c -I..

ltvdump: ltvdump.c
	cc -W -Wall -g -fsanitize=address -fno-omit-frame-pointer -o ltvdump ltvdump.c -I..

clean:
	rm -rf basic roundtrip ltv2json ltvdump c_data.ltv *.dSYM
