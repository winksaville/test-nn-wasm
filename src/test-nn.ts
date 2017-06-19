/*
 * Copyright 2017 Wink Saville
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */
import {instantiateWasmFile} from "../build/utils";
import * as  microtime from "microtime";
import * as  numeral from "numeral";

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

        let start_sec: number = microtime.nowDouble();
        let status = trainXorNn(epoch_count, error_threshold, sr1, sr2, sr3, sr4);
        let end_sec: number = microtime.nowDouble();

        let epoch = getEpochs();
        let error = getError();
        let time_sec = end_sec - start_sec
        let eps = epoch / time_sec;

        console.log(`Epoch=${numeral(epoch).format("0,0")}`
            + ` error=${numeral(error).format("0.00e+0")}`
            + ` status=${numeral(status).format("0,0")}`
            + ` time=${numeral(time_sec).format("0.00")}s`
            + ` eps=${numeral(eps).format("0,0")}`
        );
    } catch(err) {
        console.log(`err=${err}`);
    }
}

main();
