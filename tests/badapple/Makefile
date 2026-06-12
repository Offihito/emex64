# Makefile
all: compile execute clean

compile:
	emex64asm -o badapple.img src/badapple.e64 ../emexFirmware/src/gui.e64 ../emexFirmware/src/uart.e64 ../emexFirmware/src/font.e64 ../emexFirmware/src/interrupt.e64 ../emexFirmware/src/pmm.e64 ../emexFirmware/src/mc.e64 ../emexFirmware/src/mem.e64 ../emexFirmware/src/ac97.e64 -DFULL="1" -DPUT_DRAW_LOGO="0" -I../emexFirmware/inc -Wl,-T,../emexFirmware/emex64.e64ld

execute:
	emex64vm --display required --audio on --firmware badapple.img

clean:
	rm badapple.img
