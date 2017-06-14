# Makefile for test-nn
# Parameters:
#   DBG=0 or 1 (default = 0)

# Remove builtin suffix rules
#.SUFFIXES:

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
dstDir=$(outDir)/$(srcDir)

# Make the depDir and dstDirs
$(shell mkdir -p $(depDir) >/dev/null)
$(shell mkdir -p $(libDstDir) >/dev/null)

cc.wasm=$(HOME)/prgs/llvmwasm-builder/dist/bin/clang
llc.wasm=$(HOME)/prgs/llvmwasm-builder/dist/bin/llc
s2wasm=$(HOME)/prgs/llvmwasm-builder/dist/bin/s2wasm
wast2wasm=$(HOME)/prgs/llvmwasm-builder/dist/bin/wast2wasm
wasm2wast=$(HOME)/prgs/llvmwasm-builder/dist/bin/wasm2wast
wasm-link=$(HOME)/prgs/llvmwasm-builder/dist/bin/wasm-link

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

# native suffix rules
$(libDstDir)/%.o: $(libDir)/%.c $(depDir)/%.d
	$(COMPILE.c) -o $@ $<
	$(POSTCOMPILE)

$(outDir)/%.o: $(srcDir)/%.c
	$(COMPILE.c) -o $@ $<
	$(POSTCOMPILE)

$(depDir)/%.d: ;
.PRECIOUS: $(depDir)/%.d

# wasm suffix rules
$(dstDir)/%.c.bc: $(srcDir)/%.c
	@mkdir -p $(@D)
	$(cc.wasm) -emit-llvm --target=wasm32 -Weverything -Oz $< -c -o $@

$(dstDir)/%.c.s: $(dstDir)/%.c.bc
	$(llc.wasm) -asm-verbose=false $< -o $@

.PRECIOUS: $(dstDir)/%.c.wast
$(dstDir)/%.c.wast: $(dstDir)/%.c.s
	$(s2wasm) --import-memory $< -o $@

$(dstDir)/%.c.wasm: $(dstDir)/%.c.wast
	$(wast2wasm) $< -o $@
	
LIBSRCS= \
	  $(libDir)/NeuralNet.c \
	  $(libDir)/NeuralNetIo.c \
	  $(libDir)/rand0_1.c

all: $(outDir)/test-nn build.wasm

include $(wildcard $(patsubst %,$(DEPDIR)/%.d,$(basename $(LIBSRCS))))

$(outDir)/test-nn : $(LIBOBJS) $(outDir)/test-nn.o
	$(LNK) $(LIBOBJS) $(outDir)/test-nn.o $(LNKFLAGS) -o $@
	$(OD) $(ODFLAGS) $@ > $@.asm

test: $(outDir)/test-nn
	$(outDir)/test-nn $(P1)

build.wasm: $(dstDir)/call_print_i32.c.wasm

clean :
	@rm -rf $(outDir) $(depDir)
