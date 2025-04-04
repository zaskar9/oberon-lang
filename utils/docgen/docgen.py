#!/usr/bin/env -S python
# Simple parser to output Sphinx documentation
# Author : Runar Tenfjord
# TODO : replace by real C++ parser when this is stable
import sys
import argparse
from collections import namedtuple
from enum import Enum
import re

Module = namedtuple('Module', ['name', 'entries', 'doc'])

Span = namedtuple('Span', ['start', 'end'])

Token = namedtuple('Token', ['type', 'value', 'span'])

Type = Enum('Type',[
    'KEYWORD', 'TYPE', 'BUILTIN','IDENTIFIER', 'CLINE', 'CSTART', 'CEND', 'DSTART', 'DEND',
    'OPERATOR', 'STRING', 'CCHAR', 'NUMBER', 'LPAR', 'RPAR', 'LBRA', 'RBRA', 'COLON',
    'COM', 'SEMI', 'WHITESPACE', 'NEWLINE', 'MISMATCH'
])

EntryType = Enum('EntryType',['PROCEDURE', 'VAR', 'CONST', 'TYPE', 'IMPORT', 'COMMENT'])
Entry = namedtuple('Entry', ['type', 'name',  'span', 'doc', 'ref'])

keywords = {
    'AND','ARRAY','BEGIN','CASE','CONST','DEFINITION','DIV','ELSE','ELSIF','END','EXPORT','FROM','IF','IMPLEMENTATION',
    'IMPORT','MODULE','MOD','NOT','OF','OR','POINTER','PROCEDURE','RECORD','QUALIFIED','RETURN','SET','THEN','TYPE','VAR',
    'WITH','EXIT','FOR','LOOP','REPEAT','UNTIL','WHILE','TRUE','FALSE','NIL'
}

types = {
    'INTEGER','LONGINT','SHORTINT','BOOLEAN','REAL','LONGREAL','CHAR','SET'
}

builtins = {
    'NEW','DISPOSE','INC','DEC','INCL','EXCL','HALT','ABS','CAP','CHR','FLOAT','HIGH','LFLOAT','LTRUNC','MIN','MAX','ODD','SFLOAT',
    'STRUNC','TRUNC','VAL','IM','INT','LENGTH','ODD','RE'
}

operators = {
    'IN', 'IS', 'DIV', 'MOD', 'AND','OR', 'XOR'
}

patterns = [
    (Type.IDENTIFIER,   r'[_a-zA-Z][_a-zA-Z0-9]*'),
    (Type.CLINE,       r'\-\-'),
    (Type.CSTART,       r'(\(\*)'),
    (Type.CEND,         r'\*\)'),
    (Type.DSTART,       r'<\*'),
    (Type.DEND,         r'\*>'),
    (Type.OPERATOR,     r'(:=|:|\.|\+|\-|\*|/|\^|~|#|&|(<=)|<|(>=)|>|=)'),
    (Type.STRING,       r"""".*?"|'.*?'"""),
    (Type.CCHAR,        r"([0-7]+C)|(0[A-F0-9]*X)"),
    (Type.NUMBER,       r"(0[A-F0-9]*H)|(\d*(\.)?\d+((E|D([-+])?)?\d+)?)|(\d+)"),
    (Type.LPAR,         r'\('),
    (Type.RPAR,         r'\)'),
    (Type.LBRA,         r'\['),
    (Type.RBRA,         r'\]'),
    (Type.COLON,        r':'),
    (Type.COM,          r','),
    (Type.SEMI,         r';'),
    (Type.WHITESPACE,   r'[ \t]+'),
    (Type.NEWLINE,      r'\n'),
    (Type.MISMATCH,     r'.' )        
]
regex = re.compile('|'.join('(?P<%s>%s)' % (type.name, pattern) for type, pattern in patterns))

def parse(st):
    def tokens(text):
        for match in re.finditer(regex, text):
            if match.lastgroup:
                type = Type[match.lastgroup]
                if type == Type.WHITESPACE:
                    continue
                elif type == Type.IDENTIFIER:
                    if match[0] in keywords:
                        type = Type.KEYWORD
                    elif match[0] in types:
                        type = Type.TYPE
                    elif match[0] in builtins:
                        type = Type.BUILTIN
                    elif match[0] in operators:
                        type = Type.OPERATOR
                yield Token(type, match[0], Span(*match.span(0)))
    
    iter = tokens(st)
    start = None
    module = None
    name = None
    mtype = None
    comment = None

    def skip(current):
        comment = None
        clevel = 0
        while True:
            if current.type == Type.NEWLINE:
                current = next(iter)
                continue
            elif current.type == Type.CLINE:
                comment = None
                start = current.span.start
                while current.type != Type.NEWLINE:
                    current = next(iter)
                comment = Entry(EntryType.COMMENT, '', Span(start, current.span.end), None, None)
                current = next(iter)
                continue
            elif current.type == Type.CSTART:
                comment = None
                clevel = 1
                start = current.span.start
                current = next(iter)
                while clevel > 0:
                    if current.type == Type.CSTART:
                        clevel += 1
                    elif current.type == Type.CEND:
                        clevel -= 1
                    else:
                        pass
                    current = next(iter)
                comment = Entry(EntryType.COMMENT, '', Span(start, current.span.end), None, None)
                current = next(iter)
                continue
            elif current.type == Type.DSTART:
                while current.type != Type.DEND:
                    current = next(iter)
                current = next(iter)
                continue
            break
        return current, comment
    
    try:
        current = next(iter)
        while True:
            current, comment = skip(current)
            if current.type == Type.KEYWORD:
                if module is None:
                    if current.value == 'MODULE':
                        current = next(iter)
                        if current.type == Type.IDENTIFIER:
                            module = Module(current.value, [], comment)
                            comment = None
                            current = next(iter)
                        continue
                else:
                    if current.value == "END":
                        current = next(iter)
                        if current.type == Type.IDENTIFIER and current.value == module.name:
                            current = next(iter)
                            if current.type == Type.OPERATOR and current.value == '.':
                                current = next(iter)
                            break

                    elif current.value == 'IMPORT':
                        while True:
                            if current.type != Type.IDENTIFIER:
                                break
                            module.entries.append(Entry(EntryType.IMPORT, current.value, current.span, name, None))
                            current = next(iter)
                            if current.type == Type.COM:
                                current = next(iter)
                        comment = None

                    elif current.value == 'CONST':
                        current = next(iter)
                        while True:
                            current, comment = skip(current)
                            if current.type != Type.IDENTIFIER:
                                break
                            start = current.span.start
                            name = current.value
                            current = next(iter)

                            export = False
                            if current.value == '*':
                                export = True
                                current = next(iter)

                            if current.type != Type.OPERATOR or current.value != '=':
                                break
                            current = next(iter)
                            while True:
                                if current.type == Type.SEMI:
                                    break
                                current = next(iter)
                            
                            if export:
                                module.entries.append(Entry(EntryType.CONST, name, Span(start, current.span.end), None, None))
                            current = next(iter)

                        comment = None
                        continue
                    
                    elif current.value == 'TYPE':
                        current = next(iter)
                        while True:
                            current, comment = skip(current)
                            if current.type != Type.IDENTIFIER and current.type != Type.TYPE:
                                break
                            start = current.span.start
                            name = current.value
                            current = next(iter)

                            export = False
                            if current.value == '*':
                                export = True
                                current = next(iter)

                            if current.type != Type.OPERATOR or current.value != '=':
                                break
                            isRecord = False
                            current = next(iter)
                            while True:
                                if current.type == Type.KEYWORD:
                                    if current.value == 'RECORD':
                                        isRecord = True
                                    elif current.value == 'END' and isRecord:
                                        current = next(iter)
                                        break
                                elif current.type == Type.SEMI and not isRecord:
                                    break
                                current = next(iter)
                            
                            if export:
                                module.entries.append(Entry(EntryType.TYPE, name, Span(start, current.span.end), None, None))
                            current = next(iter)
                        comment = None
                        continue

                    elif current.value == 'VAR':
                        current = next(iter)
                        while True:
                            while current.type == Type.NEWLINE:
                                current = next(iter)
                            if current.type != Type.IDENTIFIER:
                                break
                            start = current.span.start
                            name = current.value
                            current = next(iter)
                            if current.type != Type.OPERATOR or current.value != ':':
                                break
                            current = next(iter)
                            while True:
                                if current.type == Type.SEMI:
                                    break
                                current = next(iter)
                            module.entries.append(Entry(EntryType.VAR, name, Span(start, current.span.end), None, None))
                            current = next(iter)
                        comment = None
                        continue

                    elif current.value == 'PROCEDURE':
                        start = current.span.start
                        current = next(iter)
                        
                        # Skip language definition
                        if current.type == Type.LBRA:
                            current = next(iter)
                            while True:
                                if current.type == Type.RBRA:
                                    break
                                current = next(iter)
                            current = next(iter)
                        
                        # Receiver
                        ref = None
                        if current.type == Type.LPAR:
                            current = next(iter)
                            if current.type == Type.KEYWORD and current.value == "VAR":
                                current = next(iter)

                            if current.type != Type.IDENTIFIER:
                                continue
                            
                            current = next(iter)
                            if current.type != Type.OPERATOR or current.value != ':':
                                continue
                            
                            current = next(iter)
                            if current.type != Type.IDENTIFIER:
                                continue
                            ref = current.value

                            current = next(iter)
                            if current.type != Type.RPAR:
                                continue
                            current = next(iter)

                        # Expect identifier
                        if current.type != Type.IDENTIFIER:
                            continue
                        name = current.value
                        current = next(iter)
                        
                        export = False
                        if current.value == '*':
                            export = True
                            current = next(iter)
                        
                        # Skip argument definition if exists
                        if current.type == Type.LPAR:
                            while True:
                                if current.type == Type.RPAR:
                                    break;
                                current = next(iter)

                        # Procedure end with semicolon
                        while True:
                            if current.type == Type.SEMI:
                                break;
                            current = next(iter)
                        current = next(iter)
                        end = current.span.end

                        # Check if EXTERN
                        if current.value == 'EXTERN':
                            current = next(iter)
                            if current.type == Type.SEMI:
                                current = next(iter)
                            if export:
                                module.entries.append(Entry(EntryType.PROCEDURE, name, Span(start, end), comment, ref))
                                comment = None
                        else:
                            while True:
                                # Search for matching end
                                if current.type == Type.KEYWORD and current.value == 'END':
                                    current = next(iter)
                                    if current.type == Type.IDENTIFIER and current.value == name:
                                        current = next(iter)
                                        if current.type == Type.SEMI:
                                            current = next(iter)
                                        if export:
                                            module.entries.append(Entry(EntryType.PROCEDURE, name, Span(start, end), comment, ref))
                                        comment = None
                                        current = next(iter)
                                        break
                                current = next(iter)
            current = next(iter)
    except StopIteration:
        pass
    
    try:
        current = next(iter)
        if current.type == Type.DSTART:
            while current.type != Type.DEND:
                current = next(iter)
            current = next(iter)
    except StopIteration:
        pass

    return module, current.span.end

def main():
    parser = argparse.ArgumentParser()
    parser.add_argument("inputfile")
    parser.add_argument("-o", "--output", type=str, default = '')
    args = parser.parse_args()
    st = open(args.inputfile).read()

    if not args.output:
        fh = sys.stdout
    else:
        print(args.output)
        fh = open(args.output, 'w')
    
    def ref(module, ref, name):
        if not ref:
            print('.. _%s.%s:' % (module, name), file = fh)
        else:
            print('.. _%s.%s.%s:' % (module, ref, name), file = fh)
        print(file = fh)

    def heading(title, ch = '=', overline = False):
        if overline : print(ch * len(title), file = fh)
        print(title, file = fh)
        print(ch * len(title), file = fh)
        print(file = fh)
    
    def comment(span):
        print(st[span.start:span.end].strip().lstrip('(*').rstrip('*)'), file = fh)
        print(file = fh)

    def code(span):
        print('.. code-block:: modula2', file = fh)
        print(file = fh)
        for line in st[span.start:span.end].splitlines():
            print('    ' + line, file = fh)
        print(file = fh)
    
    module, end = parse(st)
    epilog = st[end:].strip()

    def filtered(type):
        return tuple(filter(lambda item : item.type == type, module.entries))

    # Index & Reference
    print('.. index::', file = fh)
    print('    single: %s' % module.name, file = fh)
    print(file = fh)

    print('.. _%s:' % module.name, file = fh)
    print(file = fh)

    # Heading
    heading("%s" % module.name, '*', True)
    if not module.doc is None:
        comment(module.doc.span)

    # Const
    const = filtered(EntryType.CONST)
    if const:
        heading("Const", '=')
        for item in const:
            code(item.span)
            
    # Types
    type = filtered(EntryType.TYPE)
    if type:
        heading("Types", '=')
        for item in type:
            code(item.span)
    
    # Vars
    var = filtered(EntryType.VAR)
    if var:
        heading("Vars", '=')
        for item in var:
            code(item.span)

    # Procedures
    proc = filtered(EntryType.PROCEDURE)
    if proc:
        heading("Procedures", '=')
        for item in proc:
            ref(module.name, item.ref, item.name)
            if item.ref:
                heading("%s.%s" % (item.ref, item.name), '-')
            else:
                heading(item.name, '-')

            if not item.doc is None:
                comment(item.doc.span)
            code(item.span)
    
    # Possibly add epilog as example
    if len(epilog):
        print(file = fh)
        heading("Example", '=')
        print(epilog, file = fh)
        print(file = fh)
if __name__ == '__main__':
    main()