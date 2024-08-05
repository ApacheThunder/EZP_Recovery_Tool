
 .ALIGN
 .GLOBAL	ARM7_Bios


@-------------------------------------------------------------
 .ARM

ARM7_Bios:
    adr		r1, bios_dump+1
    bx		r1
    
 .THUMB

bios_dump:
    push	{r4-r7, lr}
    mov		r2, r0
    ldr 	r0, =0x5ED
    ldr 	r1, =0x1077

loop:
    mov		r6, #0x12
    sub 	r3, r1, r6
    adr 	r6, ret
    push	{r2-r6}
    bx		r0

 .ALIGN

ret:
    strb	r3, [r2,r1]
    sub		r1, #1
    bpl		loop

    pop		{r4-r7}
    pop		{r3}
    bx		r3


@	stmdb	r13!, {r4,r5,r6,r7,r14}
@	sub	r3, r0, #0x12
@	mov	r0, #0x5E0
@	add	r0, r0, #0xD
@	mov	r3, r3
@	add	r1, r15, #0xE
@	add	r6, r15, #0x7
@	stmdb	r13!, {r2,r3,r4,r5,r6}
@	bx	r0
@
@	strmi	r4, [r8, -r8, lsl #0xe]
@	mov	r0, r3
@	and	r0, r0, #0xFF
@	ldmia	r13!, {r4,r5,r6,r7,r14}
@	bx	r14
@---------------------------------------------------------------


.end
