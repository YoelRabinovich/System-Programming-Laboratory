; hello2

.entry  LIST
.extern W
IAMASYMBOL:   add     r3,     LIST
LOOP:   prn     #48
        lea     W,       r6
        inc     r8
        mov     r19,     K
        sub     r1,     r4
; hello again
        bne     END
        cmp     K,      #-6
        bne     &END

        dec     W
.entry  IAMASYMBOL
        jmp     &LOOP
        add     L3,     L3
END:    stop

STR:    .string     "abcd"
        .data       -100
K:      .data       31
.extern L3
