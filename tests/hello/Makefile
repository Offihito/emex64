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
OBJS = $(patsubst $(SRCDIR)/%.e64,$(BUILDDIR)/%.o,$(SRCS))

LINK_ORDER := $(BUILDDIR)/start.o

all: compile execute

compile: $(TARGET)

$(BUILDDIR):
	mkdir -p $(BUILDDIR)

$(BUILDDIR)/%.o: $(SRCDIR)/%.e64 | $(BUILDDIR)
	$(ASM) $(ASMFLAGS) -o $@ $<

$(TARGET): $(LINK_ORDER) $(LDSCRIPT)
	$(LD) -T $(LDSCRIPT) -o $@ $(LINK_ORDER)

execute:
	$(VM) --firmware $(TARGET) --display off

clean:
	rm -rf $(BUILDDIR)
