/*-------------------------------------------------------------------------
 * Filename:      serial.c
 * Version:       $Id: serial.c,v 1.17 1999/10/04 20:11:47 erikm Exp $
 * Copyright:     Copyright (C) 1999, Erik Mouw
 * Author:        Erik Mouw <J.A.K.Mouw@its.tudelft.nl>
 * Description:   Serial utilities for blob
 * Created at:    Tue Aug 24 20:25:00 1999
 * Modified by:   Erik Mouw <J.A.K.Mouw@its.tudelft.nl>
 * Modified at:   Mon Oct  4 20:11:14 1999
 *-----------------------------------------------------------------------*/
/*
 * serial.c: Serial utilities for blob
 *
 * Copyright (C) 1999  Erik Mouw (J.A.K.Mouw@its.tudelft.nl)
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

#ident "$Id: serial.c,v 1.17 1999/10/04 20:11:47 erikm Exp $"

#ifdef HAVE_CONFIG_H
# include "config.h"
#endif

/* Tell SA-1100.h to shut up; we're including it anyway. Nyah nyah ;-) */
#define __ASM_ARCH_HARDWARE_H
#include <asm/arch-sa1100/SA-1100.h>

#include "serial.h"
#include "time.h"




/* number of nibbles in a word */
#define NIBBLES_PER_WORD (8)




void SerialInit(eBauds baudrate)
{
	/* flush the output buffer */
	while(Ser3UTSR1 & UTSR1_TBY) {
		/* do nothing */
	}

	/* switch receiver and transmitter off */
	Ser3UTCR3 = 0x00;

	/* clear all sticky bits in control register 3 */
	Ser3UTSR0 = 0xff;

	/* set the port to sensible defaults: no break, no interrupts, no
	   parity, 8 databits, 1 stopbit, transmitter and receiver enabled */
	Ser3UTCR0 = ( UTCR0_1StpBit | UTCR0_8BitData );

	/* set the baudrate */
	Ser3UTCR1 = 0;
	Ser3UTCR2 = (u32)baudrate;

	/* Turn the receiver and transmitter back on */
	Ser3UTCR3 = ( UTCR3_RXE | UTCR3_TXE );
}




void SerialOutputByte(const char c)
{
	/* If \r, also do \n */
	if(c == '\r')
		SerialOutputByte('\n');
		
	/* wait for room in the tx FIFO */
	while((Ser3UTSR0 & UTSR0_TFS) == 0) ;

	Ser3UTDR = c;
}




void SerialOutputString(const char *s) {

	while(*s != 0)
		SerialOutputByte(*s++);
		
} /* SerialOutputString */




void SerialOutputHex(const u32 h)
{
	char c;
	int i;
	
	for(i = NIBBLES_PER_WORD - 1; i >= 0; i--) {
		c = (char)((h >> (i * 4)) & 0x0f);

		if(c > 9)
			c += ('A' - 10);
		else
			c += '0';

		SerialOutputByte(c);
	}
}




void SerialOutputDec(const u32 d)
{
	int leading_zero = 1;
	u32 divisor, result, remainder;

	remainder = d;

	for(divisor = 1000000000; 
	    divisor > 0; 
	    divisor /= 10) {
		result = remainder / divisor;
		remainder %= divisor;

		if(result != 0 || divisor == 1)
			leading_zero = 0;

		if(leading_zero == 0)
			SerialOutputByte((char)(result) + '0');
	}
}




void SerialOutputBlock(const char *buf, int bufsize)
{
	while(bufsize--)
		SerialOutputByte(*buf++);
}




/* returns 1 on success, 0 otherwise */
int SerialInputByte(char *c)
{
	if(Ser3UTSR1 & UTSR1_RNE) {
		int err = Ser3UTSR1 & (UTSR1_PRE | UTSR1_FRE | UTSR1_ROR);
		*c = (char)Ser3UTDR;
		if(err & UTSR1_PRE)
			SerialOutputByte('@');
		else if(err & UTSR1_FRE)
			SerialOutputByte('#');
		else if(err & UTSR1_ROR)
			SerialOutputByte('$');
#if 0
		else 
			SerialOutputByte(*c);
#endif
	
		/* We currently only care about framing and parity errors */
		if((err & (UTSR1_PRE | UTSR1_FRE)) != 0) {
			return SerialInputByte(c);
		}
		else
			return(1);
	}
	else {
			/* no bit ready */
			return(0);
	}
} /* SerialInputByte */




/*
 * read a string with maximum length len from the serial port
 * using a timeout of timeout seconds
 *
 * len is the length of array s _including_ the trailing zero,
 * the function returns the number of bytes read _excluding_
 * the trailing zero
 */
int  SerialInputString(char *s, const int len, const int timeout)
{
	u32 startTime, currentTime;
	char c;
	int i;
	int numRead;
	int skipNewline = 1;
	int maxRead = len - 1;
	
	startTime = TimerGetTime();

	for(numRead = 0, i = 0; numRead < maxRead;) {
		/* try to get a byte from the serial port */
		while(!SerialInputByte(&c)) {
			currentTime = TimerGetTime();

			/* check timeout value */
			if((currentTime - startTime) > 
			   (timeout * TICKS_PER_SECOND)) {
				/* timeout */
				s[i++] = '\0';
				return(numRead);
			}
		}

		/* eat newline characters at start of string */
		if((skipNewline == 1) && (c != '\r') && (c != '\n'))
			skipNewline = 0;

		if(skipNewline == 0) {
			if((c == '\r') || (c == '\n')) {
				s[i++] = '\0';
				return(numRead);
			} else {
				s[i++] = c;
				numRead++;
			}
		}
	}

	return(numRead);
}




int  SerialInputBlock(char *buf, int bufsize, const int timeout)
{
	u32 startTime, currentTime;
	char c;
	int i;
	int numRead;
	int maxRead = bufsize;
	
	startTime = TimerGetTime();

	for(numRead = 0, i = 0; numRead < maxRead;) {
		/* try to get a byte from the serial port */
		while(!SerialInputByte(&c)) {
			currentTime = TimerGetTime();

			/* check timeout value */
			if((currentTime - startTime) > 
			   (timeout * TICKS_PER_SECOND)) {
				/* timeout! */
				return(numRead);
			}
		}
		
		buf[i++] = c;
		numRead ++;
	}

	return(numRead);
}
