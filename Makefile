# Cross toolchain variables
# If these are not in your path, you can make them absolute.
XT_PRG_PREFIX = mipsel-linux-gnu-
CC = $(XT_PRG_PREFIX)gcc
LD = $(XT_PRG_PREFIX)ld

# uMPS2-related paths

# Simplistic search for the umps2 install. prefix. If you have umps2
# installed on some weird location, set UMPS2_DIR_PREFIX by hand.
ifneq ($(wildcard /usr/bin/umps2),)
    UMPS2_DIR_PREFIX = /usr
else
    UMPS2_DIR_PREFIX = /usr/local
endif

UMPS2_DATA_DIR = $(UMPS2_DIR_PREFIX)/share/umps2
UMPS2_INCLUDE_DIR = $(UMPS2_DIR_PREFIX)/include/umps2

# Compiler options
#CFLAGS_LANG = -ffreestanding -ansi
#CFLAGS_MIPS = -mips1 -mabi=32 -mno-gpopt -G 0 -mno-abicalls -fno-pic -mfp32
CFLAGS_MIPS = -mips1 -mfp32
#CFLAGS = $(CFLAGS_LANG) $(CFLAGS_MIPS) -I$(UMPS2_INCLUDE_DIR) -I inc/ -Wall -O0
CFLAGS = $(CFLAGS_MIPS) -I$(UMPS2_INCLUDE_DIR) -I inc/ -Wall -O0

# Linker options
LDFLAGS = -G 0 -nostdlib -T $(UMPS2_DATA_DIR)/umpscore.ldscript

# Add the location of crt*.S to the search path
VPATH = $(UMPS2_DATA_DIR)

.PHONY : all clean

all : kernel.core.umps

kernel.core.umps : kernel
	umps2-elf2umps -k $<

#kernel : test/obj/p1test_rikaya_v0.o obj/pcb.o crtso.o libumps.o
kernel : obj/p1.5test_rikaya_v0.o obj/init.o obj/handler.o obj/scheduler.o obj/pcb.o crtso.o libumps.o
	$(LD) -o $@ $^ $(LDFLAGS)

#obj/p1.5test_rikaya_v0.o : test/p1.5test_rikaya_v0.c
#	$(CC) $(CFLAGS) -c test/p1.5test_rikaya_v0.c -o obj/p1.5test_rikaya_v0.o

obj/%.o : src/%.c
	$(CC) $(CFLAGS) -c -o $@ $<

clean :
	-rm -f *.o kernel kernel.*.umps

# Pattern rule for assembly modules
%.o : %.S
	$(CC) $(CFLAGS) -c -o $@ $<



