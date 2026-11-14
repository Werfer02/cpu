# cpu (working name)

an attempt at a simple cpu emulator / vm with custom architecture, assembly, assembler etc.

## Building

```
cmake -S . -B build
```
```
cmake --build build
```

## Running

### run program from instructions file
```
build/out (path to instructions)
```
OR
```
build/out -I (path to instructions)
```
e.g.
```
build/out -I testprograms/testreturn.inst
```

### assemble program from instructions file
```
build/out -I (path to instructions file) -A (output binary path)
```
e.g.
```
build/out -I testprograms/testreturn.inst -B bin/testreturn.bin
```

### run program from binary file
```
build/out -B (path to binary file)
```
e.g.
```
build/out -B bin/testreturn.bin
```

### assemble AND run program from instructions file
```
build/out -I (path to instructions file) -A (output binary path) --run
```
e.g.
```
build/out -I testprograms/testreturn.inst -B bin/testreturn.bin --run
```

### optional arguments
#### `print-registers`
will print registers after running program
#### `print-heap-before`
will print heap before running program
#### `print-heap-after` and `print-heap`
will print heap after running program

e.g.
```
build/out -I testprograms/testreturn.inst --print-registers --print-heap
```

