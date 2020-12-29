  .text
    .globl Fibonacci
    .type Fibonacci，@function
#全局变量存储
	.comm	input, 4, 2
	.globl	a
	.bss
	.align 4
a:
	.space 4
	.globl	b
	.data
	.align 4
b:
	.long	1
	.globl	i
	.align 4
i:
	.long	1
	.comm	result, 4, 2
    .section .rodata


STR0:
    .string "%d"
STR1:
    .string "%d"
#函数

L3:
    movl a, %ecx
    movl b, %edx
    addl %ecx, %edx
    movl %edx, result
    movl b, %eax
    movl %eax, a
    movl result, %eax
    movl %eax, b
    movl i, %eax
    addl $1, %eax
    movl %eax, i

Fibonacci:
    movl 4(%esp), %eax
    movl i, %ebx
    cmpl  %ebx, %eax
    jg L3
# return result
    movl result, %eax
    ret






     .text
     .globl main
     .type main, @function

main:
#输入数字
    pushl $input
    pushl $STR0
    call scanf
    addl $8, %esp
#输出函数，并且调用的函数参数压栈
    movl input, %edx
    pushl %edx
    call Fibonacci
    addl $4, %esp
# %eax保存返回值  
    pushl %eax 
    pushl $STR1
    call printf
    addl $8, %esp
# return 0
    movl $0, %eax
    ret
    .section .note.GNU-stack,"",@progbits