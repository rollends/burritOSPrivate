	.file	"byteBuffer.c"
	.text
	.align	2
	.global	byteBufferInit
	.type	byteBufferInit, %function
byteBufferInit:
	@ args = 0, pretend = 0, frame = 16
	@ frame_needed = 1, uses_anonymous_args = 0
	mov	ip, sp
	stmfd	sp!, {fp, ip, lr, pc}
	sub	fp, ip, #4
	sub	sp, sp, #16
	str	r0, [fp, #-16]
	str	r1, [fp, #-20]
	mov	r3, r2
	strh	r3, [fp, #-24]	@ movhi
	ldr	r3, [fp, #-16]
	cmp	r3, #0
	bne	.L2
	mvn	r3, #0
	str	r3, [fp, #-28]
	b	.L4
.L2:
	ldr	r3, [fp, #-20]
	cmp	r3, #0
	bne	.L5
	mvn	r3, #1
	str	r3, [fp, #-28]
	b	.L4
.L5:
	ldrh	r3, [fp, #-24]
	cmp	r3, #0
	bne	.L7
	mvn	r3, #2
	str	r3, [fp, #-28]
	b	.L4
.L7:
	ldr	r2, [fp, #-16]
	ldr	r3, [fp, #-20]
	str	r3, [r2, #0]
	ldr	r2, [fp, #-16]
	mov	r3, #0
	strh	r3, [r2, #4]	@ movhi
	ldr	r2, [fp, #-16]
	mov	r3, #0
	strh	r3, [r2, #6]	@ movhi
	ldr	r2, [fp, #-16]
	ldrh	r3, [fp, #-24]	@ movhi
	strh	r3, [r2, #8]	@ movhi
	ldr	r2, [fp, #-16]
	mov	r3, #0
	strb	r3, [r2, #10]
	mov	r3, #0
	str	r3, [fp, #-28]
.L4:
	ldr	r3, [fp, #-28]
	mov	r0, r3
	sub	sp, fp, #12
	ldmfd	sp, {fp, sp, pc}
	.size	byteBufferInit, .-byteBufferInit
	.align	2
	.global	byteBufferPeek
	.type	byteBufferPeek, %function
byteBufferPeek:
	@ args = 0, pretend = 0, frame = 8
	@ frame_needed = 1, uses_anonymous_args = 0
	mov	ip, sp
	stmfd	sp!, {fp, ip, lr, pc}
	sub	fp, ip, #4
	sub	sp, sp, #8
	str	r0, [fp, #-16]
	ldr	r3, [fp, #-16]
	cmp	r3, #0
	bne	.L11
	mvn	r3, #0
	str	r3, [fp, #-20]
	b	.L13
.L11:
	ldr	r0, [fp, #-16]
	bl	byteBufferCount(PLT)
	mov	r3, r0
	cmp	r3, #0
	bne	.L14
	mvn	r3, #3
	str	r3, [fp, #-20]
	b	.L13
.L14:
	ldr	r3, [fp, #-16]
	ldr	r2, [r3, #0]
	ldr	r3, [fp, #-16]
	ldrh	r3, [r3, #4]
	add	r3, r2, r3
	ldrb	r3, [r3, #0]	@ zero_extendqisi2
	str	r3, [fp, #-20]
.L13:
	ldr	r3, [fp, #-20]
	mov	r0, r3
	sub	sp, fp, #12
	ldmfd	sp, {fp, sp, pc}
	.size	byteBufferPeek, .-byteBufferPeek
	.global	__modsi3
	.align	2
	.global	byteBufferGet
	.type	byteBufferGet, %function
byteBufferGet:
	@ args = 0, pretend = 0, frame = 12
	@ frame_needed = 1, uses_anonymous_args = 0
	mov	ip, sp
	stmfd	sp!, {fp, ip, lr, pc}
	sub	fp, ip, #4
	sub	sp, sp, #12
	str	r0, [fp, #-20]
	ldr	r3, [fp, #-20]
	cmp	r3, #0
	bne	.L18
	mvn	r3, #0
	str	r3, [fp, #-24]
	b	.L20
.L18:
	ldr	r0, [fp, #-20]
	bl	byteBufferCount(PLT)
	mov	r3, r0
	cmp	r3, #0
	bne	.L21
	mvn	r3, #3
	str	r3, [fp, #-24]
	b	.L20
.L21:
	ldr	r3, [fp, #-20]
	ldr	r2, [r3, #0]
	ldr	r3, [fp, #-20]
	ldrh	r3, [r3, #4]
	add	r3, r2, r3
	ldrb	r3, [r3, #0]	@ zero_extendqisi2
	str	r3, [fp, #-16]
	ldr	r3, [fp, #-20]
	ldr	r2, [r3, #0]
	ldr	r3, [fp, #-20]
	ldrh	r3, [r3, #4]
	add	r2, r2, r3
	mov	r3, #0
	strb	r3, [r2, #0]
	ldr	r3, [fp, #-20]
	ldrh	r3, [r3, #4]
	add	r2, r3, #1
	ldr	r3, [fp, #-20]
	ldrh	r3, [r3, #8]
	mov	r0, r2
	mov	r1, r3
	bl	__modsi3(PLT)
	mov	r3, r0
	mov	r3, r3, asl #16
	mov	r2, r3, lsr #16
	ldr	r3, [fp, #-20]
	strh	r2, [r3, #4]	@ movhi
	ldr	r2, [fp, #-20]
	mov	r3, #0
	strb	r3, [r2, #10]
	ldr	r3, [fp, #-16]
	str	r3, [fp, #-24]
.L20:
	ldr	r3, [fp, #-24]
	mov	r0, r3
	sub	sp, fp, #12
	ldmfd	sp, {fp, sp, pc}
	.size	byteBufferGet, .-byteBufferGet
	.align	2
	.global	byteBufferPut
	.type	byteBufferPut, %function
byteBufferPut:
	@ args = 0, pretend = 0, frame = 12
	@ frame_needed = 1, uses_anonymous_args = 0
	mov	ip, sp
	stmfd	sp!, {fp, ip, lr, pc}
	sub	fp, ip, #4
	sub	sp, sp, #12
	str	r0, [fp, #-16]
	mov	r3, r1
	strb	r3, [fp, #-20]
	ldr	r3, [fp, #-16]
	cmp	r3, #0
	bne	.L25
	mvn	r3, #0
	str	r3, [fp, #-24]
	b	.L27
.L25:
	ldr	r0, [fp, #-16]
	bl	byteBufferCount(PLT)
	mov	r2, r0
	ldr	r3, [fp, #-16]
	ldrh	r3, [r3, #8]
	cmp	r2, r3
	bne	.L28
	mvn	r3, #4
	str	r3, [fp, #-24]
	b	.L27
.L28:
	ldr	r3, [fp, #-16]
	ldr	r2, [r3, #0]
	ldr	r3, [fp, #-16]
	ldrh	r3, [r3, #6]
	add	r2, r2, r3
	ldrb	r3, [fp, #-20]
	strb	r3, [r2, #0]
	ldr	r3, [fp, #-16]
	ldrh	r3, [r3, #6]
	add	r2, r3, #1
	ldr	r3, [fp, #-16]
	ldrh	r3, [r3, #8]
	mov	r0, r2
	mov	r1, r3
	bl	__modsi3(PLT)
	mov	r3, r0
	mov	r3, r3, asl #16
	mov	r2, r3, lsr #16
	ldr	r3, [fp, #-16]
	strh	r2, [r3, #6]	@ movhi
	ldr	r3, [fp, #-16]
	ldrh	r2, [r3, #6]
	ldr	r3, [fp, #-16]
	ldrh	r3, [r3, #4]
	cmp	r2, r3
	movne	r3, #0
	moveq	r3, #1
	ldr	r2, [fp, #-16]
	strb	r3, [r2, #10]
	mov	r3, #0
	str	r3, [fp, #-24]
.L27:
	ldr	r3, [fp, #-24]
	mov	r0, r3
	sub	sp, fp, #12
	ldmfd	sp, {fp, sp, pc}
	.size	byteBufferPut, .-byteBufferPut
	.align	2
	.global	byteBufferCount
	.type	byteBufferCount, %function
byteBufferCount:
	@ args = 0, pretend = 0, frame = 8
	@ frame_needed = 1, uses_anonymous_args = 0
	mov	ip, sp
	stmfd	sp!, {fp, ip, lr, pc}
	sub	fp, ip, #4
	sub	sp, sp, #8
	str	r0, [fp, #-16]
	ldr	r3, [fp, #-16]
	cmp	r3, #0
	bne	.L32
	mvn	r3, #0
	str	r3, [fp, #-20]
	b	.L34
.L32:
	ldr	r3, [fp, #-16]
	ldrh	r2, [r3, #6]
	ldr	r3, [fp, #-16]
	ldrh	r3, [r3, #4]
	cmp	r2, r3
	bcc	.L35
	ldr	r3, [fp, #-16]
	ldrb	r3, [r3, #10]	@ zero_extendqisi2
	cmp	r3, #0
	bne	.L37
	ldr	r3, [fp, #-16]
	ldrh	r3, [r3, #6]
	mov	r2, r3
	ldr	r3, [fp, #-16]
	ldrh	r3, [r3, #4]
	rsb	r2, r3, r2
	str	r2, [fp, #-20]
	b	.L34
.L37:
	ldr	r3, [fp, #-16]
	ldrh	r3, [r3, #8]
	str	r3, [fp, #-20]
	b	.L34
.L35:
	ldr	r3, [fp, #-16]
	ldrh	r3, [r3, #8]
	mov	r2, r3
	ldr	r3, [fp, #-16]
	ldrh	r3, [r3, #4]
	rsb	r2, r3, r2
	ldr	r3, [fp, #-16]
	ldrh	r3, [r3, #6]
	add	r2, r2, r3
	str	r2, [fp, #-20]
.L34:
	ldr	r3, [fp, #-20]
	mov	r0, r3
	sub	sp, fp, #12
	ldmfd	sp, {fp, sp, pc}
	.size	byteBufferCount, .-byteBufferCount
	.align	2
	.global	byteBufferEmpty
	.type	byteBufferEmpty, %function
byteBufferEmpty:
	@ args = 0, pretend = 0, frame = 8
	@ frame_needed = 1, uses_anonymous_args = 0
	mov	ip, sp
	stmfd	sp!, {fp, ip, lr, pc}
	sub	fp, ip, #4
	sub	sp, sp, #8
	str	r0, [fp, #-16]
	ldr	r3, [fp, #-16]
	cmp	r3, #0
	bne	.L41
	mvn	r3, #0
	str	r3, [fp, #-20]
	b	.L43
.L41:
	ldr	r3, [fp, #-16]
	ldrb	r3, [r3, #10]	@ zero_extendqisi2
	cmp	r3, #0
	bne	.L44
	ldr	r3, [fp, #-16]
	ldrh	r2, [r3, #4]
	ldr	r3, [fp, #-16]
	ldrh	r3, [r3, #6]
	cmp	r2, r3
	bne	.L44
	mov	r3, #1
	str	r3, [fp, #-20]
	b	.L43
.L44:
	mov	r3, #0
	str	r3, [fp, #-20]
.L43:
	ldr	r3, [fp, #-20]
	mov	r0, r3
	sub	sp, fp, #12
	ldmfd	sp, {fp, sp, pc}
	.size	byteBufferEmpty, .-byteBufferEmpty
	.align	2
	.global	byteBufferFull
	.type	byteBufferFull, %function
byteBufferFull:
	@ args = 0, pretend = 0, frame = 8
	@ frame_needed = 1, uses_anonymous_args = 0
	mov	ip, sp
	stmfd	sp!, {fp, ip, lr, pc}
	sub	fp, ip, #4
	sub	sp, sp, #8
	str	r0, [fp, #-16]
	ldr	r3, [fp, #-16]
	cmp	r3, #0
	bne	.L49
	mvn	r3, #0
	str	r3, [fp, #-20]
	b	.L51
.L49:
	ldr	r3, [fp, #-16]
	ldrb	r3, [r3, #10]	@ zero_extendqisi2
	cmp	r3, #0
	beq	.L52
	ldr	r3, [fp, #-16]
	ldrh	r2, [r3, #4]
	ldr	r3, [fp, #-16]
	ldrh	r3, [r3, #6]
	cmp	r2, r3
	bne	.L52
	mov	r3, #1
	str	r3, [fp, #-20]
	b	.L51
.L52:
	mov	r3, #0
	str	r3, [fp, #-20]
.L51:
	ldr	r3, [fp, #-20]
	mov	r0, r3
	sub	sp, fp, #12
	ldmfd	sp, {fp, sp, pc}
	.size	byteBufferFull, .-byteBufferFull
	.ident	"GCC: (GNU) 4.0.2"
