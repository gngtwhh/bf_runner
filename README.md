# bf_runner

A BrainF..k interpreter written in C language

# Usage

1. Compile the project:

 ```Bash
 $ mkdir build
 $ cd build
 $ cmake ..
 $ make
 ```

2. Just run the program with 3 arguments:

 ```Bash
 $ ./bf_runner.exe <file> [input] [output]
 ```
If you don't specify the input file, it will use stdin. 
Also, if you don't specify the output file, it will use stdout.

3. See the output file. :)

# Example

```Bash
$ ./bf_runner.exe bf_code.bf input.txt output.txt
```

The result in `output.txt` will be:
```text
Hello World!

```
