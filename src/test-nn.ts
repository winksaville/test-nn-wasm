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

let trainXorNn: (epoch_count: number, error_threshold: number,
    sr1: number, sr2: number, sr3: number, sr4: number) => number;
let getError: () => number;
let getEpochs: () => number;

async function load_wasm_imports(): Promise<Error | null> {
    try {
        let instance = await instantiateWasmFile("./out/lib/libwasm.c.wasm",
            importsForInstance);
        trainXorNn = instance.exports.trainXorNn;
        getError = instance.exports.getError;
        getEpochs = instance.exports.getEpochs;
        return Promise.resolve(null);
    } catch (err) {
        return Promise.reject(err);
    }
}

async function main() {
    try {
        await load_wasm_imports();
        let epoch_count = 10000000;
        let error_threshold = 0.0;
        let sr1 = 1;
        let sr2 = 2;
        let sr3 = 3;
        let sr4 = 4;
        console.log(`trainXorNn(${epoch_count}, ${error_threshold}, ${sr1}, ${sr2}, ${sr3}, ${sr4})=`
        + `${trainXorNn(epoch_count, error_threshold, sr1, sr2, sr3, sr4)}`);
        console.log(`  error=${getError()} epochs=${getEpochs()}`);
    } catch(err) {
        console.log(`err=${err}`);
    }
}

main();
