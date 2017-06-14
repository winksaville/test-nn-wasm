# An artificial neural network library targeting WebAssembly

This is based on [this commit in test-nn-c](https://github.com/winksaville/test-nn-c/commit/5ca27097024ce857041b5a9773a86a91617724a0)
which is from branch Make-compatible-with-test-nn-ts. The code is originally
based on the [Step by Step Guide to Implementing a Neural Network in C](http://www.cs.bham.ac.uk/~jxb/INC/nn.html)
by [John A. Bullinaria](http://www.cs.bham.ac.uk/~jxb/).

# Prerequistes
- clang
- objdump
- node version ^8.1.0 as wasm is needed
- yarn
- [llvmwasm-builder](https://github.com/winksaville/llvmwasm-builder) installed at ../llvmwasm-builder

# Install
```
$ yarn
yarn install v0.24.6
[1/4] Resolving packages...
[2/4] Fetching packages...
[3/4] Linking dependencies...
[4/4] Building fresh packages...
$ yarn postcleanup
yarn postcleanup v0.24.6
```

# Run simple wasm test
```
$ yarn test
yarn test v0.24.6
$ make build.wasm && yarn build:tsc
/home/wink/prgs/llvmwasm-builder/dist/bin/clang -emit-llvm --target=wasm32 -Weverything -Oz src/call_print_i32.c -c -o out/src/call_print_i32.c.bc
/home/wink/prgs/llvmwasm-builder/dist/bin/llc -asm-verbose=false out/src/call_print_i32.c.bc -o out/src/call_print_i32.c.s
/home/wink/prgs/llvmwasm-builder/dist/bin/s2wasm --import-memory out/src/call_print_i32.c.s -o out/src/call_print_i32.c.wast
/home/wink/prgs/llvmwasm-builder/dist/bin/wast2wasm out/src/call_print_i32.c.wast -o out/src/call_print_i32.c.wasm
rm out/src/call_print_i32.c.s out/src/call_print_i32.c.bc
yarn build:tsc v0.24.6
$ tsc -p src/utils.tsconfig.json && tsc -p src/print_i32.tsconfig.json 
Done in 2.73s.
$ node --expose_wasm build/print_i32.js 
x=null
invoke call_print_i32
print_i32: arg=48
invoked call_print_i32
Done in 3.14s.
```

# Run native tsc-nn
```
$ make test
out/test-nn 10000000


Epoch=10,000,000 Error=4.58e-08 time=5.537s eps=1,805,983

Pat	Input0   	Input1   	Target0   	Output0   
0	0.000000	0.000000	0.000000	0.000142
1	1.000000	0.000000	1.000000	0.999857
2	0.000000	1.000000	1.000000	0.999857
3	1.000000	1.000000	0.000000	0.000175
```
