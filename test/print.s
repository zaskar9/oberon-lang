# begin of data section
	.data
s01:
	.asciz "%d\n"

# begin of code section
	.text
	
# global (exported) function definition: main
	.globl _main
_main:
	pushq %rbp				# save the base pointer
	movq  %rsp, %rbp		# set new base pointer

	movl  $42, %edi			# 1st parameter: 42
	xorl  %eax, %eax		# zero out %eax (#vargs)
	callq _write			# call write(42)

	xorl   %edi, %edi		# 1st parameter: 0
  	call  _exit				# call exit(0)
  	
# global (exported) function definition: write
	.globl _write
_write:
	pushq %rbp				# save the base pointer
	movq  %rsp, %rbp		# set new base pointer
	
	movq  %rdi, %rdx		# save 1st argument
	leaq  s01(%rip), %rdi	# 1st parameter
	movq  %rdx, %rsi		# 2nd parameter
	xorl  %eax, %eax		# zero out %eax (#vargs)
	callq _printf			# call printf("%d\n", x)

	movq  %rbp, %rsp		# reset stack to previous base pointer
	popq  %rbp				# restore previous base pointer
	retq					# return to caller
	
// gcc -o print print.s 