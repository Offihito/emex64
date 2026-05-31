# Makefile

all: compile execute clean

compile:
	emex64asm -o emex64OS src/*.e64

execute:
	emex64vm --bios ./emex64OS --memory 20

clean:
	rm -rf emex64OS
