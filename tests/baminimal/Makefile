# Makefile

all: minimal doit
all-benchmark: benchmark doit

doit: execute clean

minimal:
	emex64asm -o badapple src/*.e64 -fno_page_align

benchmark:
	emex64asm \
		-o badapple \
		src/*.e64 \
	    ../emexFirmware/src/uart.e64 \
	    ../emexFirmware/src/put.e64 \
	    ../emexFirmware/src/gui.e64 \
	    ../emexFirmware/src/font.e64 \
	    -fno_page_align \
	    -DBENCHMARK="1"

execute:
	emex64vm --firmware ./badapple

clean:
	rm -rf badapple
