# Makefile for test-nn
# The auto-dependency generation algorithm:
#    http://make.mad-scientist.net/papers/advanced-auto-dependency-generation/
# Parameters:
#   DBG=0 or 1 (default = 0)

# Remove builtin suffix rules
#.SUFFIXES:

# _DBG will be 0 if DBG isn't defined on the command line
_DBG = +$(DBG)
ifeq ($(_DBG), +)
  _DBG = 0
endif

# Default value for P1 parameter
P1=10000000

depDir=.d
outDir=out
srcDir=src
libDir=lib
incDir=inc
libDstDir=$(outDir)/$(libDir)
srcDstDir=$(outDir)/$(srcDir)

# Make the depDir and srcDstDirs
$(shell mkdir -p $(depDir) >/dev/null)
$(shell mkdir -p $(libDstDir) >/dev/null)

cc.wasm=$(HOME)/prgs/llvmwasm-builder/dist/bin/clang
llc.wasm=$(HOME)/prgs/llvmwasm-builder/dist/bin/llc
s2wasm=$(HOME)/prgs/llvmwasm-builder/dist/bin/s2wasm
wast2wasm=$(HOME)/prgs/llvmwasm-builder/dist/bin/wast2wasm
wasm2wast=$(HOME)/prgs/llvmwasm-builder/dist/bin/wasm2wast
wasm-link=$(HOME)/prgs/llvmwasm-builder/dist/bin/wasm-link

CC=clang
CFLAGS=-O3 -Weverything -Werror -std=c11 -I$(incDir) -DDBG=$(_DBG)
DEPFLAGS = -MT $@ -MMD -MP -MF $(depDir)/$*.Td

OD=objdump
ODFLAGS=-S -M x86_64,intel

LNK=$(CC)
LNKFLAGS=-lm

COMPILE.c = $(CC) $(DEPFLAGS) $(CFLAGS) $(CPPFLAGS) $(TARGET_ARCH) -g -c
POSTCOMPILE = @mv -f $(depDir)/$*.Td $(depDir)/$*.d && touch $@

# native suffix rules
$(libDstDir)/%.o: $(libDir)/%.c $(depDir)/%.d
	$(COMPILE.c) -o $@ $<
	$(POSTCOMPILE)

$(outDir)/%.o: $(srcDir)/%.c
	$(COMPILE.c) -o $@ $<
	$(POSTCOMPILE)

$(depDir)/%.d: ;
.PRECIOUS: $(depDir)/%.d

# wasm suffix rules for srcDir
$(srcDstDir)/%.c.bc: $(srcDir)/%.c
	@mkdir -p $(@D)
	$(cc.wasm) -emit-llvm --target=wasm32 $(CFLAGS) $< -c -o $@

$(srcDstDir)/%.c.s: $(srcDstDir)/%.c.bc
	$(llc.wasm) -asm-verbose=false $< -o $@

#S2WASMFLAGS=--import-memory
S2WASMFLAGS=
.PRECIOUS: $(srcDstDir)/%.c.wast
$(srcDstDir)/%.c.wast: $(srcDstDir)/%.c.s
	$(s2wasm) $(S2WASMFLAGS) $< -o $@

$(srcDstDir)/%.c.wasm: $(srcDstDir)/%.c.wast
	$(wast2wasm) $< -o $@

# wasm suffix rules for libDir
$(libDstDir)/%.c.bc: $(libDir)/%.c
	@mkdir -p $(@D)
	$(cc.wasm) -emit-llvm --target=wasm32 $(CFLAGS) $< -c -o $@

$(libDstDir)/%.c.s: $(libDstDir)/%.c.bc
	$(llc.wasm) -asm-verbose=false $< -o $@

.PRECIOUS: $(libDstDir)/%.c.wast
$(libDstDir)/%.c.wast: $(libDstDir)/%.c.s
	$(s2wasm) $(S2WASMFLAGS) $< -o $@

$(libDstDir)/%.c.wasm: $(libDstDir)/%.c.wast
	$(wast2wasm) $< -o $@
	
LIBSRCS= \
	  $(libDir)/NeuralNet.c \
	  $(libDir)/NeuralNetIo.c \
	  $(libDir)/xoroshiro128plus.c \
	  $(libDir)/rand0_1.c \
	  $(libDir)/e_exp.c \
	  $(libDir)/calloc.c \
	  $(libDir)/malloc.c \
	  $(libDir)/memset.c

LIBOBJS= \
	  $(libDstDir)/NeuralNet.o \
	  $(libDstDir)/NeuralNetIo.o \
	  $(libDstDir)/xoroshiro128plus.o \
	  $(libDstDir)/rand0_1.o \
	  $(libDstDir)/e_exp.o \
	  $(libDstDir)/calloc.o \
	  $(libDstDir)/malloc.o \
	  $(libDstDir)/memset.o

all: build.native build.wasm

build.native: $(outDir)/test-nn

include $(wildcard $(patsubst %,$(DEPDIR)/%.d,$(basename $(LIBSRCS))))

$(outDir)/test-nn : $(LIBOBJS) $(outDir)/test-nn.o
	$(LNK) $(LIBOBJS) $(outDir)/test-nn.o $(LNKFLAGS) -o $@
	$(OD) $(ODFLAGS) $@ > $@.asm

test: $(outDir)/test-nn
	$(outDir)/test-nn $(P1)

# Currently wasm-link doesn't work with modules that have a memory section
#build.wasm: \
# $(srcDstDir)/call_print_i32.c.wasm \
# $(libDstDir)/e_exp.c.wasm \
# $(libDstDir)/malloc.c.wasm \
# $(libDstDir)/calloc.c.wasm \
# $(libDstDir)/memset.c.wasm \
# $(libDstDir)/xoroshiro128plus.c.wasm \
# $(libDstDir)/rand0_1.c.wasm \
# $(libDstDir)/NeuralNet.c
#	$(wasm-link) $^ -o $(libDstDir)/libwasm.c.wasm

$(libDir)/libwasm.c: \
 $(libDir)/e_exp.c \
 $(libDir)/malloc.c \
 $(libDir)/calloc.c \
 $(libDir)/memset.c \
 $(libDir)/xoroshiro128plus.c \
 $(libDir)/rand0_1.c \
 $(libDir)/NeuralNet.c
	touch $(libDir)/libwasm.c

build.wasm: \
 $(libDir)/libwasm.c \
 $(libDstDir)/libwasm.c.wasm \
 $(srcDstDir)/call_print_i32.c.wasm

clean :
	@rm -rf $(outDir) $(depDir)
