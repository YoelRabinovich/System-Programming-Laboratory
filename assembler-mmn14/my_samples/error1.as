; hello

.entry  LIST
.extern W
MAIN:   add     r3,     LIST
LOOP:   prn     #48
        lea     W
        inc     r8
        mov     r3,     K
        sub     r1,     LIST

        bne     END
        cmp     K
        bne     &END START

        dec     W
.entry  MAIN
        jmp     &LOOP
        add     L3,     L3
END:    stop

STR:    .string     "abcd" lala
LISSST:   .data       6,  -9   
        .data       -100
K:      .data       31
.extern L3
