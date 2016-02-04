.section .text
.global memcpy

@
@ C signature: void _memcpy(U32* dest, U32* src, U32 length)
@
memcpy:
    stmfd   sp!, {r4-r11} 
    add     r11, pc, #12

    memcmp:
        subs    r2, r2, #8
        bpl     mem8

        @ This multiplies r2 by 12 and stores to pc. This branches to the
        @ appropriate memcpy sub label. Good luck future selves (and Ben).
        rsb     r3, r2, r2, asl #2
        sub     pc, r11, r3, asl #2

    mem8:
        ldmia   r1!, {r3,r4,r5,r6,r7,r8,r9,r10}
        stmia   r0!, {r3,r4,r5,r6,r7,r8,r9,r10}
        b       memcmp

    mem7:
        ldmia   r1, {r3,r4,r5,r6,r7,r8,r9}
        stmia   r0, {r3,r4,r5,r6,r7,r8,r9}
        b       memexit

    mem6:
        ldmia   r1, {r3,r4,r5,r6,r7,r8}
        stmia   r0, {r3,r4,r5,r6,r7,r8}
        b       memexit

    mem5:
        ldmia   r1, {r3,r4,r5,r6,r7}
        stmia   r0, {r3,r4,r5,r6,r7}
        b       memexit

    mem4:
        ldmia   r1, {r3,r4,r5,r6}
        stmia   r0, {r3,r4,r5,r6}
        b       memexit

    mem3:
        ldmia   r1, {r3,r4,r5}
        stmia   r0, {r3,r4,r5}
        b       memexit

    mem2:
        ldmia   r1, {r3,r4}
        stmia   r0, {r3,r4}
        b       memexit

    mem1:
        ldmia   r1, {r3}
        stmia   r0, {r3}
        nop

    memexit:
        ldmfd   sp!, {r4-r11}
        bx      lr
        