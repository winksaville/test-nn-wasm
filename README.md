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

# Performance
As of 2018-06-18 the native code is about 1.28x faster than wasm:
```
1,430,000 / 1,120,000 = 1.28x
```

# Run native test-nn
```
$ make test-nn
out/test-nn 10000000


Epoch=10,000,000 error=4.58e-08 time=7.010s eps=1,426,570

Pat	Input0   	Input1   	Target0   	Output0   
0	0.000000	0.000000	0.000000	0.000142
1	1.000000	0.000000	1.000000	0.999857
2	0.000000	1.000000	1.000000	0.999857
3	1.000000	1.000000	0.000000	0.000175
```

# Run wasm test-nn
```
$ yarn test-nn
yarn test-nn v0.24.6
$ yarn build.wasm
yarn build.wasm v0.24.6
$ make build.wasm 
make: Nothing to be done for 'build.wasm'.
Done in 0.10s.
$ node build/test-nn.js 
Epoch=10,000,000 error=4.58e-8 status=0 time=8.92s eps=1,120,637
Done in 9.35s.
```
