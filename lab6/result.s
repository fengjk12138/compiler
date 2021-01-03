.section .data
a: .int 0
b: .fill 20

.section .rodata
string1:
    .string "%d"
.text
.global main
.type main, @function

main:
    pushl %ebp
    movl %esp, %ebp
    pushl $0

    movl %ebp, %eax
    subl $4, %eax

    pushl %eax
    pushl $string1
    call scanf
    addl $8,%esp


    pushl -4(%ebp)
    pushl $string1
    call printf
    addl $8, %esp
    movl $0, %eax

    popl %ebx
    popl %ebp
    ret
    .section .note.GNU-stack,"",@progbits
