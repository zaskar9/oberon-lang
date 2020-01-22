%pragma macho64 prefix _

         global   main
         extern   printf

         section  .data
msg:     db       "The answer is %d!", 10, 0

         section  .text
main:    push     rbp
         mov      rbp, rsp

         xor      rax, rax
         ;lea      rdi, [rel msg]       ;lea      rcx, [rel msg]
         mov      rdi, msg              ;mov      rcx, msg
         mov      rsi, 42               ;mov      rdx, 42
         call     printf

         xor      rax, rax
         mov      rsp, rbp
         pop      rbp
         ret

; nasm -fmacho64 hello.asm
; ld -macosx_version_min 10.11 -lSystem -e _main -o hello hello.o

; nasm -fwin64 hello.asm
; link hello.obj msvcrt.lib legacy_stdio_definitions.lib
