# Arithmetic integer calculator

## Octal Hex Decimal CALCulator

Project was inspired by [sencalc](http://www.softpedia.com/get/Science-CAD/SenCalc.shtml). This calculator is intended to be cross-platform with some extra features, compared to its predecessor.

Calculator supports evaluating of expressions, mixing decimal, octal, hexadecimal, character and binary numbers like:

`101i * (2fh & 3o) - 10 + "32\nf"`

Base numbers are set according to preffixes or suffixes, listed in the following table:

| Base | Prefixes | Suffixes |
| ---- | ------- | ------ |
|Binary|         | i, b   |
|Octal | 0       | o      |
|Hex   | 0x      | h      |

ASCII (char) input should be enclosed with double quotes. Ascii supports specials symbols, like: *\r \n \t*. For representing '\\' and '"' characters, use '\\\\' and '\\"' accordingly.

Decimal numbers are used by default and don't have any distinguishing features, like suffix or prefix. Calculator is case-insensetive, hence both 10h and 10H mean the same number.

Project consists of library (ohbcalc), console application (cohbcalc) and gui application (gohbcalc)

## Projects
### ohbcalc
Core library, providing all the calculations.

### cohbcalc
Console application, that uses ohbcalc library for calculating. Result is shown in bases: decimal, hexadecimal, octal and binary.

### gohbcalc
Gui application, similar to cohbcalc. Output is the same as in cohbcalc application, but with the character one.

## Requirements
### ohbcalc
Compiler with c++14 support.

### cohbcalc
Compiler with c++17 support.
Supported systems: Windows, Linux

### gohbcalc
Compiler with c++14 support, Qt5.

## Generating project
Project supports only cmake 3.0+ build.
```sh
$ mkdir build
$ cd build
$ cmake ..
```

## Testing project
```sh
$ cmake --build . --target check
```

### Building project
```sh
$ cmake --build .
```
