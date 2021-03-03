.global a,c
.extern b
.section .text:
        JMP *a(%pc)
        JMP *e(%r7)
        JNE *b(%r0)
        JGT *d(%r1)
d:       MOV 6(%r1),b
        ADD c,  %r1
        AND %r1, e
        HALT
        POP %r4
        
        PUSH e
        HALT
        
.section .data:
        .skip  0x8
        .equ f,  a-e+d+6
        .equ g, 0xA-1+0b10
        .equ h,f-g
e:      .word f
        .byte 15,0xF,0b1111
        .word c
        .word .bss
a:      .byte -6,-0x6,-0b110
.section .bss:
c:      .skip  0b1000
.end


