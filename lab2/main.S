	.file	"main.c"
	.text
	.comm	_input, 4, 2
	.globl	_a
	.bss
	.align 4
_a:
	.space 4
	.globl	_b
	.data
	.align 4
_b:
	.long	1
	.globl	_i
	.align 4
_i:
	.long	1
	.comm	_result, 4, 2
	.text
	.globl	_Fibonacci
	.def	_Fibonacci;	.scl	2;	.type	32;	.endef
_Fibonacci:
LFB13:
	.cfi_startproc
	pushl	%ebp
	.cfi_def_cfa_offset 8
	.cfi_offset 5, -8
	movl	%esp, %ebp
	.cfi_def_cfa_register 5
	jmp	L2
L3:
	movl	_a, %edx
	movl	_b, %eax
	addl	%edx, %eax
	movl	%eax, _result
	movl	_b, %eax
	movl	%eax, _a
	movl	_result, %eax
	movl	%eax, _b
	movl	_i, %eax
	addl	$1, %eax
	movl	%eax, _i
L2:
	movl	_i, %eax
	cmpl	%eax, 8(%ebp)
	jg	L3
	movl	_result, %eax
	popl	%ebp
	.cfi_restore 5
	.cfi_def_cfa 4, 4
	ret
	.cfi_endproc
LFE13:
	.def	___main;	.scl	2;	.type	32;	.endef
	.section .rdata,"dr"
LC0:
	.ascii "%d\0"
	.text
	.globl	_main
	.def	_main;	.scl	2;	.type	32;	.endef
_main:
LFB14:
	.cfi_startproc
	pushl	%ebp
	.cfi_def_cfa_offset 8
	.cfi_offset 5, -8
	movl	%esp, %ebp
	.cfi_def_cfa_register 5
	andl	$-16, %esp
	subl	$16, %esp
	call	___main
	movl	$_input, 4(%esp)
	movl	$LC0, (%esp)
	call	_scanf
	movl	_input, %eax
	movl	%eax, (%esp)
	call	_Fibonacci
	movl	%eax, 4(%esp)
	movl	$LC0, (%esp)
	call	_printf
	movl	$0, %eax
	leave
	.cfi_restore 5
	.cfi_def_cfa 4, 4
	ret
	.cfi_endproc
LFE14:
	.ident	"GCC: (i686-posix-dwarf-rev0, Built by MinGW-W64 project) 8.1.0"
	.def	_scanf;	.scl	2;	.type	32;	.endef
	.def	_printf;	.scl	2;	.type	32;	.endef
