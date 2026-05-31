# Makefile

all: compile execute clean

compile:
	emex64asm -fno_page_align -o hello src/*.e64

execute:
	emex64vm --bios ./hello --memory 1

clean:
	rm hello
