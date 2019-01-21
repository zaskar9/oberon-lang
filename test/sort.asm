%pragma macho64 prefix _
         global   main
         extern   printf

         section  .data
msg:     db       "%d", 10, 0
msg2:    db       "(%d,%d)", 10, 0

         section  .bss
array:   resb     80                        ; reserve 20 * 4 bytes

         section  .text
init:    push     rbp                       ; save base pointer
         mov      rbp, rsp                  ; set base pointer to stack pointer
         sub      rsp, 4                    ; allocate space for one more variable
         push     rbx                       ; save callee-saved registers
         push     r12
         push     r13
         push     r14
         push     r15
         mov      rbx, 0                    ; i := 0
         cmp      rbx, 20                   ; i < 20
         jge      endloop_1
startloop_1:
         lea      rcx, [rel array]
         mov      rdx, rbx
         neg      rdx                       ; -i
         add      rdx, 20                   ; Dim - i
         mov      [rcx,rbx*4], edx          ; a[i] := Dim - i
         inc      rbx                       ; i := i + 1
         cmp      rbx, 20                   ; i < 20
         jl       startloop_1
endloop_1:
         pop      r15                       ; restore callee-save registers
         pop      r14
         pop      r13
         pop      r12
         pop      rbx
         mov      rsp, rbp                  ; restore stack pointer
         pop      rbp                       ; restore base pointer
         ret                                ; return

print:   push     rbp                       ; save base pointer
         mov      rbp, rsp                  ; set base pointer to stack pointer
         sub      rsp, 16                   ; allocate space for one more variable
         push     rbx                       ; save callee-saved registers
         push     r12
         push     r13
         push     r14
         push     r15
         mov      rbx, 0                    ; i := 0
         cmp      rbx, 20                   ; i < 20
         jge      endloop_2
startloop_2:
         mov      rdi, msg
         lea      rcx, [rel array]
         mov      esi, [rcx,rbx*4]          ; a[i]
         xor      rax, rax
         call     printf
         inc      rbx                       ; i := i + 1
         cmp      rbx, 20                   ; i < 20
         jl       startloop_2
endloop_2:
         pop      r15                       ; restore callee-save registers
         pop      r14
         pop      r13
         pop      r12
         pop      rbx
         mov      rsp, rbp                  ; restore stack pointer
         pop      rbp                       ; restore base pointer
         ret                                ; return

swap:    push     rbp                       ; save base pointer
         mov      rbp, rsp                  ; set base pointer to stack pointer
         sub      rsp, 16                   ; allocate space for one more variable
         push     rbx                       ; save callee-saved registers
         push     r12
         push     r13
         push     r14
         push     r15
         mov      ebx, [rdi]                ; perform swap
         mov      ecx, [rsi]

         push     rdi
         push     rsi

         mov      rdi, msg2
         mov      rsi, rbx
         mov      rdx, rcx
         xor      rax, rax
         mov      [rbp-4], ecx
         call     printf
         mov      ecx, [rbp-4]

         pop      rsi
         pop      rdi

         mov      [rsi], ebx
         mov      [rdi], ecx
         pop      r15                       ; restore callee-save registers
         pop      r14
         pop      r13
         pop      r12
         pop      rbx
         mov      rsp, rbp                  ; restore stack pointer
         pop      rbp                       ; restore base pointer
         ret                                ; return

bubble_sort:
         push     rbp                       ; save base pointer
         mov      rbp, rsp                  ; set base pointer to stack pointer
         sub      rsp, 16                   ; allocate space for one more variable
         push     rbx                       ; save callee-saved registers
         push     r12
         push     r13
         push     r14
         push     r15
         mov      rbx, 0
         cmp      rbx, 20
         jge      endloop_3
startloop_3:
         mov      rcx, 19

         ;mov      rdi, msg
         ;mov      rsi, rbx
         ;xor      rax, rax
         ;mov      [rbp-4], ecx
         ;call     printf
         ;mov      ecx, [rbp-4]

         cmp      rcx, rbx
         jle      endloop_4
startloop_4:
         lea      rdx, [rel array]
         mov      r12b, [rdx-4,rcx*4]
         mov      r13b, [rdx,rcx*4]
         cmp      r12b, r13b
         jle      endif_1
         lea      rdi, [rdx-4,rcx*4]
         lea      rsi, [rdx,rcx*4]
         mov      [rbp-4], ecx
         call     swap
         mov      ecx, [rbp-4]

         ;mov      rdi, msg2
         ;mov      rsi, rcx
         ;xor      rax, rax
         ;mov      [rbp-4], ecx
         ;call     printf
         ;mov      ecx, [rbp-4]

endif_1: dec      rcx
         cmp      rcx, rbx
         jg       startloop_4
endloop_4:
         inc      rbx
         cmp      rbx, 20
         jl       startloop_3
endloop_3:
         pop      r15                       ; restore callee-save registers
         pop      r14
         pop      r13
         pop      r12
         pop      rbx
         mov      rsp, rbp                  ; restore stack pointer
         pop      rbp                       ; restore base pointer
         ret                                ; return

q_sort:
         push     rbp                       ; save base pointer
         mov      rbp, rsp                  ; set base pointer to stack pointer
         push     rdi
         push     rsi
         sub      rsp, 8                    ; allocate space for one more variable
         push     rbx                       ; save callee-saved registers
         push     r12
         push     r13
         push     r14
         push     r15

         mov      ebx, edi ; l
         mov      r12d, esi ; r
         mov      edx, 0
         mov      eax, r12d
         add      eax, ebx ; l + r
         mov      ecx, 2
         div      ecx
         mov      r13d, eax
         lea      r14, [rel array]
         mov      r13d, [r14,r13*4]
qs_outer:
         mov      r15d, [r14,rbx*4]
         cmp      r15d, r13d
         jge      qs_scan_ltr_end
qs_scan_ltr_start:
         inc      rbx
         mov      r15d, [r14,rbx*4]
         cmp      r15d, r13d
         jl       qs_scan_ltr_start
qs_scan_ltr_end:
         mov      r15d, [r14,r12*4]
         cmp      r13d, r15d
         jge      qs_scan_rtl_end
qs_scan_rtl_start:
         dec      r12
         mov      r15d, [r14,r12*4]
         cmp      r13d, r15d
         jl       qs_scan_rtl_start
qs_scan_rtl_end:
         cmp      rbx, r12
         jg       qs_if_end
         lea      rdi, [r14,rbx*4]
         lea      rsi, [r14,r12*4]
         call     swap
         inc      rbx
         dec      r12
qs_if_end:
         cmp      rbx, r12
         jle      qs_outer
         mov      r15, [rbp-8]
         cmp      r15, r12
         jge      qs_if_2_end
         mov      rdi, r15
         mov      rsi, r12
         call     q_sort
qs_if_2_end:
         mov      r15, [rbp-16]
         cmp      rbx, r15
         jge      qs_if_3_end
         mov      rdi, rbx
         mov      rsi, r15
         call     q_sort
qs_if_3_end:
         pop      r15                       ; restore callee-save registers
         pop      r14
         pop      r13
         pop      r12
         pop      rbx
         mov      rsp, rbp                  ; restore stack pointer
         pop      rbp                       ; restore base pointer
         ret

quick_sort:
         push     rbp                       ; save base pointer
         mov      rbp, rsp                  ; set base pointer to stack pointer
         sub      rsp, 8                    ; allocate space for one more variable
         push     rbx                       ; save callee-saved registers
         push     r12
         push     r13
         push     r14
         push     r15

         mov      rdi, 0
         mov      rsi, 19
         call     q_sort

         pop      r15                       ; restore callee-save registers
         pop      r14
         pop      r13
         pop      r12
         pop      rbx
         mov      rsp, rbp                  ; restore stack pointer
         pop      rbp                       ; restore base pointer
         ret

main:    push     rbp                       ; save base pointer
         mov      rbp, rsp                  ; set base pointer to stack pointer
         push     rbx                       ; save callee-saved registers
         push     r12
         push     r13
         push     r14
         push     r15

         call     init

         lea      rbx, [rel array]
         ;mov      rcx, 6
         ;lea      rdi, [rbx-4,rcx*4]
         ;lea      rsi, [rbx,rcx*4]
         add      rbx, 32
         mov      rdi, rbx
         add      rbx, 32
         mov      rsi, rbx
         call     swap

         call     print

         call     quick_sort

         call     print

         pop      r15                       ; restore callee-save registers
         pop      r14
         pop      r13
         pop      r12
         pop      rbx
         xor      rax, rax                  ; set return value
         mov      rsp, rbp                  ; restore stack pointer
         pop      rbp                       ; restore base pointer
         ret

; nasm -fmacho64 sort.asm
; ld -macosx_version_min 10.11 -lSystem -e _main -o sort sort.o
