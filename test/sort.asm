%pragma macho64 prefix _
      global   main
      extern   printf

      section  .data
val:  db       "%d", 10, 0
tpl:  db       "(%d,%d)", 10, 0

      section  .bss
arr:  resb     80                ; reserve 20 * 4 bytes

      section  .text
;===============================================================================
init:
      push     rbp               ; save base pointer
      mov      rbp, rsp          ; set base pointer to stack pointer
      sub      rsp, 4            ; allocate space for one variable w/o alignment
      push     rbx               ; save callee-saved registers
      push     r12
      push     r13
      push     r14
      push     r15
      mov      ebx, 0            ; i := 0
      cmp      ebx, 20           ; i < 20
      jge      endloop_1
startloop_1:
      lea      rcx, [rel arr]    ; @a
      mov      edx, 20
      sub      edx, ebx          ; Dim - i
      mov      [rcx,rbx*4], edx  ; a[i] := Dim - i
      inc      ebx               ; i := i + 1
      cmp      ebx, 20           ; i < 20
      jl       startloop_1
endloop_1:
      pop      r15               ; restore callee-save registers
      pop      r14
      pop      r13
      pop      r12
      pop      rbx
      mov      rsp, rbp          ; restore stack pointer
      pop      rbp               ; restore base pointer
      ret                        ; return

;===============================================================================
print:
      push     rbp               ; save base pointer
      mov      rbp, rsp          ; set base pointer to stack pointer
      sub      rsp, 8            ; allocate space for one variable and align
      push     rbx               ; save callee-saved registers
      push     r12
      push     r13
      push     r14
      push     r15
      mov      ebx, 0            ; i := 0
      cmp      ebx, 20           ; i < 20
      jge      endloop_2
startloop_2:
      mov      rdi, val
      lea      rcx, [rel arr]    ; @a
      mov      esi, [rcx,rbx*4]  ; a[i]
      xor      rax, rax
      call     printf
      inc      ebx               ; i := i + 1
      cmp      ebx, 20           ; i < 20
      jl       startloop_2
endloop_2:
      pop      r15               ; restore callee-save registers
      pop      r14
      pop      r13
      pop      r12
      pop      rbx
      mov      rsp, rbp          ; restore stack pointer
      pop      rbp               ; restore base pointer
      ret                        ; return

;===============================================================================
swap:
      push     rbp               ; save base pointer
      mov      rbp, rsp          ; set base pointer to stack pointer
      sub      rsp, 8            ; allocate space for one variable and align
      push     rbx               ; save callee-saved registers
      push     r12
      push     r13
      push     r14
      push     r15
      mov      ebx, [rdi]        ; perform swap
      mov      r12d, [rsi]
      mov      [rsi], ebx
      mov      [rdi], r12d
      mov      rdi, tpl          ; print
      mov      esi, ebx
      mov      edx, r12d
      xor      rax, rax
      call     printf
      pop      r15               ; restore callee-save registers
      pop      r14
      pop      r13
      pop      r12
      pop      rbx
      mov      rsp, rbp          ; restore stack pointer
      pop      rbp               ; restore base pointer
      ret                        ; return

;===============================================================================
bubble_sort:
      push     rbp               ; save base pointer
      mov      rbp, rsp          ; set base pointer to stack pointer
      sub      rsp, 8            ; allocate space for one variable with align
      push     rbx               ; save callee-saved registers
      push     r12
      push     r13
      push     r14
      push     r15
      mov      ebx, 0            ; i := 0
      cmp      ebx, 20           ; i < 20
      jge      endloop_3
startloop_3:
      mov      ecx, 19           ; j := 19
      cmp      ecx, ebx          ; j > i
      jle      endloop_4
startloop_4:
      lea      rdx, [rel arr]    ; @a
      mov      r12d, [rdx-4,rcx*4]
      mov      r13d, [rdx,rcx*4]
      cmp      r12d, r13d        ; a[j - 1] > a[j]
      jle      endif_1
      lea      rdi, [rdx-4,rcx*4]
      lea      rsi, [rdx,rcx*4]
      mov      [rbp-4], ecx
      call     swap
      mov      ecx, [rbp-4]
endif_1:
      dec      ecx               ; j := j - 1
      cmp      ecx, ebx          ; j > i
      jg       startloop_4
endloop_4:
      inc      ebx               ; i := i + 1
      cmp      ebx, 20           ; i < 20
      jl       startloop_3
endloop_3:
      pop      r15               ; restore callee-save registers
      pop      r14
      pop      r13
      pop      r12
      pop      rbx
      mov      rsp, rbp          ; restore stack pointer
      pop      rbp               ; restore base pointer
      ret                        ; return

;===============================================================================
q_sort:
      push     rbp               ; save base pointer
      mov      rbp, rsp          ; set base pointer to stack pointer
      push     rdi               ; save 1st argument (l)
      push     rsi               ; save 2nd argumern (r)
      sub      rsp, 24           ; allocate space for three variables and align
      push     rbx               ; save callee-saved registers
      push     r12
      push     r13
      push     r14
      push     r15
      mov      ebx, edi          ; i := l
      mov      r12d, esi         ; j := r
      mov      edx, 0            ; div: set higher 32 bits to
      mov      eax, r12d         ; div: set lower 32 bits to r
      add      eax, ebx          ; div: set lower 32 bit to r + l
      mov      ecx, 2            ; div: set divisor to 2
      div      ecx               ; (l + r) DIV 2
      lea      r14, [rel arr]    ; @arr
      mov      r13d, [r14,rax*4] ; x := a[(l + r) DIV 2]
qs_outer:
      mov      r15d, [r14,rbx*4] ; a[i]
      cmp      r15d, r13d        ; a[i] < x
      jge      qs_scan_ltr_end
qs_scan_ltr_start:
      inc      ebx               ; i := i + 1
      mov      r15d, [r14,rbx*4] ; a[i]
      cmp      r15d, r13d        ; a[i] < x
      jl       qs_scan_ltr_start
qs_scan_ltr_end:
      mov      r15d, [r14,r12*4] ; a[j]
      cmp      r13d, r15d        ; x < a[j]
      jge      qs_scan_rtl_end
qs_scan_rtl_start:
      dec      r12d              ; j := j - 1
      mov      r15d, [r14,r12*4] ; a[j]
      cmp      r13d, r15d        ; x < a[j]
      jl       qs_scan_rtl_start
qs_scan_rtl_end:
      cmp      ebx, r12d         ; i <= j
      jg       qs_if_end
      lea      rdi, [r14,rbx*4]  ; @a[i]
      lea      rsi, [r14,r12*4]  ; @a[j]
      call     swap
      inc      ebx               ; i := i + 1
      dec      r12d              ; j := j - 1
qs_if_end:
      cmp      ebx, r12d         ; i <= j
      jle      qs_outer
      mov      r15d, [rbp-8]     ; r
      cmp      r15d, r12d        ; r < j
      jge      qs_if_2_end
      mov      edi, r15d
      mov      esi, r12d
      call     q_sort            ; QSort(r, j)
qs_if_2_end:
      mov      r15d, [rbp-16]    ; l
      cmp      ebx, r15d         ; i < l
      jge      qs_if_3_end
      mov      edi, ebx
      mov      esi, r15d
      call     q_sort            ; QSort(i, l)
qs_if_3_end:
      pop      r15               ; restore callee-save registers
      pop      r14
      pop      r13
      pop      r12
      pop      rbx
      mov      rsp, rbp          ; restore stack pointer
      pop      rbp               ; restore base pointer
      ret

;===============================================================================
quick_sort:
      push     rbp               ; save base pointer
      mov      rbp, rsp          ; set base pointer to stack pointer
      push     rbx               ; save callee-saved registers
      push     r12
      push     r13
      push     r14
      push     r15
      sub      rsp, 8            ; align stack pointer
      mov      edi, 0
      mov      esi, 19
      call     q_sort            ; QSort(0, 19)
      add      rsp, 8
      pop      r15               ; restore callee-save registers
      pop      r14
      pop      r13
      pop      r12
      pop      rbx
      mov      rsp, rbp          ; restore stack pointer
      pop      rbp               ; restore base pointer
      ret

;===============================================================================
main:
      push     rbp               ; save base pointer
      mov      rbp, rsp          ; set base pointer to stack pointer
      sub      rsp, 8
      push     rbx               ; save callee-saved registers
      push     r12
      push     r13
      push     r14
      push     r15
      call     init
      lea      rbx, [rel arr]    ; @a
      add      ebx, 28
      mov      edi, ebx
      add      ebx, 36
      mov      esi, ebx
      call     swap              ; Swap(a[7], a[16])
      call     print
      call     bubble_sort
      call     print
      pop      r15               ; restore callee-save registers
      pop      r14
      pop      r13
      pop      r12
      pop      rbx
      xor      rax, rax          ; set return value
      mov      rsp, rbp          ; restore stack pointer
      pop      rbp               ; restore base pointer
      ret

; nasm -fmacho64 sort.asm
; ld -macosx_version_min 10.11 -lSystem -e _main -o sort sort.o
