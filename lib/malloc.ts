import {instantiateWasmFile} from "../build/utils";

/**
 * The import object passed to instantiateWasmFile
 */
let importsForInstance = {
    env: {
      // Not needed as we're not using s2wasm flag '--import-memory'
      // memory: new WebAssembly.Memory({initial:1})
    }
};

let malloc: (size: number) => number;
let free: (add: number) => void; 

async function load_wasm_imports(): Promise<Error | null> {
    try {
        let instance = await instantiateWasmFile("./out/lib/malloc.c.wasm",
            importsForInstance);
        malloc = instance.exports.malloc;
        free = instance.exports.free;
        return Promise.resolve(null);
    } catch (err) {
        return Promise.reject(err);
    }
}

async function main() {
    try {
        await load_wasm_imports();
        let addr1 = malloc(100);
        console.log(`addr1:${addr1} = malloc(100)`);
        free(addr1);
        console.log(`free(${addr1})`);
        let addr2 = malloc(10);
        console.log(`addr2:${addr2} = malloc(10)`);
        free(addr2);
        console.log(`free(addr2:${addr2})`);
    } catch(err) {
        console.log(`err=${err}`);
    }
}

main();
