.. meta::
   :description: oberon-lang the Oberon LLVM fronted.
   :keywords: oberon, LLVM, compiler

###########
oberon-lang
###########

*oberon-lang* is a compiler for the *Oberon* language family utilizing the *LLVM* compiler
infrastructure to target at wide variety of platforms. In general the the compiler should
be portable to host platforms supported by the *LLVM* project.

The current status is that it is very close to support the whole of *Oberon-07* language with
some notable missing parts:

* **BYTE** type.
* **CASE** statement.
* **PROCEDURE** type.
* **RECORD** extensions.

The exact status can be checked by inspecting the failing tests in the unittests.

There are some examples in the project test folder and the unit tests can be inspected
for basic usage specific features.

.. toctree::
   :maxdepth: 1
   :caption: Topics
   :hidden:

   self
   src/Installation
   src/GettingStarted
   src/CommandLineInterface
   src/FAQ

##################
Indices and tables
##################

* :ref:`genindex`
* :ref:`search`