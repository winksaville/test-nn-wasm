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

let rand0_1: () => number;
let srand0_1: (v1: number, v2: number, v3: number, v4: number) => void;

async function load_wasm_imports(): Promise<Error | null> {
    try {
        let instance = await instantiateWasmFile("./out/lib/rand0_1.c.wasm",
            importsForInstance);
        rand0_1 = instance.exports.rand0_1;
        srand0_1 = instance.exports.srand0_1;
        return Promise.resolve(null);
    } catch (err) {
        return Promise.reject(err);
    }
}

async function main() {
    try {
        await load_wasm_imports();
        srand0_1(1, 2, 3, 4);
        for (let i = 0; i < 10; i++) {
            let v: number = rand0_1();
            console.log(`rand0_1=${v}`);
        }
    } catch(err) {
        console.log(`err=${err}`);
    }
}

main();
