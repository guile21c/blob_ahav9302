/*-------------------------------------------------------------------------
 * Filename:      util.c
 * Version:       $Id: util.c,v 1.5 1999/10/03 22:03:52 erikm Exp $
 * Copyright:     Copyright (C) 1999, Jan-Derk Bakker
 * Author:        Jan-Derk Bakker <J.D.Bakker@its.tudelft.nl>
 * Description:   Simple utility functions for blob
 * Created at:    Wed Aug 25 21:00:00 1999
 * Modified by:   Erik Mouw <J.A.K.Mouw@its.tudelft.nl>
 * Modified at:   Sun Oct  3 21:10:43 1999
 *-----------------------------------------------------------------------*/
/*
 * util.c: Simple utility functions for blob
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

#ident "$Id: util.c,v 1.5 1999/10/03 22:03:52 erikm Exp $"

#ifdef HAVE_CONFIG_H
# include "config.h"
#endif

#include "types.h"
#include "util.h"
#include "serial.h"




void MyMemCpy(u32 *dest, const u32 *src, int numWords) 
{
#ifdef BLOB_DEBUG
	SerialOutputString("# Now copying 0x");
	SerialOutputHex(numWords);
	SerialOutputString(" words from 0x");
	SerialOutputHex((int)src);
	SerialOutputString(" to 0x");
	SerialOutputHex((int)dest);
	SerialOutputString("...");
#endif

	while(numWords--) {
		if((numWords & 0xffff) == 0x0)
			SerialOutputByte('.');

		*dest++ = *src++;
	}

#ifdef BLOB_DEBUG
	SerialOutputString(" done\r");
#endif
} /* MyMemCpy */




void MyMemCpyChar(char *dest, const char *src, int numBytes)
{
	char *destLim = dest + numBytes;
	
	while(dest < destLim)
		*dest++ = *src++;
} /* MyMemCpyChar */




void MyMemSet(u32 *dest, const u32 wordToWrite, int numWords)
{
	u32 *limit = dest + numWords;
	
	while(dest < limit)
		*dest++ = wordToWrite;
} /* MyMemSet */




int MyStrNCmp(const char *s1, const char *s2, int maxlen) 
{
	int i;

	for(i = 0; i < maxlen; i++) {
		if(s1[i] != s2[i])
			return ((int) s1[i]) - ((int) s2[i]);
		if(s1[i] == 0)
			return 0;
	}

	return 0;
} /* MyStrNCmp */




int MyToUpper(int c)
{
	if((c >= 'a') && (c <= 'z'))
		c += 'A' - 'a';

	return(c);
}




int MyToLower(int c)
{
	if((c >= 'A') && (c <= 'Z'))
		c += 'a' - 'A';

	return(c);
}
