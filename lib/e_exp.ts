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

/**
 * The import object passed to instantiateWasmFile
 */
let importsForInstance = {
    env: {
      // Not needed as we're not using s2wasm flag '--import-memory'
      // memory: new WebAssembly.Memory({initial:1})
    }
};

let e_exp: (v: number) => number;

async function load_wasm_imports(): Promise<Error | null> {
    try {
        let instance = await instantiateWasmFile("./out/lib/e_exp.c.wasm",
            importsForInstance);
        e_exp = instance.exports.e_exp;
        return Promise.resolve(null);
    } catch (err) {
        return Promise.reject(err);
    }
}

async function main() {
    try {
        await load_wasm_imports();
        for (let i = 0; i < 10; i++) {
            let v: number = e_exp(i);
            console.log(`e_exp(${i})=${v}`);
        }
    } catch(err) {
        console.log(`err=${err}`);
    }
}

main();
