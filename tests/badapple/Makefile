# Makefile
ASM = emex64asm
LD = emex64ld
VM = emex64vm

# firmware headers
INCDIR = $(abspath ./../emexFirmware/inc)

ASMFLAGS = -fno_page_align -I$(INCDIR)
LDSCRIPT = $(abspath ./../emexFirmware/emex64.e64ld)

SRCDIR = src
BUILDDIR = build
TARGET = $(BUILDDIR)/emex64fw

SRCS = $(wildcard $(SRCDIR)/*.e64)
OBJS = $(patsubst $(SRCDIR)/%.e64,$(BUILDDIR)/%.e64o,$(SRCS))

LINK_ORDER = \
    $(BUILDDIR)/badapple.e64o \
    $(BUILDDIR)/../../emexFirmware/build/put.e64o \
    $(BUILDDIR)/../../emexFirmware/build/gui.e64o \
    $(BUILDDIR)/../../emexFirmware/build/uart.e64o \
    $(BUILDDIR)/../../emexFirmware/build/font.e64o \
    $(BUILDDIR)/../../emexFirmware/build/interrupt.e64o \
    $(BUILDDIR)/../../emexFirmware/build/pmm.e64o \
    $(BUILDDIR)/../../emexFirmware/build/mc.e64o \
    $(BUILDDIR)/../../emexFirmware/build/mem.e64o

all: all-full

all-full: ASMFLAGS += -DFULL="1"
all-full: compile execute

all-benchmark: ASMFLAGS += -DBENCHMARK="1"
all-benchmark: compile execute

all-minimal: compile execute

compile: $(TARGET)

$(BUILDDIR):
	mkdir -p $(BUILDDIR)

$(BUILDDIR)/%.e64o: $(SRCDIR)/%.e64 | $(BUILDDIR)
	$(ASM) $(ASMFLAGS) -o $@ $<

$(TARGET): $(LINK_ORDER) $(LDSCRIPT)
	$(LD) -T $(LDSCRIPT) -o $@ $(LINK_ORDER)

execute:
	$(VM) --firmware $(TARGET)

clean:
	rm -rf $(BUILDDIR)
