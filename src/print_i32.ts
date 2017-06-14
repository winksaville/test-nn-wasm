import {instantiateWasmFile} from "../build/utils";

/**
 * The import object passed to instantiateWasmFile
 */
let importsForInstance = {
    env: {
        print_i32: (arg: number) => {
            console.log(`print_i32: arg=${arg}`);
        },
        memory: new WebAssembly.Memory({initial:1})
    }
};

let call_print_i32: () => void;

async function load_wasm_imports(): Promise<Error | null> {
    try {
        let instance = await instantiateWasmFile("./out/src/call_print_i32.c.wasm",
            importsForInstance);
        call_print_i32 = instance.exports.call_print_i32;
        return Promise.resolve(null);
    } catch (err) {
        return Promise.reject(err);
    }
}

async function main() {
    try {
        let x = await load_wasm_imports();
        console.log(`x=${x}`);

        console.log("invoke call_print_i32");
        call_print_i32();
        console.log("invoked call_print_i32");
    } catch(err) {
        console.log(`err=${err}`);
    }
}

main();
