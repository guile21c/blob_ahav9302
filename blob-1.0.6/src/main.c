/*-------------------------------------------------------------------------
 * Filename:      main.c
 * Version:       $Id: main.c,v 1.24 2000/03/25 13:42:52 erikm Exp $
 * Copyright:     Copyright (C) 1999, Jan-Derk Bakker
 * Author:        Jan-Derk Bakker <J.D.Bakker@its.tudelft.nl>
 * Description:   Main file for the trivially simple bootloader for the 
 *                LART boards
 * Created at:    Mon Aug 23 20:00:00 1999
 * Modified by:   Erik Mouw <J.A.K.Mouw@its.tudelft.nl>
 * Modified at:   Sat Mar 25 14:31:16 2000
 *-----------------------------------------------------------------------*/
/*
 * main.c: main file for the blob bootloader
 *
 * Copyright (C) 1999  Jan-Derk Bakker (J.D.Bakker@its.tudelft.nl) and
 *                     Erik Mouw (J.A.K.Mouw@its.tudelft.nl)
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

#ident "$Id: main.c,v 1.24 2000/03/25 13:42:52 erikm Exp $"

#ifdef HAVE_CONFIG_H
# include "config.h"
#endif

#include "clock.h"
#include "command.h"
#include "flash.h"
#include "serial.h"
#include "time.h"
#include "main.h"
#include "util.h"
#include "uucodec.h"




typedef enum {
	fromFlash = 0,
	fromDownload = 1
} blockSource;




typedef struct {
	int kernelSize;
	blockSource kernelType;

	int ramdiskSize;
	blockSource ramdiskType;

	u32 blockSize;

	eBauds downloadSpeed;
} blobStatus;




void BootKernel(char *commandline);
void Download(char *commandline, blobStatus *status);
void Flash(char *commandline, blobStatus *status);
void PrintHelp(void);
void SetDownloadSpeed(char *commandline, blobStatus *status);
void PrintStatus(blobStatus *status);
void ResetTerminal(void);
void Reload(char *commandline, blobStatus *status);
void PrintSerialSpeed(eBauds speed);




void c_main(char *blockBase, u32 blockSize)
{
	int numRead = 0;
	char commandline[128];
	blobStatus status;
	int i;
	int retval = 0;
	
	/* We really want to be able to communicate, so initialise the
	 * serial port at 9k6 (which works good for terminals)
	 */
	SerialInit(baud9k6);
	TimerInit();
	
	/* initialise status */
	status.kernelSize = 0;
	status.kernelType = fromFlash;
	status.ramdiskSize = 0;
	status.ramdiskType = fromFlash;
	status.blockSize = blockSize;
	status.downloadSpeed = baud115k2;
	
	/* Load kernel and ramdisk from flash to RAM */
	Reload("kernel", &status);
	Reload("ramdisk", &status);

	/* Print the required GPL string */
	SerialOutputString("\r" PACKAGE " version " VERSION  "\r"
			   "Copyright (C) 1999 2000 "
			   "Jan-Derk Bakker and Erik Mouw\r"
			   "Copyright (C) 2000 "
			   "Johan Pouwelse.\r");
	SerialOutputString(PACKAGE " comes with ABSOLUTELY NO WARRANTY; "
			   "read the GNU GPL for details.\r");
	SerialOutputString("This is free software, and you are welcome "
			   "to redistribute it\r");
	SerialOutputString("under certain conditions; "
			   "read the GNU GPL for details.\r\r");

	/* and some information */
#ifdef BLOB_DEBUG
	SerialOutputString("Running from ");
	if(RunningFromInternal())
		SerialOutputString("internal flash\r");
	else
		SerialOutputString("external flash\r");

	SerialOutputString("blockBase = 0x");
	SerialOutputHex((int) blockBase);
	SerialOutputString(", blockSize = 0x");
	SerialOutputHex(blockSize);
	SerialOutputByte('\r');
#endif
	/* wait 10 seconds before starting autoboot */
	SerialOutputString("Autoboot in progress, press any key to stop ");
	for(i = 0; i < 10; i++) {
		SerialOutputByte('.');

 		retval = SerialInputBlock(commandline, 1, 1); 

		if(retval > 0)
			break;
	}

	/* no key was pressed, so proceed booting the kernel */
	if(retval == 0) {
		commandline[0] = '\0';
		BootKernel(commandline);
	}

	SerialOutputString("\rAutoboot aborted\r");
	SerialOutputString("Type \"help\" to get a list of commands\r");

	/* the command loop. endless, of course */
	for(;;) {
		DisplayPrompt(NULL);

		/* wait an hour to get a command */
		numRead = GetCommand(commandline, 128, 3600);

		if(numRead > 0) {
			if(MyStrNCmp(commandline, "boot", 4) == 0) {
				BootKernel(commandline + 4);
			} else if(MyStrNCmp(commandline, "clock", 5) == 0) {
				SetClock(commandline + 5);
			} else if(MyStrNCmp(commandline, "download ", 9) == 0) {
				Download(commandline + 9, &status);
			} else if(MyStrNCmp(commandline, "flash ", 6) == 0) {
				Flash(commandline + 6, &status);
			} else if(MyStrNCmp(commandline, "help", 4) == 0) {
				PrintHelp();
			} else if(MyStrNCmp(commandline, "reload ", 7) == 0) {
				Reload(commandline + 7, &status);
			} else if(MyStrNCmp(commandline, "reset", 5) == 0) {
				ResetTerminal();
			} else if(MyStrNCmp(commandline, "speed ", 6) == 0) {
				SetDownloadSpeed(commandline + 6, &status);
			}
			else if(MyStrNCmp(commandline, "status", 6) == 0) {
				PrintStatus(&status);
			} else {
				SerialOutputString("*** Unknown command: ");
				SerialOutputString(commandline);
				SerialOutputByte('\r');
			}
		}
	}
} /* c_main */




void BootKernel(char *commandline)
{
	void (*theKernel)(int zero, int arch) = (void (*)(int, int))KERNEL_RAM_BASE;

#warning "FIXME: should set kernel commandline parameters -- Erik"

#ifdef BLOB_DEBUG
	SerialOutputString("\r*** Ignoring kernel parameters\r");
#endif

	/* we assume that the kernel is in place */
	SerialOutputString("\rStarting kernel ...\r\r");
	theKernel(0, 9);

	SerialOutputString("Hey, the kernel returned! This should not happen.\r");
}




void Download(char *commandline, blobStatus *status)
{
	u32 startAddress = 0;
	int bufLen;
	int *numRead = 0;

	if(MyStrNCmp(commandline, "kernel", 6) == 0) {
		/* download kernel */
		startAddress = KERNEL_RAM_BASE;
		bufLen = status->blockSize - KERNEL_BLOCK_OFFSET;
		numRead = &status->kernelSize;
		status->kernelType = fromDownload;
	} else if(MyStrNCmp(commandline, "ramdisk", 7) == 0) {
		/* download ramdisk */
		startAddress = RAMDISK_RAM_BASE;
		bufLen = status->blockSize - RAMDISK_BLOCK_OFFSET;
		numRead = &status->ramdiskSize;
		status->ramdiskType = fromDownload;
	} else {
		SerialOutputString("*** Don't know how to download \"");
		SerialOutputString(commandline);
		SerialOutputString("\"\r");
		return;
	}


	SerialOutputString("Switching to ");
	PrintSerialSpeed(status->downloadSpeed);
	SerialOutputString(" baud\r");

	SerialOutputString("You have 60 seconds to switch your terminal emulator to the same speed and\r");
	SerialOutputString("start downloading. After that " PACKAGE " will switch back to 9600 baud.\r");
	
	SerialInit(status->downloadSpeed);

	*numRead = UUDecode((char *)startAddress, bufLen);
	
	SerialOutputString("\r(Please switch your terminal emulator back to 9600 baud)\r");
	
	if(*numRead < 0) {
		/* something went wrong */
		SerialOutputString("*** Uudecode receive failed\r");
		
		/* reload the correct memory */
		Reload(commandline, status);

		SerialInit(baud9k6);
		return;
	}
	SerialOutputString("Received ");
	SerialOutputDec(*numRead);
	SerialOutputString(" (0x");
	SerialOutputHex(*numRead);
	SerialOutputString(") bytes.\r");


	SerialInit(baud9k6);
}




void Flash(char *commandline, blobStatus *status)
{
	u32 startAddress = 0;
	tBlockType block;
	int numBytes = 0;;

	if(RunningFromInternal()) {
		SerialOutputString("*** Flash not possible when using internal flash\r");
		return;
	}

	if(MyStrNCmp(commandline, "kernel", 6) == 0) {
		startAddress = KERNEL_RAM_BASE;
		block = blKernel;
		numBytes = status->kernelSize;

		if(status->kernelType == fromFlash) {
			SerialOutputString("*** No kernel downloaded\r");
			return;
		}

		SerialOutputString("Saving kernel to flash ");
	} else if(MyStrNCmp(commandline, "ramdisk", 7) == 0) {
		startAddress = RAMDISK_RAM_BASE;
		block = blRamdisk;
		numBytes = status->ramdiskSize;

		if(status->ramdiskType == fromFlash) {
			SerialOutputString("*** No ramdisk downloaded\r");
			return;
		}

		SerialOutputString("Saving ramdisk to flash ");
	} else {
		SerialOutputString("*** Don't know how to flash \"");
		SerialOutputString(commandline);
		SerialOutputString("\"\r");
		return;
	}

	EraseBlocks(block);
	SerialOutputByte(' ');
	WriteBlocksFromMem(block, (u32 *)startAddress, numBytes);
	SerialOutputString(" done\r");
}




void PrintHelp(void)
{
	SerialOutputString("Help for " PACKAGE " " VERSION ", the LART bootloader\r");
	SerialOutputString("The following commands are supported:\r");
	SerialOutputString("* boot [kernel options]      Boot Linux with optional kernel options\r");
	SerialOutputString("* clock PPCR MDCNFG MDCAS0 MDCAS1 MDCAS2\r");
	SerialOutputString("                             Set the SA1100 core clock and DRAM timings\r");
	SerialOutputString("                             (WARNING: dangerous command!)\r");
	SerialOutputString("* download {kernel|ramdisk}  Download kernel or ramdisk image to RAM\r");
	SerialOutputString("* flash {kernel|ramdisk}     Copy kernel or ramdisk from RAM to flash\r");
	SerialOutputString("* help                       Get this help\r");
	SerialOutputString("* reload {kernel|ramdisk}    Reload kernel or ramdisk from flash to RAM\r");
	SerialOutputString("* reset                      Reset terminal\r");
	SerialOutputString("* speed                      Set download speed\r");
	SerialOutputString("* status                     Display current status\r");
}




void SetDownloadSpeed(char *commandline, blobStatus *status)
{
	if(MyStrNCmp(commandline, "1200", 4) == 0) {
		status->downloadSpeed = baud1k2;
	} else if(MyStrNCmp(commandline, "1k2", 3) == 0) {
		status->downloadSpeed = baud1k2;
	} else if(MyStrNCmp(commandline, "9600", 4) == 0) {
		status->downloadSpeed = baud9k6;
	} else if(MyStrNCmp(commandline, "9k6", 3) == 0) {
		status->downloadSpeed = baud9k6;
	} else if(MyStrNCmp(commandline, "19200", 5) == 0) {
		status->downloadSpeed = baud19k2;
	} else if(MyStrNCmp(commandline, "19k2", 4) == 0) {
		status->downloadSpeed = baud19k2;
	} else if(MyStrNCmp(commandline, "38400", 5) == 0) {
		status->downloadSpeed = baud38k4;
	} else if(MyStrNCmp(commandline, "38k4", 4) == 0) {
		status->downloadSpeed = baud38k4;
	} else if(MyStrNCmp(commandline, "57600", 5) == 0) {
		status->downloadSpeed = baud57k6;
	} else if(MyStrNCmp(commandline, "57k6", 4) == 0) {
		status->downloadSpeed = baud57k6;
	} else if(MyStrNCmp(commandline, "115200", 6) == 0) {
		status->downloadSpeed = baud115k2;
	} else if(MyStrNCmp(commandline, "115k2", 5) == 0) {
		status->downloadSpeed = baud115k2;
	} else {
		SerialOutputString("*** Invalid download speed value \"");
		SerialOutputString(commandline);
		SerialOutputString("\"\r*** Valid values are:\r");
		SerialOutputString("*** 1200, 9600, 19200, 38400, 57600, 115200,\r");
		SerialOutputString("*** 1k2, 9k6, 19k2, 38k4, 57k6, and 115k2\r");
	}

	SerialOutputString("Download speed set to ");
	PrintSerialSpeed(status->downloadSpeed);
	SerialOutputString(" baud\r");
}




void PrintStatus(blobStatus *status)
{
	SerialOutputString("Bootloader    : " PACKAGE "\r");
	SerialOutputString("Version       : " VERSION "\r");

	SerialOutputString("Running from  : ");
	if(RunningFromInternal())
		SerialOutputString("internal");
	else
		SerialOutputString("external");

	SerialOutputString(" flash\rBlocksize     : 0x");
	SerialOutputHex(status->blockSize);

	SerialOutputString("\rDownload speed: ");
	PrintSerialSpeed(status->downloadSpeed);
	SerialOutputString(" baud\r");

	SerialOutputString("Kernel        : ");
	if(status->kernelType == fromFlash) {
		SerialOutputString("from flash\r");
	} else {
		SerialOutputString("downloaded, ");
		SerialOutputDec(status->kernelSize);
		SerialOutputString(" bytes\r");
	}

	SerialOutputString("Ramdisk       : ");
	if(status->ramdiskType == fromFlash) {
		SerialOutputString("from flash\r");
	} else {
		SerialOutputString("downloaded, ");
		SerialOutputDec(status->ramdiskSize);
		SerialOutputString(" bytes\r");
	}
}




void ResetTerminal(void)
{
	int i;

	SerialInit(baud9k6);
	SerialOutputString("          c");
	for(i = 0; i < 100; i++)
		SerialOutputByte('\r');

	SerialOutputString("c");
}




void Reload(char *commandline, blobStatus *status)
{
	u32 *src = 0;
	u32 *dst = 0;
	int numWords;

	if(MyStrNCmp(commandline, "kernel", 6) == 0) {
		src = (u32 *)KERNEL_RAM_BASE;
		dst = (u32 *)KERNEL_START;
		numWords = KERNEL_LEN / 4;
		status->kernelSize = 0;
		status->kernelType = fromFlash;
		SerialOutputString("Loading kernel from flash ");
	} else if(MyStrNCmp(commandline, "ramdisk", 7) == 0) {
		src = (u32 *)RAMDISK_RAM_BASE;
		dst = (u32 *)INITRD_START;
		numWords = INITRD_LEN / 4;
		status->ramdiskSize = 0;
		status->ramdiskType = fromFlash;
		SerialOutputString("Loading ramdisk from flash ");
	} else {
		SerialOutputString("*** Don't know how to reload \"");
		SerialOutputString(commandline);
		SerialOutputString("\"\r");
		return;
	}

	MyMemCpy(src, dst, numWords);
	SerialOutputString(" done\r");
}




void PrintSerialSpeed(eBauds speed)
{
	switch(speed) {
	case baud1k2:
		SerialOutputDec(1200);
		break;

	case baud9k6:
		SerialOutputDec(9600);
		break;

	case baud19k2:
		SerialOutputDec(19200);
		break;

	case baud38k4:
		SerialOutputDec(38400);
		break;

	case baud57k6:
		SerialOutputDec(57600);
		break;

	case baud115k2:
		SerialOutputDec(115200);
		break;

	default:
		SerialOutputString("(unknown)");
		break;
	}
}

