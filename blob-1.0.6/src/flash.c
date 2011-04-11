/*-------------------------------------------------------------------------
 * Filename:      flash.c
 * Version:       $Id: flash.c,v 1.14 2000/01/15 18:51:11 erikm Exp $
 * Copyright:     Copyright (C) 1999, Jan-Derk Bakker
 * Author:        Jan-Derk Bakker <J.D.Bakker@its.tudelft.nl>
 * Description:   Flash I/O functions for blob
 * Created at:    Mon Aug 23 20:00:00 1999
 * Modified by:   Erik Mouw <J.A.K.Mouw@its.tudelft.nl>
 * Modified at:   Sat Jan 15 19:16:34 2000
 *-----------------------------------------------------------------------*/
/*
 * flash.c: Flash I/O functions for blob
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

#ident "$Id: flash.c,v 1.14 2000/01/15 18:51:11 erikm Exp $"

#ifdef HAVE_CONFIG_H
# include "config.h"
#endif

#include "main.h"
#include "util.h"
#include "serial.h"
#include "flash.h"

/* define for advanced flash functionality (not that it works) -- Erik */
/* #define ADVANCED_FLASH 1 */




/* Static function defs */
static u32 EraseOne(const char *whichOne);
#ifdef ADVANCED_FLASH
static void GetBlockOrder(tBlkInfo *blocksInfo, u8 which, 
			  u8 blockOrder[NUM_FLASH_BLOCKS]);
#endif




#ifdef ADVANCED_FLASH
void ScanFlash(tBlkInfo *blocksInfo)
{
/* 	Scans the flash for headers in the main blocks. Maybe in the future we will 
	put the headers in one of the parameter blocks. */
	int i,j;
	
	j = 0;
	for(i = 0; i < NUM_FLASH_BLOCKS; i++) {
		MyMemCpy((u32 *) &(blocksInfo->headers[i]), 
			FLASH_BLOCK_BASE + i * FLASH_BLOCK_SIZE,
			sizeof(tBlkHdr) / 4);
		/* Is this a 'first' block ? */
		if(BLOCK_IN_USE(blocksInfo->headers[i]) 
			&& (blocksInfo->headers[i].seqNum == 0))
			blocksInfo->firstBlockIndex[j++] = i;
	}
	
	for(; j < NUM_FLASH_BLOCKS; j++)
		blocksInfo->firstBlockIndex[j] = NO_BLOCK;
} /* ScanFlash */
#endif	




#ifdef ADVANCED_FLASH
void LoadBlocksToMem(tBlkInfo *blocksInfo, u8 which, u32 *baseAddr)
{
/* Load a series of blocks to memory */
	u8	blockOrder[NUM_FLASH_BLOCKS];
	int	numBlocks = blocksInfo->headers[which].totSeq;
	int	i;
	u32	*srcAddr, *dstAddr = baseAddr;
	
	/* If the block isn't in use, fail silently. */
	if(!BLOCK_IN_USE(blocksInfo->headers[which]))
		return;
	
	GetBlockOrder(blocksInfo, which, blockOrder);
				
	for(i = 0; i < numBlocks; i++) {
		srcAddr = FLASH_BLOCK_BASE + 
			blockOrder[i] * FLASH_BLOCK_SIZE + sizeof(tBlkHdr);
		MyMemCpy(dstAddr, srcAddr, blocksInfo->headers[i].bytesInBlock / 4);
		dstAddr += blocksInfo->headers[i].bytesInBlock / 4;
	}
} /* LoadBlocksToMem */
#endif




/* The spooky functions that write to the same flash that we're executing from */

u32	data_from_flash(u32 what);
u32	data_to_flash(u32 what);

#define READ_ARRAY	0x00FF00FF
#define ERASE_SETUP	0x00200020
#define ERASE_CONFIRM	0x00D000D0
#define	PGM_SETUP	0x00400040
#define	STATUS_READ	0x00700070
#define	STATUS_CLEAR	0x00500050
#define STATUS_BUSY	0x00800080
#define STATUS_ERASE_ERR	0x00200020
#define STATUS_PGM_ERR	0x00100010




void EraseBlocks(tBlockType which) 
{
	char	*thisBlock;
	int	numBlocks, i;

	if(RunningFromInternal()) {
		SerialOutputString("*** Can't erase -- running from internal flash.\r");
		return;
	}
	
	if(which == blKernel) {
		thisBlock = (char *)KERNEL_START;
		numBlocks = NUM_KERNEL_BLOCKS;
	} else {
		thisBlock = (char *)INITRD_START;
		numBlocks = NUM_INITRD_BLOCKS;
	}

	for(i = 0; i < numBlocks; i++, thisBlock += MAIN_BLOCK_SIZE) {
		SerialOutputByte('.');
		if((EraseOne(thisBlock) & STATUS_ERASE_ERR) != 0) {
			SerialOutputString("\r*** Erase error at address 0x");
			SerialOutputHex((u32)thisBlock);
			SerialOutputByte('\r');
			return;
		}
	}
} /* EraseBlocks */




void WriteBlocksFromMem(tBlockType type, const u32 *source, int length)
{
	volatile u32	*flashBase;
	u32	result;
	int	maxLength, i;		
	
	if((u32)source & 0x03) {
		SerialOutputString("*** Source is not on a word boundary: 0x");
		SerialOutputHex((u32)source);
		SerialOutputByte('\r');
		return;
	}
	
	if(length & 0x03)
		length += 0x04;
	length &= ~((u32) 0x03);
	
	if(type == blKernel) {
		flashBase = (u32 *)KERNEL_START;
		maxLength = KERNEL_LEN;
	}
	else {
		flashBase = (u32 *)INITRD_START;
		maxLength = INITRD_LEN;
	}
	
	if(length > maxLength)
		length = maxLength;

#ifdef BLOB_DEBUG
	SerialOutputString(__FUNCTION__ "(): Flashing 0x");
	SerialOutputHex((u32)length);
	SerialOutputString(" bytes from 0x");
	SerialOutputHex((u32)source);
	SerialOutputString(" to 0x");
	SerialOutputHex((u32)flashBase);
	SerialOutputByte('\r');
#endif
	
	for(i = 0; i < length; i+= 4, flashBase++, source++) {
		if((i % MAIN_BLOCK_SIZE) == 0)
			SerialOutputByte('.');

		*flashBase = data_to_flash(PGM_SETUP);
		*flashBase = *source;

		do {
			*flashBase = data_to_flash(STATUS_READ);
			result = data_from_flash(*flashBase);
		} while((~result & STATUS_BUSY) != 0);
		
		*flashBase = data_to_flash(READ_ARRAY);

		if((result & STATUS_PGM_ERR) != 0 || *flashBase != *source) {
			SerialOutputString("\r*** Write error at address 0x");
			SerialOutputHex((u32)flashBase);
			SerialOutputByte('\r');
			return;
		}
	}
} /* WriteBlocksFromMem */




static u32 EraseOne(const char *whichOne)
{
/* Routine to erase one block of flash */

	volatile u32 *writeMe = (u32 *)whichOne;
	u32	result;

#ifdef BLOB_DEBUG
	SerialOutputString(__FUNCTION__ "(): erasing block at address 0x");
	SerialOutputHex((u32)whichOne);
	SerialOutputByte('\r');
#endif
	
	*writeMe = data_to_flash(ERASE_SETUP);
	*writeMe = data_to_flash(ERASE_CONFIRM);
	
	do {
		*writeMe = data_to_flash(STATUS_READ);
		result = data_from_flash(*writeMe);
	} while((~result & STATUS_BUSY) != 0);
	
	*writeMe = data_to_flash(READ_ARRAY);
	return result;
} /* EraseOne */




#ifdef ADVANCED_FLASH
static void GetBlockOrder(tBlkInfo *blocksInfo, u8 which, 
			  u8 blockOrder[NUM_FLASH_BLOCKS])
{

	tBlockType	type = blocksInfo->headers[which].type;
	char	*name = blocksInfo->headers[which].name;
	int	i;
	
	/* If the block isn't in use, fail silently. */
	if(!BLOCK_IN_USE(blocksInfo->headers[which]))
		return;
	
	for(i = 0; i < NUM_FLASH_BLOCKS; i++) {
		if(BLOCK_IN_USE(blocksInfo->headers[i]) && 
			(blocksInfo->headers[i].type == type) &&
			!MyStrNCmp(blocksInfo->headers[i].name, name, BLOCK_NAME_LEN)) {
			
			blockOrder[blocksInfo->headers[i].seqNum] = i;
		}
	}
} /* GetBlockOrder */
#endif
