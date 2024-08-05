
 .ALIGN
 .GLOBAL	wait_TM1, port_setcmd, Key1_cmd, enc_key2, encryObj
 .CODE 32
 .ARM



@=======================================
@ R0 <- R1, R2 = Byte
@=======================================

_cpy_byte:
	cmp	r2, #0xF
	str	r14, [r13, #-0x4]!
	mov	r12, r0
	mov	r14, r2
	bls	_cpyb_1

	orr	r3, r1, r0
	tst	r3, #0x3
	beq	_cpyb_4

_cpyb_1:
	cmp	r14, #0x0
	beq	_cpyb_3

	mov	r2, #0x0

_cpyb_2:
	ldrb	r3, [r1], #+0x1
	strb	r3, [r2, +r12]
	add	r2, r2, #0x1
	cmp	r2, r14
	bne	_cpyb_2

_cpyb_3:
	ldr	r14, [r13], #+0x4
	bx	r14

_cpyb_4:
	ldr	r3, [r1, #+0x0]
	str	r3, [r12, #+0x0]
	ldr	r2, [r1, #+0x4]
	str	r2, [r12, #+0x4]
	ldr	r3, [r1, #+0x8]
	str	r3, [r12, #+0x8]
	sub	r14, r14, #0x10
	ldr	r3, [r1, #+0xc]
	cmp	r14, #0xF
	str	r3, [r12, #+0xc]
	add	r1, r1, #0x10
	add	r12, r12, #0x10
	bhi	_cpyb_4

	cmp	r14, #0x3
	bls	_cpyb_1

_cpyb_5:
	sub	r14, r14, #0x4
	ldr	r3, [r1], #+0x4
	cmp	r14, #0x3
	str	r3, [r12], #+0x4
	bhi	_cpyb_5

	b	_cpyb_1


@=======================================
@ R0, R1 = CMD
@=======================================
_cmd_send:
	sub	r13, r13, #0x8
	stmia	r13, {r0, r1}
	mov	r0, #0x4000000

_c_send_1:
	ldr	r1, [r0, #+0x1a4]
	cmp	r1, #0x0
	blt	_c_send_1

	mov	r1, r0
	mov	r3, #0xC0
	strb	r3, [r0, #+0x1a1]
	add	r1, r1, #0x1A8
	add	r0, r0, #0x1B0
	mov	r2, r13

_c_send_2:
	ldrb	r3, [r2, #+0x7]
	strb	r3, [r1], #+0x1
	cmp	r1, r0
	sub	r2, r2, #0x1
	bne	_c_send_2

	add	r13, r13, #0x8
	bx	r14

@=======================================
@	R0 = Time
@=======================================
wait_TM1:
	stmdb	r13!, {r1,r2,r3}
	mov	r2, #0x4000000
	add 	r2, r2, #0x100
	ldrh	r3, [r2, #+0x4]

	add 	r3, r3, r0
	mov	r3, r3, lsl #0x10
	mov	r1, r3, lsr #0x10

_wait_1:
	ldrh	r3, [r2, #+0x4]
	cmp	r3, r1
	bne	_wait_1

	ldmia	r13!, {r1,r2,r3}
	bx	r14


@=======================================
@ R0 = Normal ptr, R1 = Key1 ptr ,R2 = Heder ptr
@=======================================
port_setcmd:

	stmdb	r13!, {r3,r4,r5,r6,r12,r14}

	mov	r6, r0
	mov	r5, r1
	mov	r3, r2

	ldrb	r0, [r3, #+0x65]
	ldrb	r1, [r3, #+0x64]
	ldrb	r4, [r3, #+0x66]
	ldrb	r14, [r3, #+0x61]
	ldrb	r2, [r3, #+0x60]
	ldrb	r12, [r3, #+0x67]
	orr	r1, r1, r0, lsl #0x8
	orr	r1, r1, r4, lsl #0x10
	ldrb	r0, [r3, #+0x62]
	orr	r12, r1, r12, lsl #0x18
	orr	r2, r2, r14, lsl #0x8
	mov	r1, r12, lsl #0x13
	orr	r2, r2, r0, lsl #0x10
	mov	r3, #0x8F0
	mov	r1, r1, lsr #0x13
	add	r3, r3, #0x7
	bic	r2, r2, #0xFF000000
	cmp	r1, r3
	bic	r0, r12, #0xFF000000

	bic	r0, r0, #0xC00000
	bic	r0, r0, #0xE000
@	str	r0, [r14, #+0x0]

	andls	r3, r12, #0x3F0000
	addls	r0, r3, #0x8F0
	addls	r0, r0, #0x8
@	strls	r0, [r14, #+0x0]
	str	r0, [r6, #+0x0]


@	ldr	r0, [r14, #+0x0]
	mov	r3, r0, lsr #0x10
	cmp	r3, #0x17
	bicls	r3, r0, #0x3F0000
	addls	r0, r3, #0x180000
	strls	r0, [r6, #+0x0]		@ normal

	mov	r3, r0, lsl #0x13
	mov	r3, r3, lsr #0x13
	add	r3, r3, r0, lsr #0x10
	str	r3, [r5, #+0x0]		@ key1

	mov	r0, #0x0
	str	r0, Gamecode

	ldmia	r13!, {r3,r4,r5,r6,r12,r14}
	bx	r14


@=======================================
@ R0, R1 = CMD, R2 = Gamecode, R3 = Key1_table
@=======================================
Key1_cmd:
	stmdb	r13!, {r3,r4,r5,r6,r7,r8,r9,r12,r14}
	sub	r13, r13, #0xC
	mov	r5, r0
	mov	r6, r1
	mov	r8, r2
	mov	r9, r3

	mov	r12, #0x0
	mov	r7, r13

_k1cmd_1:
	mov	r2, r12, lsl #0x3
	mov	r3, r5, lsr r2		@@@@ e1a03235 mov r3, lsr r5
	rsb	r1, r2, #0x20
	orr	r3, r3, r6, lsl r1
	subs	r0, r2, #0x20
	movpl	r3, r6, lsr r0		@@@@ 51a03036 movpl r3, lsr r6
	strb	r3, [r12, +r7]
	add	r12, r12, #0x1
	cmp	r12, #0x8
	bne	_k1cmd_1

@	ldr	r0, r8		@03809520 Gamecode
	mov	r0, r8
	mov	r1, r13
	mov	r2, r9

	bl	_enc_prc


	mov	r5, #0x0
	mov	r6, #0x0
	mov	r0, #0x7

_k1cmd_2:
	ldrb	r1, [r0, +r7]
	mov	r2, #0x0
	mov	r4, r6, lsl #0x8
	sub	r0, r0, #0x1
	orr	r4, r4, r5, lsr #0x18
	mov	r3, r5, lsl #0x8
	cmn	r0, #0x1
	orr	r5, r3, r1
	orr	r6, r4, r2
	bne	_k1cmd_2

	mov	r0, r5
	mov	r1, r6
	bl	_cmd_send

@	mov	r0, #0x5
@	bl	wait_TM1

@ kkkkk++
@	ldr	r2, [r15, #+0x20]
@	ldr	r3, [r2, #+0x0]
@	add	r3, r3, #0x1
@	bic	r3, r3, #0xFF000000
@	bic	r3, r3, #0xF00000
@	str	r3, [r2, #+0x0]

	add	r13, r13, #0xC
	ldmia	r13!, {r3,r4,r5,r6,r7,r8,r9,r12,r14}
	bx	r14


@===================================================================================================



@ R0=Gamecode, R1=CMD(stack add), R12=nByte

_enc_prc:
	stmdb	r13!, {r4,r5,r6,r7,r8,r14}

	ldr	r3, Gamecode
	cmp	r3, r0

	sub	r13, r13, #0x4
	mov	r6, r2			@ key1_table
	mov	r7, r0
	mov	r8, r1
	beq	_enc_p2


@	mov	r5, #0x1040
@	ldr	r6, _enc_Tblb
@	add	r5, r5, #0x38
@	mov	r4, #0x30

@_enc_p1:
@	mov	r0, r4
@	bl	_ARM7_Bios

@	add	r3, r4, r6
@	add	r4, r4, #0x1
@	cmp	r4, r5
@	strb	r0, [r3, #-0x30]
@	bne	_enc_p1


	mov	r0, r7			@ gamecode
	mov	r1, r6			@ key1_table
	bl	_enc_tb_prc

	str	r7, Gamecode

_enc_p2:
	ldr	r3, _enc_val
	ldr	r14, [r3, #+0x4]
	ldr	r12, [r3, #+0x8]
	ldr	r0, _enc_Tbls
	mov	r14, r14, lsl #0x1
	mov	r12, r12, lsr #0x1
	mov	r2, r8
	add	r1, r8, #0x4
	str	r12, [r3, #+0x8]
	str	r14, [r3, #+0x4]
	add	r13, r13, #0x4
	ldmia	r13!, {r4,r5,r6,r7,r8,r14}

_enc_sub:
	stmdb	r13!, {r4,r5,r6,r7,r8,r10,r14}
	mov	r8, r2
	ldr	r7, [r8, #+0x0]
	ldr	r2, [r1, #+0x0]
	sub	r13, r13, #0x4
	mov	r10, r1
	mov	r6, r0
	mov	r5, #0x0

_enc_s1:
	ldr	r3, [r5, +r6]
	eor	r4, r2, r3
	mov	r0, r6
	mov	r1, r4
	bl	_enc_s2

	add	r5, r5, #0x4
	cmp	r5, #0x40
	eor	r2, r0, r7
	mov	r7, r4
	bne	_enc_s1

	ldr	r3, [r6, #+0x40]
	eor	r3, r3, r2
	str	r3, [r8, #+0x0]
	ldr	r2, [r6, #+0x44]
	eor	r2, r2, r4
	str	r2, [r10, #+0x0]
	add	r13, r13, #0x4
	ldmia	r13!, {r4,r5,r6,r7,r8,r10,r14}
	bx	r14


_enc_s2:
	mov	r2, r1, lsr #0xe
	mov	r12, r1, lsr #0x6
	mov	r3, r1, lsr #0x18
	and	r2, r2, #0x3FC
	add	r2, r2, r0
	add	r3, r0, r3, lsl #0x2
	and	r12, r12, #0x3FC

	stmdb	r13!, {r4,r14}
	add	r12, r12, r0
	ldr	r4, [r2, #+0x448]
	mov	r14, r0
	and	r1, r1, #0xFF
	ldr	r0, [r3, #+0x48]
	add	r14, r14, r1, lsl #0x2
	ldr	r2, [r12, #+0x848]
	add	r0, r0, r4
	ldr	r3, [r14, #+0xc48]
	eor	r0, r0, r2
	add	r0, r0, r3
	ldmia	r13!, {r4,r14}

	bx	r14



_enc_tb_prc:

	stmdb	r13!, {r4,r5,r6,r14}
	ldr	r6, _enc_Tbls
	ldr	r4, _enc_val
	mov	r2, #0x1040
	mov	r5, r0
	add	r2, r2, #0x8
@	ldr	r1, _enc_Tblb
	mov	r0, r6
	bl	_cpy_byte

	mov	r2, r5, lsl #0x1
	mov	r3, r5, lsr #0x1
	mov	r0, r6
	mov	r1, r4
	str	r3, [r4, #+0x4]
	str	r2, [r4, #+0x8]
	str	r5, [r4, #+0x0]
	bl	_enc_tb_p1

	mov	r0, r6
	mov	r1, r4
	ldmia	r13!, {r4,r5,r6,r14}


_enc_tb_p1:
	stmdb  r13!, {r4,r5,r6,r14}
	add	r5, r1, #0x4
	mov	r4, r1
	mov	r6, r0
	mov	r2, r5
	add	r1, r1, #0x8
	bl	_enc_sub

	mov	r0, r6
	mov	r1, r5
	mov	r2, r4
	bl	_enc_sub

	mov	r0, r6
	mov	r1, r4
	ldmia	r13!, {r4,r5,r6,r14}


	stmdb	r13!, {r4,r5,r6,r7,r8,r14}
	mov	r3, #0x0
	sub	r13, r13, #0x8
	str	r3, [r13, #+0x0]

	str	r3, [r13, #+0x4]
	mov	r5, r0
	mov	r14, r3

_enc_tb_p2:
	mov	r12, #0x0
	mov	r0, r12

_enc_tb_p3:
	add	r3, r0, r14
	and	r3, r3, #0x7
	ldrb	r2, [r3, +r1]
	add	r0, r0, #0x1
	cmp	r0, #0x4
	orr	r12, r2, r12, lsl #0x8
	bne	_enc_tb_p3

	ldr	r3, [r14, +r5]
	eor	r3, r3, r12
	str	r3, [r14, +r5]
	add	r14, r14, #0x4
	cmp	r14, #0x48
	bne	_enc_tb_p2

	mov	r4, r5
	mov	r8, r13
	add	r7, r13, #0x4
	add	r6, r5, #0x48

_enc_tb_p4:
	mov	r2, r7
	mov	r0, r5
	mov	r1, r13
	bl	_enc_sub

	ldr	r2, [r13, #+0x4]
	ldr	r3, [r13, #+0x0]
	str	r2, [r4, #+0x4]
	str	r3, [r4, #+0x0]
	add	r4, r4,#0x8
	cmp	r4, r6
	bne	_enc_tb_p4

	mov	r4, r5
	add	r6, r5, #0x1000

_enc_tb_p5:
	mov	r2, r7
	mov	r0, r5
	mov	r1, r13
	bl	_enc_sub

	ldr	r2, [r13, #+0x4]
	ldr	r3, [r13, #+0x0]
	str	r2, [r4, #+0x4c]
	str	r3, [r4, #+0x48]
	add	r4, r4, #0x8
	cmp	r4, r6
	bne	_enc_tb_p5

	add	r13, r13, #0x8
	ldmia	r13!, {r4,r5,r6,r7,r8,r14}
	bx	r14

@
@ R0=mmmnnn, R1=Encryption Seed Select value
@
enc_key2:
	stmdb	r13!, {r2,r3,r4,r5,r6,r7,r8,r9,r12,r14}
	mov	r4, r0
	sub	r13, r13, #0x4
@	add	r0, r1, #0x2A
@	bl	_ARM7_Bios
	mov	r0, r1

	mov	r5, r4, asr #0x1f
	mov	r5, r5, lsl #0xf
	orr	r5, r5, r4, lsr #0x11
	mov	r2, #0x6000
	mov	r4, r4, lsl #0xf
	mov	r3, #0x0
	and	r0, r0, #0xFF
	mov	r1, #0x0
	orr	r6, r4, r2
	orr	r7, r5, r3

@	ldr	r3, [r15, #+0x74]		;r15+0x74=*(03803e04)=#58747792(0x03806b90)
	orr	r8, r0, r6
	orr	r9, r1, r7
@	ldr	r2, [r15, #+0x6c]		;r15+0x6c=*(03803e08)=#58747800(0x03806b98)
@	stmia  r3 ,{r8,r9}
	ldr	r3, =0x879b9b05
	mov	r4, #0x5C
@	stmia	r2 ,{r3,r4}
	mov	r0, r8
	mov	r1, r9
	bl	_eky2_sub

@	ldr	r3, [r15, #+0x54]		;r15+0x54=*(03803e10)=#58747808(0x03806ba0)
@	stmia	r3, {r0,r1}
	ldr	r0, =0x879b9b05
	mov	r1, #0x5C
	bl	_eky2_sub

@	ldr	r2, [r15, #+0x44]		;r15+0x44=*(03803e14)=#58747816(0x03806ba8)
	mov	r12, #0x4000000
@	stmia	r2, {r0,r1}

	mov	r3, r9, lsl #0x10
	ldr	r2, =0x879b9b05
	add	r1, r12, #0x100
	mov	r3, r3, lsr #0x10
	mov	r4, #0x5C
	str	r8, [r12, #+0x1b0]
	strh	r3, [r1, #+0xb8]
	str	r2, [r12, #+0x1b4]
	strh	r4, [r1, #+0xba]
	add	r13, r13, #0x4
	ldmia	r13!, {r2,r3,r4,r5,r6,r7,r8,r9,r12,r14}
	bx	r14

_eky2_sub:
	stmdb	r13!, {r4,r5,r6,r7,r8,r10,r11}
	mov	r5, r0
	mov	r6, r1
	mov	r0, #0x0
	mov	r1, #0x0
	mov	r12, #0x0
	mov	r7, #0x1
	mov	r8, #0x0

_eky2_s1:
	adds	r3, r0, r0
	adc	r4, r1, r1
	mov	r2, r4
	mov	r1, r3
	and	r4, r6, r8
	and	r3, r5, r7
	add	r12, r12, #0x1
	orr	r10, r1, r3
	movs	r6, r6, lsr #0x1
	mov	r5, r5, rrx		@ e1a05065 mov	r5, r5, ror #0x0
	orr	r11, r2, r4
	cmp	r12, #0x27
	mov	r0, r10
	mov	r1, r11
	bne	_eky2_s1

	ldmia	r13!, {r4,r5,r6,r7,r8,r10,r11}
	bx	r14


@
@ R0=gamecode, R1=Ptr, R2=key1_table
@
encryObj:
	stmdb	r13!, {r4,r5,r14}
	mov	r5, r1
	sub	r13, r13, #0x4
	add	r4, r1, #0x4
	mov	r1, r2
	bl	_enc_tb_prc


	mov	r2, r5
	ldr	r0, _enc_Tbls
	mov	r1, r4
	bl	encry_tbl

	ldr	r3, _enc_val
	ldmib	r3, {r2,r12}
	mov	r2, r2, lsl #0x1
	mov	r12, r12, lsr #0x1
	mov	r1, r3
	stmib	r3, {r2,r12}
	ldr	r0, _enc_Tbls
	bl	_enc_tb_p1

	mov	r2, r5
	ldr	r0, _enc_Tbls
	mov	r1, r4
	bl	encry_tbl

	ldr	r2, [r5, #+0x0]
	ldr	r3, =0x72636e65		@ encr
	cmp	r2, r3
	beq	_encO_3

_encO_1:
	mvn	r2, #0x18000000
	sub	r2, r2, #0x2100
	mov	r3, #0x0

_encO_2:
	str	r2, [r3, +r5]
	add	r3, r3, #0x4
	cmp	r3, #0x800
	bne	_encO_2

	mov	r3, #0x2800000
	sub	r3, r3, #0x1
	mov	r2, #0x1
	str	r2, [r3, #-0x7f1]

_encO_fin:
	add	r13, r13, #0x4
	ldmia	r13!, {r4,r5,r14}
	bx	r14


_encO_3:
	ldr	r2, [r5, #+0x4]
	ldr	r3, =0x6a624f79		@ yObj
	cmp	r2, r3
	bne	_encO_3

	mvn	r3, #0x18000000
	sub	r3, r3, #0x2100
	str	r3, [r5, #+0x4]
	str	r3, [r5, #+0x0]
	add	r4, r4, #0x4
	add	r5, r5, #0x800

_encO_4:
	add	r1, r4, #0x4
	mov	r2, r4
	ldr	r0, _enc_Tbls
	add	r4, r4, #0x8
	bl	encry_tbl

	cmp	r4, r5
	bne	_encO_4

	b	_encO_fin


encry_tbl:
	stmdb	r13!, {r4,r5,r6,r7,r8,r10,r14}
	ldr	r12, [r1, #+0x0]
	ldr	r7, [r2, #+0x0]
	sub	r13, r13, #0x4
	mov	r8, r1
	mov	r10, r2
	mov	r6, r0
	mov	r5, #0x11

_encry_1:
	ldr	r3, [r6, +r5, lsl #0x2]
	eor	r4, r12, r3
	mov	r0, r6
	mov	r1, r4
	bl	_enc_s2

	sub	r5, r5, #0x1
	cmp	r5, #0x1
	eor	r12, r0, r7
	mov	r7, r4
	bne	_encry_1

	ldr	r3, [r6, #+0x0]
	eor	r3, r3, r4
	str	r3, [r8, #+0x0]
	ldr	r2, [r6, #+0x4]
	eor	r2, r2, r12
	str	r2, [r10, #+0x0]
	add	r13, r13, #0x4
	ldmia	r13!, {r4,r5,r6,r7,r8,r10,r14}
	bx	r14


@=======================================================

Gamecode:	.word	0x00000000

_enc_val:	.word	encval

_enc_Tbls:	.word	eTbls
@_enc_Tblb:	.word	eTblb

@=======================================================

.bss
encval:	.skip	(16)
eTbls:	.skip	(4352)
@eTblb:	.skip	(4352)

.end
