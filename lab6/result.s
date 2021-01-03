.section .data
a: .int 0
.section .rodata
string1:
    .string "%d"
.text
.global main
.type main, @function

main:
    pushl $a
    pushl $string1
    call scanf
    addl $8, %esp
    ret
    .section .note.GNU-stack,"",@progbits
