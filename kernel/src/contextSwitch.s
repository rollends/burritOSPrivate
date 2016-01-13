

.section .text
.global asm_contextSwitch
.type asm_contextSwitch, %function
asm_contextSwitch:	
	@ asm_contextSwitch( void *(*Task_PC) , unsigned int* Task_PSR, unsigned int** Task_Stack )
	
	STMFD	SP!, {R4 - R12, LR}

	@ Stack kernel parameters.
	STMFD	SP!, {R0 - R3}

	@ Load Task state into appropriate registers
	LDR	LR, [R0]
	LDR	R1, [R1]
	MSR	SPSR, R1

	@ Enter System Mode to access User Registers
	MRS	R1, CPSR
	ORR	R1, R1, #0x1F
	MSR	CPSR, R1

	LDR	SP, [R2]
	LDMFD	SP!, {R4 - R12, LR}	@ Unstack tasks's registers

	@ Enter Supervisor Mode
	MRS	R1, CPSR
	AND	R1, R1, #0x13
	MSR	CPSR, R1

	MOVS	PC, LR			@ Branch to User Task whilst copying SPSR_svc to CPSR

kernel_entry:
	@ Mode Switch into System Mode to access User Registers
	MRS	R1, CPSR		@ Read CPSR
	ORR	R1, R1, #0x1F		@ Mode Switch into System Mode
	MSR	CPSR, R1		@ Enter System Mode so that we can access User Registers

	@ Stack User Registers
	STMFD	SP!, {R4 - R12, LR}
	MOV	R0, SP

	@ Mode Switch Into Supervisor Mode
	MRS	R1, CPSR
	AND	R1, R1, #0x13		@ Mode bits for Supervisor Mode (Assuming we were in System Mode)
	MSR	CPSR, R1		@ Enter Supervisor Mode

	@ Unstack Kernel Parameters...
	LDMFD	SP!, {R1 - R3}
	
	@ Store Task special values...for when we switch back (Stack, PSR, PC)
	STR	R0, [R3]
	MRS	R3, SPSR
	STR	R3, [R2]
	STR	LR, [R1]

	LDMFD 	SP!, {R4 - R12, PC}	@ Return to previous Kernel State before Context Switch
