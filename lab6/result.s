.section .data
a: .int 0
b: .int 20,12,30

.section .rodata
string1:
    .string "%d"
.text
.global main
.type main, @function

main:
    pushl %ebp
    movl %esp, %ebp

   
    movl $b,%eax 
    addl $4,%eax
    pushl (%eax)
    pushl $string1
    call printf
    addl $8, %esp
    movl $0, %eax

   
    popl %ebp
    ret
    .section .note.GNU-stack,"",@progbits
