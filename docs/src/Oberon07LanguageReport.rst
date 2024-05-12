Oberon-07 Language Report 2016
==============================

Revision 1.10.2013 / 3.5.2016

Niklaus Wirth

*Make it as simple as possible, but not simpler. (A. Einstein)*

Introduction
------------

Oberon is a general-purpose programming language that evolved from Modula-2. Its principal new feature is the concept of type extension.
It permits the construction of new data types on the basis of existing ones and to relate them.

This report is not intended as a programmer's tutorial. It is intentionally kept concise. Its function is to serve as a reference for programmers,
implementors, and manual writers. What remains unsaid is mostly left so intentionally, either because it is derivable from stated rules of the language,
or because it would unnecessarily restrict the freedom of implementors.

This document describes the language defined in 1988/90 as revised in 2007 / 2016.

Syntax
------

A language is an infinite set of sentences, namely the sentences well formed according to its syntax. In Oberon, these sentences are called compilation units.
Each unit is a finite sequence of *symbols* from a finite vocabulary. The vocabulary of Oberon consists of identifiers, numbers, strings, operators, delimiters,
and comments. They are called *lexical symbols* and are composed of sequences of *characters*. (Note the distinction between *symbols* and *characters*.)

To describe the syntax, an extended Backus-Naur Formalism called EBNF is used. Brackets [ and ] denote optionality of the enclosed sentential form, and braces
{ and } denote its repetition (possibly 0 times). Syntactic entities (non-terminal symbols) are denoted by English words expressing their intuitive meaning.
Symbols of the language vocabulary (terminal symbols) are denoted by strings enclosed in quote marks or by words in capital letters.

Vocabulary
----------

The following lexical rules must be observed when composing symbols. Blanks and line breaks must not occur within symbols (except in comments, and blanks in strings).
They are ignored unless they are essential to separate two consecutive symbols. Capital and lower-case letters are considered as being distinct.

Identifiers are sequences of letters and digits. The first character must be a letter. ::

    ident  =  letter {letter | digit}
