/*-------------------------------------------------------------------------
 * Filename:      uucodec.c
 * Version:       $Id: uucodec.c,v 1.4 1999/09/28 21:48:14 erikm Exp $
 * Copyright:     Copyright (C) 1999, Jan-Derk Bakker
 * Author:        Jan-Derk Bakker <J.D.Bakker@its.tudelft.nl>
 * Description:   uu{en|de}code functiuonality for uploading of kernels 
 *                and the like
 * Created at:    Wed Aug 25 20:00:00 1999
 * Modified by:   Erik Mouw <J.A.K.Mouw@its.tudelft.nl>
 * Modified at:   Tue Sep 28 23:25:15 1999
 *-----------------------------------------------------------------------*/
/*
 * uucodec.c: uu{en|de}code functiuonality for uploading of kernels and 
 *            the like
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

#ident "$Id: uucodec.c,v 1.4 1999/09/28 21:48:14 erikm Exp $"

#ifdef HAVE_CONFIG_H
# include "config.h"
#endif

#include "serial.h"
#include "util.h"




#define INT_BUF_SIZE 1024
#define	MAX_RETRIES 10

#define TEST_MAX_RETRIES do { 				                   \
	if(retries++ > MAX_RETRIES) {	 		                   \
	        SerialOutputString("\r*** Timeout exceeded. Aborting.\r"); \
		return -1;					           \
	}						                   \
} while(0)

#define	DEC(c)	(((c) - ' ') & 077)		/* single character decode */
#define IS_DEC(c) ( (((c) - ' ') >= 0) &&  (((c) - ' ') <= 077 + 1) )
/* #define IS_DEC(c) (1) */

#define OUT_OF_RANGE do {	\
	SerialOutputByte('\r');	\
        SerialOutputString(buf);	\
        SerialOutputString("\r*** Received character out of range. Aborting.\r"); \
        return -1; \
} while(0)

#define PUT_CHAR(x) do {	             \
	if(bytesWritten < bufLen)	     \
	        bufBase[bytesWritten++] = x; \
} while(0)




int UUDecode(char *bufBase, int bufLen) {
/*	Receives and decodes an incoming uuencoded stream. Returns the number of 
	bytes put in the buffer on success, or -1 otherwise. */

	int n, linesReceived = 0;
	char ch, *p;
	int bytesWritten = 0, retries = 0;
	char buf[INT_BUF_SIZE];

	/* Search for header line. We don't care about the mode or filename */
	retries = 0;
	do {
		SerialInputString(buf, sizeof(buf), 6);
		TEST_MAX_RETRIES;		
	} while (MyStrNCmp(buf, "begin ", 6) != 0);

/*
	SerialOutputString("Bytes received : 0x00000000");
*/
	/* for each input line */
	for (;;) {
		if (SerialInputString(p = buf, sizeof(buf), 2) == 0) {
			SerialOutputString("\r*** Short file. Aborting\r");
			return -1;
		}
		/* Status print to show where we are at right now */
		if((linesReceived++ & 0x007F) == 0) {
			SerialOutputByte('.');
		}
		/*
		 * `n' is used to avoid writing out all the characters
		 * at the end of the file.
		 */
		if ((n = DEC(*p)) <= 0)
			break;
		for (++p; n > 0; p += 4, n -= 3)
			if (n >= 3) {
				if (!(IS_DEC(*p) && IS_DEC(*(p + 1)) && 
				     IS_DEC(*(p + 2)) && IS_DEC(*(p + 3))))
                                	OUT_OF_RANGE;

				ch = DEC(p[0]) << 2 | DEC(p[1]) >> 4;
				PUT_CHAR(ch);
				ch = DEC(p[1]) << 4 | DEC(p[2]) >> 2;
				PUT_CHAR(ch);
				ch = DEC(p[2]) << 6 | DEC(p[3]);
				PUT_CHAR(ch);
				
			}
			else {
				if (n >= 1) {
					if (!(IS_DEC(*p) && IS_DEC(*(p + 1))))
						OUT_OF_RANGE;
					ch = DEC(p[0]) << 2 | DEC(p[1]) >> 4;
					PUT_CHAR(ch);
				}
				if (n >= 2) {
					if (!(IS_DEC(*(p + 1)) && 
						IS_DEC(*(p + 2))))
						OUT_OF_RANGE;

					ch = DEC(p[1]) << 4 | DEC(p[2]) >> 2;
					PUT_CHAR(ch);
				}
				if (n >= 3) {
					if (!(IS_DEC(*(p + 2)) && 
						IS_DEC(*(p + 3))))
						OUT_OF_RANGE;
					ch = DEC(p[2]) << 6 | DEC(p[3]);
					PUT_CHAR(ch);
				}
			}
	}
	SerialOutputByte('\r');
	if (SerialInputString(p = buf, sizeof(buf), 2) == 0 || (MyStrNCmp(buf, "end", 3))) {
		SerialOutputString("*** No \"end\" line. Aborting.\r");
		return(-1);
	}
	return(bytesWritten);
} /* UUDecode */


/* ENC is the basic 1 character encoding function to make a char printing */
#define	ENC(c) ((c) ? ((c) & 077) + ' ': '`')


void UUEncode(char *bufBase, int bufLen) {
	register int ch, n;
	register char *p;
	char buf[80];

	SerialOutputString("begin 644 testme.jdb\r");
	while (bufLen > 0) {
		n = (bufLen > 45) ? 45 : bufLen;
		MyMemCpyChar(buf, bufBase, n);
		bufBase += n;
		bufLen -= n;
		ch = ENC(n);
		SerialOutputByte(ch);
		for (p = buf; n > 0; n -= 3, p += 3) {
			ch = *p >> 2;
			ch = ENC(ch);
			SerialOutputByte(ch);
			ch = ((*p << 4) & 060) | ((p[1] >> 4) & 017);
			ch = ENC(ch);
			SerialOutputByte(ch);
			ch = ((p[1] << 2) & 074) | ((p[2] >> 6) & 03);
			ch = ENC(ch);
			SerialOutputByte(ch);
			ch = p[2] & 077;
			ch = ENC(ch);
			SerialOutputByte(ch);
		}
		SerialOutputByte('\r');
	}
	ch = ENC('\0');
	SerialOutputByte(ch);
	SerialOutputByte('\r');
	SerialOutputString("end\r");
	
} /* UUEncode */
