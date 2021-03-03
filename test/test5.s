.section .text:
main:
    PUSH %psw
d:   XCHG %r2,(%r5)
    MOV  main,labela(%r7)
    MOV  0xFA,labelaspolja(%pc)
    ADD  0b1110,main
    SUB  57(%r4),22(%r4)
    MUL (%r3),%r2
    CMP  (%r3),(%r6)
labela:
    NOT $0x55 , labelaspolja
    AND $0b1101 , labela
    MOV  $17, main
    PUSH $main
    OR $labelaspolja,main
a:  SHL labelaspolja,(%r4)
    SHR (%r7),labela
    MOV labelaspolja(%pc),%r5
    POP %psw
    IRET

    MUL d, %r6
    SUB a(%pc), 75
    XCHG $0b11111110,(%r7)
    HALT
    

.section .data:
    labelaspolja:
.end