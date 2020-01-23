   .data
_msg:
   .ascii  "Hello, world!\n"

   .text
.globl _main
_main:
   pushq %rbp
   movq  %rsp, %rbp
   leaq  _msg(%rip), %rdi
   #movq  _msg@GOTPCREL(%rip), %rdi
   xorq  %rax, %rax
   call  _printf
   popq  %rbp
   retq

