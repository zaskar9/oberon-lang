.. index::
    single: GettingsStarted

.. _GettingsStarted:

***************
GettingsStarted
***************

The well known HelloWorld program in *Oberon*:

.. code-block:: modula2

    MODULE HelloWorld;
    IMPORT Out;

    CONST Message = "Hoi Niklaus!";

    BEGIN
        Out.String(Message); Out.Ln
    END HelloWorld.

This can be run directly by the oberon-lang compiler:

.. code-block:: shell

   oberon-lang -I/usr/include/oberon-lang -L/usr/lib -loberon -r hello.mod

Here it is assumed the ".smb" symbol files is installed in "/usr/include/oberon-lang"
and the link libraries for liboberon is installed in "/usr/lib". This might need to be
adjusted according to where these files are actually installed.

The "-r" flag selects the JIT mode and execute a single module.

In order to actually bild a executeable we first need to build a object file which
is linked by the system compiler.

Build object file:

.. code-block:: shell

   oberon-lang -I/usr/include/oberon-lang -fenable-main hello.mod

The *Oberon* language does not have the concept of a main module and in order to
execute the HelloWorld module we use the "-fenable-main" option. The oberon-lang
compiler also need to find the ".smb" symbol file for the Out module.
The above command line should create an object file "hello.o" if there are no
errors.

Link object file to executable:

.. code-block:: shell

   clang hello.o -L/usr/lib -loberon -ohello.exe

Here the *clang* compiler is used and this command line will by defaul link with
the system libc standard library need for the output functions in liboberon.
Other compilers might need additional options in order to link with the needed libraries.

Further command line flags is documented in :ref:`CommandLineInterface`