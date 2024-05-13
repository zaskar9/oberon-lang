.. index::
    single: Installation

.. _Installation:

************
Installation
************

General
=======

Currently the project must be built from source as it is still to
regarded as under heavy development and changes are performed
frequently.

The main project dependencies are *LLVM*, *BOOST* and *CMAKE*.
A fairly recent C/C++ compiler supporting the *C++20* and *C11* standards.
Optional dependencies are the *lit* and *filecheck* tools for running
the unittests and the *Sphinx* for building the documentation.

Build archives are available on the Github actions page. Note that you need
to be logged in into Github to access this. These executables could need further
library dependencies as dictated by the build environments. Please check the
Github Action scripts for the current setups steps to compile the project
for supported environments.
