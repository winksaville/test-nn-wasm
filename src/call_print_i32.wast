(module
  (func $prtI32 (import "importsForPrintI32" "print_i32") (param i32))
  (func (export "call_print_i32")
    i32.const 47
    call $prtI32))
