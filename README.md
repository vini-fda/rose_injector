# ROSE-injector

This is an experimental tool to identify and substitute variables that are used in expressions in C code, effectively performing something which loosely resembles [Dependency Injection](https://en.wikipedia.org/wiki/Dependency_injection), written in C++ with the help of the ROSE compiler infrastructure. The main implementation is based on Matthew Sottile's code [here](https://github.com/rose-compiler/rose/issues/228).

## How to build

Make sure you have [ROSE](https://github.com/rose-compiler/rose) installed, as well as a C++ compiler and Make. Then, just run `make` and the main binary, `rose_injector`, will be built in the `target/` directory.
