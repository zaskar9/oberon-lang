%pragma macho64 prefix _
         global   main
         extern   printf

         section  .data
msg:     db       "The value is %d!", 10, 0

         section  .text
func:    push     rbp
         mov      rbp, rsp

         push     rdi              ; push 1st argument on stack
         push     rsi              ; push 2nd argument on stack
         push     rdx              ; push 3rd argument on stack

         sub      rsp, 8           ; allocate space for two more variables

         push     rbx              ; save callee-saved registers
         push     r12
         push     r13
         push     r14
         push     r15

         mov      rbx, [rbp-8]
         mov      rcx, [rbp-16]
         mov      rdx, [rbp-24]

         add      rcx, rdx
         add      rbx, rcx
         mov      [rbp-32], rbx

         mov      rbx, [rbp-32]
         imul     rbx, 5
         mov      [rbp-36], rbx

         mov      rax, [rbp-32]

         pop      r15
         pop      r14
         pop      r13
         pop      r12
         pop      rbx

         mov      rsp, rbp
         pop      rbp
         ret


main:    push     rbp
         mov      rbp, rsp

         mov      rdi, 1
         mov      rsi, 2
         mov      rdx, 3
         call     func

         mov      rdi, msg
         mov      rsi, rax
         xor      rax, rax
         call     printf

         xor      rax, rax
         mov      rsp, rbp
         pop      rbp
         ret
