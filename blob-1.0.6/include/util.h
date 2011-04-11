/*-------------------------------------------------------------------------
 * Filename:      util.h
 * Version:       $Id: util.h,v 1.5 1999/10/03 22:03:52 erikm Exp $
 * Copyright:     Copyright (C) 1999, Jan-Derk Bakker
 * Author:        Jan-Derk Bakker <J.D.Bakker@its.tudelft.nl>
 * Description:   Simple utility functions for blob
 * Created at:    Wed Aug 25 21:00:00 1999
 * Modified by:   Erik Mouw <J.A.K.Mouw@its.tudelft.nl>
 * Modified at:   Tue Sep 28 23:45:46 1999
 *-----------------------------------------------------------------------*/
/*
 * util.h: Simple utility functions for blob
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

#ident "$Id: util.h,v 1.5 1999/10/03 22:03:52 erikm Exp $"


#ifndef BLOB_UTIL_H
#define BLOB_UTIL_H

#include "types.h"

void MyMemCpy(u32 *dest, const u32 *src, int numWords);
void MyMemCpyChar(char *dest, const char *src, int numBytes);
void MyMemSet(u32 *dest, const u32 wordToWrite, int numWords);
int MyStrNCmp(const char *s1, const char *s2, int maxlen);

int MyToUpper(int c);
int MyToLower(int c);

#endif
