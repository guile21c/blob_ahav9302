###########################################################################
## Filename:      start.S
## Version:       $Id: start.S,v 1.15 1999/10/03 22:03:52 erikm Exp $
## Copyright:     Copyright (C) 1999, Erik Mouw
## Author:        Erik Mouw <J.A.K.Mouw@its.tudelft.nl>
## Description:   blob start code
## Created at:    Sun Jul 18 20:29:08 1999
## Modified by:   Erik Mouw <J.A.K.Mouw@its.tudelft.nl>
## Modified at:   Sun Oct  3 21:07:36 1999
###########################################################################
/*
 * start.S: blob start code
 *
 * Copyright (C) 1999  Erik Mouw (J.A.K.Mouw@its.tudelft.nl) and 
 *                     Jan-Derk Bakker (J.D.Bakker@its.tudelft.nl)
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 *
 */
/*
 * This is the blob start code. The SA-1100 jumps to address 0x00000000
 * after a reset. We put a single branch code at this position which jumps
 * to a safe region to do the actual setup. All other vectors just point
 * to an endless loop for the moment.
 *
 * Documentation: 
 * [1] Intel Corporation, "Intel StrongARM SA-1100 Microprocessor
 *     Developer's Manual", April 1999
 * [2] S. Furber, "ARM System Architecture", Addison Wesley Longman
 *     Ltd., Essex, England, 1996.
 */

.ident "$Id: start.S,v 1.15 1999/10/03 22:03:52 erikm Exp $"

.text

/* extern function to blink the LED */
.extern blink_led


/* Jump vector table as in table 3.1 in [1] */
.globl _start
_start:	b	reset
	b	undefined_instruction
	b	software_interrupt
	b	abort_prefetch
	b	abort_data
	b	not_used
	b	irq
	b	fiq

	
	
	
reset:

old_blink_led:	
	/* Register addresses can be found in [1] Appendix A */

	/* Switch the SA-1100 to 220 MHz */
	mov	r1, #0x90000000
	add	r1, r1, #0x20000
	mov	r2, #0xB
	str	r2, [r1, #0x14]

	/* Set up the DRAM in banks 0 and 1 */
	mov	r1, #0xA0000000
	mov	r2,     #0xC7000000
	add	r2, r2, #0x001C0000
	add	r2, r2, #0x00007000
	add	r2, r2, #0x0000003F
	str	r2, [r1, #0x04]

	mov	r2,     #0xFF000000
	add	r2, r2, #0x00C70000
	add	r2, r2, #0x00001C00
	add	r2, r2, #0x00000071
	str	r2, [r1, #0x08]	

	mov	r2,     #0xFF000000
	add	r2, r2, #0x00FF0000
	add	r2, r2, #0x0000FF00
	add	r2, r2, #0x000000FF
	str	r2, [r1, #0x0C]	

	mov	r2,     #0x03000000
	add	r2, r2, #0x00340000
	add	r2, r2, #0x0000B200
	add	r2, r2, #0x00000023
	str	r2, [r1, #0x00]

	/* Try to transmit over the serial port III. */
	/* Do port setup the paranoid way */

	/* First, mask **ALL** interrupts */
	mov	r1, #0x90000000
	add	r1, r1, #0x50000
	mov	r2, #0x00
	str	r2, [r1, #0x4]

	/* Now clear all 'sticky' bits in serial III registers, cf. [1] 11.11 */
	mov	r1, #0x80000000
	add	r1, r1, #0x50000
	mov	r2, #0xFF
	str	r2, [r1, #0x1C]
	
	/* Set the serial port to sensible defaults: no break, no interrupts, */
	/* no parity, 8 databits, 1 stopbit. */
	mov	r2, #0x00
	str	r2, [r1, #0x0C]
	mov	r2, #0x08
	str	r2, [r1, #0x00]

	/* Set BRD to 5, for a baudrate of 38k4 ([1] 11.11.4.1) */
	/* Set BRD to 23, for a baudrate of 9k6 ([1] 11.11.4.1) */
	mov	r2, #0x00
	str	r2, [r1, #0x04]
	mov	r2, #23
	str	r2, [r1, #0x08]

	/* Initialize the GPDR (GPIO Pin Direction Register) in such a
	way that the LED is on an output port */

	/* load the GPIO base in r2 */	
	mov	r2, #0x90000000
	add	r2, r2, #0x40000

	/* Bit 23 is the LED, according to JDB */
	mov	r1, #0x00800000
	str	r1, [r2, #0x04]

	/* The LED controls FORCE_ON, so turn it on here */
	str     r1, [r2, #0x08]

	/* Prepare to send some characters out in a loop */
	mov	r1, #0x80000000
	add	r1, r1, #0x50000	
	
	/* Enable the transmitter */
	mov	r2, #0x02
	str	r2, [r1, #0x0C]

	/* Send out a welcome message */
	adr	r0, welcome
	bl	print_str

	/* Determine whether this code runs from main flash or external flash */
	/* Clue: Main flash is 32 bit, external is 16 bit. */
	mov	r1, #0xA0000000
	ldr	r0, [r1, #0x10]
	ands	r0, r0, #0x04
	bne	cpy_ext2int

	/* Put the main flash to the correct speed */
	mov	r2,     #0xAD000000
	add	r2, r2, #0x006C0000
	add	r2, r2, #0x00004800
	add	r2, r2, #0x00000068
	str	r2, [r1, #0x10]

	adr	r0, int_flash_str
	bl	print_str

test_mem:
	/* Start the memory tester, hardcoded bank 0 & 1 for now */
	adr	r0, start_test
	bl	print_str

	/* Erase memory */
	adr	r0, zeroing_mem
	bl	print_str

	mov	r4, #0xC0000000
	mov	r5, #0xD0000000
	mov	r6, #0x400000
	sub	r6, r6, #0x01

	mov	r0, r4
	bl	print_hex

zero_loop:
	mov	r0, #0x0
	mov	r1, #0x0
	mov	r2, #0x0
	mov	r3, #0x0
	mov	r7, #0x0
	mov	r8, #0x0
	mov	r9, #0x0
	mov	r10, #0x0
zero_loop1:
	stmia	r4!, {r0-r3, r7-r10}
	stmia	r4!, {r0-r3, r7-r10}
	stmia	r4!, {r0-r3, r7-r10}
	stmia	r4!, {r0-r3, r7-r10}
	tst	r4, r6
	bne	zero_loop1
	adr	r0, del10
	bl	print_str
	mov	r0, r4
	bl	print_hex
	cmp	r4, r5
	blt	zero_loop

	adr	r0, zeroing_done
	bl	print_str

	mov	r4, #0xC0000000
	mov	r5, #0xD0000000
	mov	r6, r4 /* Base for current block */
	mov	r7, #0x0 /* Diff between r4 and [r4] */

	/* Go hunting for aliases */
alias_loop:
	ldr	r8, [r4]
	cmp	r8, #0x0
	streq	r4, [r4] /* If the value was zero, load r4 to it */
	moveq	r8, r4
	sub	r9, r4, r8
	cmp	r9, r7 /* Is this a new block ? */
	bne	alias_detected
	add	r4, r4, #0x1000
	cmp	r4, r5
	blt	alias_loop	
	b	alias_done

alias_detected:

#	adr	r0, block_at_str
#	bl	print_str
#	mov	r0, r6
#	bl	print_hex
#	adr	r0, length_str
#	bl	print_str
#	sub	r0, r4, r6
#	bl	print_hex
	sub	r8, r6, r7
	cmp	r7, #0x0	/* Is this a 'new' block ? */
	moveq	r10, r6		/* If so, remember the start of this block...*/ 
	moveq	r11, r4		/* ...and the start of the next block */
	mov	r6, r4
	mov	r7, r9
	beq	alias_loop
#	adr	r0, alias_for_str
#	bl	print_str
#	mov	r0, r8
#	bl	print_hex
	b	alias_loop

alias_done:

	/* Set up the stack pointer */
	sub	sp, r11, #0x04

	/* Arg 0 of the C code is the start of the block it can use; */
	/* arg 1 is the size of that block. */
	mov	r0, r10
	sub	r1, r11, r10

	/* Jump to the C code */
jump_to_c:
	bl	c_main

	/* The c code should never return ! */
	b	reset


blinky:
	/* The old blinker */
	mov     r2, #0x90000000
	add     r2, r2, #0x40000
	mov     r1, #0x00800000

old_led_on:
	/* turn on the LED by writing the GPSR (GPIO Pin output Set
	Register) */
	str	r1, [r2, #0x08]
	mov	r4, #0x10000
loop1:
	subs	r4, r4, #1
	bne	loop1

old_led_off:
	/* turn off the LED by writing the GPCR (GPIO Pin output Clear
	Register) */
	str	r1, [r2, #0x0c]

	mov     r4, #0x10000
loop2:
	subs    r4, r4, #1
	bne	loop2

	/* and loop forever */
	b	old_led_on


.align 4
welcome:
	.string "\n\r\n\rConsider yourself LARTed!\n\r"

.align 4
int_flash_str:
	.string "Running from internal Flash.\n\r"

.align 4
start_test:
	.string "Starting the memory tester...\n\r"

.align 4
zeroing_mem:
	.string "Zeroing memory..."

.align 4
zeroing_done:
	.string "\n\rZeroing done. Testing for aliases...\n\r"

.align 4
del10:
	.string "\b\b\b\b\b\b\b\b\b\b"

.align 4
block_at_str:
	.string "\n\rBlock at "

.align 4
length_str:
	.string " with length "

.align 4
alias_for_str:
	.string " is an alias of "
.align 4

	/* Subroutine that sends a string over the serial port */
	/* The address of the string should be in r0 */
print_str:
	/* Save the return address */
	mov	r13, r14
	mov	r2, r0
prs1:
	ldrsb	r0, [r2]
	add	r2, r2, #0x01
	ands	r0, r0, #0xFF
	beq	prs2
	bl	print_byte
	b	prs1

prs2:
	/* Return */
	mov	pc, r13


	/* Subroutine to send a hex word (in r0) over the serial port */
print_hex:
	mov	r13, r14
	mov	r2, r0
	mov	r3, #0x08
	mov	r0, #0x30
	bl	print_byte
	mov	r0, #0x78
	bl	print_byte
prh1:
	and	r0, r2, #0xF0000000
	mov	r0, r0, lsr #28
	add	r0, r0, #0x30
	cmp	r0, #0x3A
	addge	r0, r0, #0x07
	bl	print_byte
	mov	r2, r2, lsl #4
	subs	r3, r3, #0x01	
	bne	prh1

	mov	pc, r13

	/* Subroutine that sends a byte over the serial port. */
	/* The byte is in r0 */
print_byte:
	/* Wait for room in the tx fifo */
	mov     r1, #0x80000000
	add     r1, r1, #0x50000
	ldr     r1, [r1, #0x1C]
	ands    r1, r1, #0x01
	beq	print_byte

	mov     r1, #0x80000000
	add     r1, r1, #0x50000
	str	r0, [r1, #0x14]
	mov	pc, r14
	

cpy_ext2int:

	/* Put the Flash to the correct speed */
	mov	r1, #0xA0000000
	mov	r2,     #0x48000000
	add	r2, r2, #0x00680000
	add	r2, r2, #0x0000AD00
	add	r2, r2, #0x0000006C
	str	r2, [r1, #0x10]

	adr	r0, ext_flash_str
	bl	print_str

	/* Get the ID of the internal flash */
	mov	r0, #0x0090
	add	r0, r0, #0x00900000
	bl	data_to_flash
	mov	r4, #0x08000000
	str	r0, [r4]
	ldr	r0, [r4]
	bl	data_from_flash
	mov	r5, r0
	ldr	r0, [r4, #0x04]
	bl	data_from_flash
	mov	r6, r0
	mov	r3, #0x0089
	add	r3, r3, #0x00890000
	cmp	r3, r5
	beq	flash_man_ok
	adr	r0, int_unknown_str
	bl	print_str
	mov	r0, r5
	bl	print_hex
	b	cpy_exit
flash_man_ok:
	adr	r0, int_intel_str
	bl	print_str
	mov	r3,     #0x88000000
	add	r3, r3, #0x00F40000
	add	r3, r3, #0x00008800
	add	r3, r3, #0x000000F4
	cmp	r3, r6
	beq	flash_dev_ok
	adr	r0, int_unknown2_str
	bl	print_str
	mov	r0, r6
	bl	print_hex
	b	cpy_exit
flash_dev_ok:
	adr	r0, int_16m_b_str
	bl	print_str

	/* Find the size of the data in the external flash */
	mov	r5, #0x020000  /* Sod it, we hardcode it to 128k */

	/* Erase blocks in the internal flash, when needed */
	mov	r4, #0x08000000
	add	r6, r4, r5

	mov	r0, #0x00500000
	add	r0, r0, #0x00000050
	bl	data_to_flash
	str	r0, [r4]

erase_loop:
	/* switch to read array mode, if we're not there already */
	mov	r0, #0x00FF0000
	add	r0, r0, #0x000000FF
	bl	data_to_flash
	str	r0, [r4]
	ldr	r0, [r4]
	cmn	r0, #0x01
	beq	no_erase
	adr	r0, erase_str
	bl	print_str
	mov	r0, r4
	bl	print_hex
	mov	r0, #0x00200000
	add	r0, r0, #0x00000020
	bl	data_to_flash
	str	r0, [r4]
	mov	r0, #0x00D00000
	add	r0, r0, #0x000000D0
	bl	data_to_flash
	str	r0, [r4]
	bl	wait_status
	mov	r3, #0x00200000
	add	r3, r3, #0x00000020
	tst	r0, r3
	beq	erase_ok
	bl	print_hex
	adr	r0, erase_err_str
	bl	print_str
	b	cpy_exit
erase_ok:
	adr	r0, erase_end_str
	bl	print_str
no_erase:
	add	r4, r4, #0x04
	cmp	r4, r6
	blt	erase_loop

	/* Write blocks to the internal flash */
	mov	r4, #0x00

	mov	r0, #0x00500000
	add	r0, r0, #0x00000050
	bl	data_to_flash
	str	r0, [r4]

	adr	r0, write_start_str
	bl	print_str

write_loop:
	ldr	r7, [r4]
	cmn	r7, #0x01
	beq	no_write
	add	r6, r4, #0x08000000
	mov	r0, #0x00400000
	add	r0, r0, #0x00000040
	bl	data_to_flash
	str	r0, [r6]
	str	r7, [r6]
	bl	wait_status
	mov	r3, #0x00100000
	add	r3, r3, #0x00000010
	tst	r0, r3
	beq	no_write
	mov	r7, r0
	adr	r0, write_err1_str
	bl	print_str
	mov	r0, r7
	bl	print_hex
	adr	r0, write_err2_str
	bl	print_str
	mov	r0, r6
	bl	print_hex
	b	cpy_exit
no_write:
	add	r4, r4, #0x04
	cmp	r4, r5
	blt	write_loop

	/* Verify blocks written to the internal flash */
	mov	r4, #0x00
	add	r6, r4, #0x08000000

	mov	r0, #0x00500000
	add	r0, r0, #0x00000050
	bl	data_to_flash
	str	r0, [r6]

	mov	r0, #0x00FF0000
	add	r0, r0, #0x000000FF
	bl	data_to_flash
	str	r0, [r6]

	adr	r0, vrfy_start_str
	bl	print_str

vrfy_loop:
	ldr	r7, [r4]
	add	r6, r4, #0x08000000
	ldr	r8, [r6]
	cmp	r7, r8
	beq	next_vrfy
	adr	r0, vrfy_err1_str
	bl	print_str
	mov	r0, r6
	bl	print_hex
	adr	r0, vrfy_err2_str
	bl	print_str
	mov	r0, r7
	bl	print_hex
	adr	r0, vrfy_err3_str
	bl	print_str
	mov	r0, r8
	bl	print_hex
	b	cpy_exit
next_vrfy:
	add	r4, r4, #0x04
	cmp	r4, r5
	blt	vrfy_loop

	adr	r0, cpy_done_str
	bl	print_str

cpy_exit:
	b	test_mem

.align 4
ext_flash_str:
	.string "Running from external flash.\n\r"

.align 4
int_intel_str:
	.string "Internal flash manufacturer: Intel.\n\r"

.align 4
int_unknown_str:
	.string "Error: Unknown internal flash manufacturer ID "

.align 4
int_16m_b_str:
	.string "Internal flash devices: 2 x 28F160F3 16Mbit bottom flash.\n\r"

.align 4
int_unknown2_str:
	.string "Error: Unknown internal flash devices ID "

.align 4
erase_str:
	.string "Erasing block at "

.align 4
erase_err_str:
	.string "  <- error !\n\r"

.align 4
erase_end_str:
	.string " done.\n\r"

.align 4
write_start_str:
	.string "Writing data...\n\r"

.align 4
write_err1_str:
	.string "Error "

.align 4
write_err2_str:
	.string " at "

.align 4
vrfy_start_str:
	.string "Verifying data...\n\r"

.align 4
vrfy_err1_str: 
	.string "Verify error at address "

.align 4
vrfy_err2_str:
	.string "; expected : "

.align 4
vrfy_err3_str:
	.string	"; found : "

.align 4
cpy_done_str:
	.string "Copying done.\n\r"

.align 4
	/* Wait for the status of *both* internal flash devices to be ready. */
	/* Returns the status in r0. */
wait_status:
	mov	r13, r14
wait_status_loop:
	mov	r0, #0x00700000
	add	r0, r0, #0x0070
	bl	data_to_flash
	mov	r1, #0x08000000
	str	r0, [r1]
	ldr	r0, [r1]
	bl	data_from_flash
	tst	r0, #0x00000080
	tstne	r0, #0x00800000
	beq	wait_status_loop
	mov	pc, r13

.globl	data_to_flash
.globl	data_from_flash
	/* Subroutine that takes data in r0 and formats it so it will be in */
	/* the correct order for the internal flash */
data_to_flash:
	mov	r1, #0x0
	
	tst	r0, #0x00000001
	orrne	r1, r1, #0x00001000
	tst	r0, #0x00000002
	orrne	r1, r1, #0x00004000
	tst	r0, #0x00000004
	orrne	r1, r1, #0x00000800
	tst	r0, #0x00000008
	orrne	r1, r1, #0x00000200
	tst	r0, #0x00000010
	orrne	r1, r1, #0x00000001
	tst	r0, #0x00000020
	orrne	r1, r1, #0x00000004
	tst	r0, #0x00000040
	orrne	r1, r1, #0x00000080
	tst	r0, #0x00000080
	orrne	r1, r1, #0x00000020

	tst	r0, #0x00000100
	orrne	r1, r1, #0x00002000
	tst	r0, #0x00000200
	orrne	r1, r1, #0x00008000
	tst	r0, #0x00000400
	orrne	r1, r1, #0x00000400
	tst	r0, #0x00000800
	orrne	r1, r1, #0x00000100
	tst	r0, #0x00001000
	orrne	r1, r1, #0x00000002
	tst	r0, #0x00002000
	orrne	r1, r1, #0x00000008
	tst	r0, #0x00004000
	orrne	r1, r1, #0x00000040
	tst	r0, #0x00008000
	orrne	r1, r1, #0x00000010

	tst	r0, #0x00010000
	orrne	r1, r1, #0x00100000
	tst	r0, #0x00020000
	orrne	r1, r1, #0x00400000
	tst	r0, #0x00040000
	orrne	r1, r1, #0x00080000
	tst	r0, #0x00080000
	orrne	r1, r1, #0x00020000
	tst	r0, #0x00100000
	orrne	r1, r1, #0x01000000
	tst	r0, #0x00200000
	orrne	r1, r1, #0x04000000
	tst	r0, #0x00400000
	orrne	r1, r1, #0x80000000
	tst	r0, #0x00800000
	orrne	r1, r1, #0x20000000

	tst	r0, #0x01000000
	orrne	r1, r1, #0x00200000
	tst	r0, #0x02000000
	orrne	r1, r1, #0x00800000
	tst	r0, #0x04000000
	orrne	r1, r1, #0x00040000
	tst	r0, #0x08000000
	orrne	r1, r1, #0x00010000
	tst	r0, #0x10000000
	orrne	r1, r1, #0x02000000
	tst	r0, #0x20000000
	orrne	r1, r1, #0x08000000
	tst	r0, #0x40000000
	orrne	r1, r1, #0x40000000
	tst	r0, #0x80000000
	orrne	r1, r1, #0x10000000

	mov	r0, r1
	mov	pc, r14

	/* Takes data received from the flash, and unshuffles it. */
data_from_flash:
	mov	r1, #0x00

	tst	r0, #0x00000001
	orrne	r1, r1, #0x00000010
	tst	r0, #0x00000002
	orrne	r1, r1, #0x00001000
	tst	r0, #0x00000004
	orrne	r1, r1, #0x00000020
	tst	r0, #0x00000008
	orrne	r1, r1, #0x00002000
	tst	r0, #0x00000010
	orrne	r1, r1, #0x00008000
	tst	r0, #0x00000020
	orrne	r1, r1, #0x00000080
	tst	r0, #0x00000040
	orrne	r1, r1, #0x00004000
	tst	r0, #0x00000080
	orrne	r1, r1, #0x00000040

	tst	r0, #0x00000100
	orrne	r1, r1, #0x00000800
	tst	r0, #0x00000200
	orrne	r1, r1, #0x00000008
	tst	r0, #0x00000400
	orrne	r1, r1, #0x00000400
	tst	r0, #0x00000800
	orrne	r1, r1, #0x00000004
	tst	r0, #0x00001000
	orrne	r1, r1, #0x00000001
	tst	r0, #0x00002000
	orrne	r1, r1, #0x00000100
	tst	r0, #0x00004000
	orrne	r1, r1, #0x00000002
	tst	r0, #0x00008000
	orrne	r1, r1, #0x00000200

	tst	r0, #0x00010000
	orrne	r1, r1, #0x08000000
	tst	r0, #0x00020000
	orrne	r1, r1, #0x00080000
	tst	r0, #0x00040000
	orrne	r1, r1, #0x04000000
	tst	r0, #0x00080000
	orrne	r1, r1, #0x00040000
	tst	r0, #0x00100000
	orrne	r1, r1, #0x00010000
	tst	r0, #0x00200000
	orrne	r1, r1, #0x01000000
	tst	r0, #0x00400000
	orrne	r1, r1, #0x00020000
	tst	r0, #0x00800000
	orrne	r1, r1, #0x02000000

	tst	r0, #0x01000000
	orrne	r1, r1, #0x00100000
	tst	r0, #0x02000000
	orrne	r1, r1, #0x10000000
	tst	r0, #0x04000000
	orrne	r1, r1, #0x00200000
	tst	r0, #0x08000000
	orrne	r1, r1, #0x20000000
	tst	r0, #0x10000000
	orrne	r1, r1, #0x80000000
	tst	r0, #0x20000000
	orrne	r1, r1, #0x00800000
	tst	r0, #0x40000000
	orrne	r1, r1, #0x40000000
	tst	r0, #0x80000000
	orrne	r1, r1, #0x00400000

	mov	r0, r1
	mov	pc, r14
	


undefined_instruction:
	b	undefined_instruction



	
software_interrupt:
	b	software_interrupt




abort_prefetch:
	b	abort_prefetch




abort_data:
	b	abort_data




not_used:
	b	not_used



	
irq:
	b	irq




fiq:
	b	fiq
