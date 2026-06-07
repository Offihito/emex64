# Makefile

all: minimal doit
all-benchmark: benchmark doit
all-full: full doit

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

full:
	emex64asm \
		-o badapple \
		src/*.e64 \
		../emexFirmware/src/interrupt.e64 \
		../emexFirmware/src/pmm.e64 \
		../emexFirmware/src/mem.e64 \
		../emexFirmware/src/mc.e64 \
		-fno_page_align \
		-DFULL="1"

execute:
	emex64vm --firmware ./badapple

clean:
	rm -rf badapple
