.. index::
    single: CommandLineInterface

.. _CommandLineInterface:

**********************
Command Line Interface
**********************

.. rubric:: SYNOPSIS

.. code-block:: shell

    oberon-lang -h|--help
    oberon-lang --version
    oberon-lang [-v|--verbose] [-q|--quiet] [-I path] [-L path] [-l library,...] [-f flag,...] [-O level] [-o name] [-r|--run] module ...

.. rubric:: DESCRIPTION

*oberon-lang* is a compiler for the *Oberon* language family utilizing the *LLVM* compiler
infrastructure to target at wide variety of platforms. By default it built one or several
supplied modules to object files. With the "[-r|--run]" flag set it executes directly a
single module. In order to create and executable a single module must be marked as the
main module with the "[-f enable-main]" flag.

.. rubric:: OPTIONS

-h, --help          Show help message and exit.
--version           Show version information and exit.
-v, --verbose       Turn on debugging outputs.
-q, --quiet         Suppress all compiler outputs.
-I <path>           Search paths for symbol files.
-L <path>           Search paths for libraries.
-l <library>        Static or dynamic library.
-f <flag>           Compiler configuration flags:

                    * **sym-dir** : Set output path for generated .smb files.
                    * **filetype** : Set type of output file. [asm, bc, obj, ll]
                    * **reloc** : Set relocation model. [default, static, pic]
                    * **target** : Target triple for cross compilation.

-O level            Optimization level. [**O0, O1, O2, O3**]
-o name             Name of the output file.
-r, --run           Run with LLVM JIT.

.. rubric:: AUTHOR

Michael Grossniklaus

.. rubric:: COPYRIGHT

MIT License

Copyright (c) 2017 Michael Grossniklaus
