<p align="center">
  <a href="https://github.com/zaskar9/oberon-lang/actions"><img src="https://github.com/zaskar9/oberon-lang/actions/workflows/linux.yml/badge.svg?branch=master" alt="Github Actions Badge"></a>
  <a href="https://github.com/zaskar9/oberon-lang/actions"><img src="https://github.com/zaskar9/oberon-lang/actions/workflows/macos.yml/badge.svg?branch=master" alt="Github Actions Badge"></a>
  <a href="https://github.com/zaskar9/oberon-lang/actions"><img src="https://github.com/zaskar9/oberon-lang/actions/workflows/msys2.yml/badge.svg?branch=master" alt="Github Actions Badge"></a>
  <a href="https://github.com/zaskar9/oberon-lang/actions"><img src="https://github.com/zaskar9/oberon-lang/actions/workflows/docs.yml/badge.svg?branch=master" alt="Github Actions Badge"></a>
</p>

# A Compiler for the Oberon Programming Language

The [Oberon](https://www.ethoberon.ethz.ch) programming language was proposed in 1987 by 
[Niklaus Wirth](https://people.inf.ethz.ch/wirth/) as a successor to Pascal and Modula-2. Due to this lineage, Oberon 
is an ALGOL-like language with strong (static and dynamic) typing discipline. The programming paradigm of Oberon can 
be classified as imperative, structured, modular, and object-oriented.

## About

This project implements a compiler for the Oberon programming language as a frontend to the [LLVM](http://llvm.org)
Compiler Infrastructure. It is written in C++ and originated as project accompanying the MSc course "Compiler 
Construction" taught at the [University of Konstanz](https://uni.kn). As a consequence, this compiler originally only
targeted the Oberon-0 subset of the language, as described in Niklaus Wirth's book 
["Compiler Construction"](http://www.ethoberon.ethz.ch/WirthPubl/CBEAll.pdf) (Chapter 6, pp. 30-32). Since then, the
supported subset of the Oberon has been continuously extended with the goal to eventually cover the full language 
specification as described in the latest version of the [Oberon Language Report](https://inf.ethz.ch/personal/wirth/Oberon/Oberon07.Report.pdf).
In addition to these "official" extensions, other features were added to the supported dialect of the Oberon programming language.
These feature were either inspired by convenience, such as interfacing with standard libraries, or by the compiler 
author's nostalgia of learning [Turbo Pascal](https://en.wikipedia.org/wiki/Turbo_Pascal) 6.0 as his first programming 
language at high school. A description of the currently supported Oberon dialect in terms of syntax and semantics (of
unofficial features) can be found in the Wiki section of this project repository.

## Dependencies and Toolchains

Owing to its origin as a course project, care has been taken that the provided C++ sourcecode can be compiled on
different operating systems and with different toolchains. Currently the sourcecode only depends on 
[Boost](https://www.boost.org) and [LLVM](https://llvm.org).
As of August 2023, the following configurations are tested and known to work.

|       | macOS             | Windows        | MSYS2 (CLANG64)   | Linux (Ubuntu) |
|-------|-------------------|----------------|-------------------|----------------|
| Boost | 1.83.0            | 1.83.0         | 1.85.0            | 1.74.0         |
| LLVM  | 17.0.6            | 17.0.2         | 18.1.4            | 17.0.0         |
| CMake | 3.28.0            | 3.27.0         | 3.29.3            | 3.26.0         |
| CXX   | LLVM Clang 17.0.6 | CL 19.38.33133 | LLVM Clang 18.1.4 | GCC 11.4.0     |

For macOS and Linux, both the x64 and the arm64 version have been tested. Full support for Windows (arm64) is currently
under development.
Detailed instruction on how to build the Oberon compiler and its dependencies can be found in the Wiki section of this
project repository.

## Getting started
See the [Documentation](https://zaskar9.github.io/oberon-lang/) pages.