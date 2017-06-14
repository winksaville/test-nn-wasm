# Makefile for test-nn
# Parameters:
#   DBG=0 or 1 (default = 0)

# Remove builtin suffix rules
.SUFFIXES:

# _DBG will be 0 if DBG isn't defined on the command line
_DBG = +$(DBG)
ifeq ($(_DBG), +)
  _DBG = 0
endif

P1=10000000

depDir=.d
outDir=out
srcDir=src
libDir=lib
incDir=inc
libDstDir=$(outDir)/$(libDir)

# Make the depDir and dstDirs
$(shell mkdir -p $(depDir) >/dev/null)
$(shell mkdir -p $(libDstDir) >/dev/null)

CC=clang
CFLAGS=-O3 -g -Weverything -Werror -I$(incDir) -DDBG=$(_DBG)
DEPFLAGS = -MT $@ -MMD -MP -MF $(depDir)/$*.Td

OD=objdump
ODFLAGS=-S -M x86_64,intel

LNK=$(CC)
LNKFLAGS=-lm

COMPILE.c = $(CC) $(DEPFLAGS) $(CFLAGS) $(CPPFLAGS) $(TARGET_ARCH) -c
POSTCOMPILE = @mv -f $(depDir)/$*.Td $(depDir)/$*.d && touch $@

LIBOBJS=$(libDstDir)/NeuralNet.o $(libDstDir)/NeuralNetIo.o \
		$(libDstDir)/rand0_1.o

# My suffix rules
$(libDstDir)/%.o: $(libDir)/%.c $(depDir)/%.d
	$(COMPILE.c) -o $@ $<
	$(POSTCOMPILE)

$(outDir)/%.o: $(srcDir)/%.c
	$(COMPILE.c) -o $@ $<
	$(POSTCOMPILE)

$(depDir)/%.d: ;
.PRECIOUS: $(depDir)/%.d

LIBSRCS= \
	  $(libDir)/NeuralNet.c \
	  $(libDir)/NeuralNetIo.c \
	  $(libDir)/rand0_1.c

all: $(outDir)/test-nn

include $(wildcard $(patsubst %,$(DEPDIR)/%.d,$(basename $(LIBSRCS))))

$(outDir)/test-nn : $(LIBOBJS) $(outDir)/test-nn.o
	$(LNK) $(LIBOBJS) $(outDir)/test-nn.o $(LNKFLAGS) -o $@
	$(OD) $(ODFLAGS) $@ > $@.asm

test: $(outDir)/test-nn
	$(outDir)/test-nn $(P1)

clean :
	@rm -rf $(outDir) $(depDir)
