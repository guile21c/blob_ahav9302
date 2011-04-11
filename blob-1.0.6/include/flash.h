/*-------------------------------------------------------------------------
 * Filename:      flash.h
 * Version:       $Id: flash.h,v 1.10 1999/10/03 22:03:52 erikm Exp $
 * Copyright:     Copyright (C) 1999, Jan-Derk Bakker
 * Author:        Jan-Derk Bakker <J.D.Bakker@its.tudelft.nl>
 * Description:   Flash I/O functions for the blob loader
 * Created at:    Mon Aug 23 20:00:00 1999
 * Modified by:   Erik Mouw <J.A.K.Mouw@its.tudelft.nl>
 * Modified at:   Sun Oct  3 21:35:12 1999
 *-----------------------------------------------------------------------*/
/*
 * flash.c: Flash I/O functions for the blob bootloader
 *
 * Copyright (C) 1999  Jan-Derk Bakker (J.D.Bakker@its.tudelft.nl)
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

#ident "$Id: flash.h,v 1.10 1999/10/03 22:03:52 erikm Exp $"

#ifndef BLOB_FLASH_H
#define BLOB_FLASH_H

#include "types.h"

#define NUM_FLASH_BLOCKS	(31)
#define FLASH_BLOCK_BASE	((u32 *) 0x020000)
#define FLASH_BLOCK_SIZE	((u32) 0x020000)
#define	BLOCK_NAME_LEN	(64)
#define	NO_BLOCK	((u8) 0xFF)
#define	BLOCK_HDR_SIG	((u32) 'LART')
#define	BLOCK_EMPTY_SIG	((u32) 0xFFFFFFFF)

#define BLOCK_IN_USE(x) (x.signature == BLOCK_HDR_SIG)

typedef	enum {
	blRamdisk, blKernel
} tBlockType;

static inline int RunningFromInternal(void) {
	
	if(((*(u32 *)0xA0000010) & 0x04) == 0)
		return 1;
	else
		return 0;
}

#define MAIN_BLOCK_SIZE	(32768 * 4)

#define CS0_BASE	(0x00000000)
#define	CS1_BASE	(0x08000000)

#define INT_FLASH_BASE (RunningFromInternal() ? CS0_BASE : CS1_BASE)

#define	KERNEL_START		(INT_FLASH_BASE + MAIN_BLOCK_SIZE)
#define NUM_KERNEL_BLOCKS	(7)
#define	KERNEL_LEN		(NUM_KERNEL_BLOCKS * MAIN_BLOCK_SIZE)
#define INITRD_START		(KERNEL_START + KERNEL_LEN)
#define NUM_INITRD_BLOCKS	(24)
#define INITRD_LEN		(NUM_INITRD_BLOCKS * MAIN_BLOCK_SIZE)

typedef struct {
	u32	signature;
	char	name[BLOCK_NAME_LEN];
	tBlockType	type;
	u32	seqNum, totSeq;
	u32	bytesInBlock;
	u32	defLoadAddr;
} tBlkHdr;

typedef struct {
	tBlkHdr	headers[NUM_FLASH_BLOCKS];
	u8	firstBlockIndex[NUM_FLASH_BLOCKS];
} tBlkInfo;

void ScanFlash(tBlkInfo *blocksInfo);
void LoadBlocksToMem(tBlkInfo *blocksInfo, u8 which, u32 *baseAddr);
void EraseBlocks(tBlockType which);
void WriteBlocksFromMem(tBlockType type, const u32 *source, int length);

#endif
