/*-------------------------------------------------------------------------
 * Filename:      serial.h
 * Version:       $Id: serial.h,v 1.10 1999/10/03 22:03:52 erikm Exp $
 * Copyright:     Copyright (C) 1999, Erik Mouw
 * Author:        Erik Mouw <J.A.K.Mouw@its.tudelft.nl>
 * Description:   Header file for serial.c
 * Created at:    Tue Aug 24 20:25:00 1999
 * Modified by:   Erik Mouw <J.A.K.Mouw@its.tudelft.nl>
 * Modified at:   Tue Sep 28 23:43:46 1999
 *-----------------------------------------------------------------------*/
/*
 * serial.h: Serial utilities for blob
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

#ident "$Id: serial.h,v 1.10 1999/10/03 22:03:52 erikm Exp $"

#ifndef BLOB_SERIAL_H
#define BLOB_SERIAL_H


#include "types.h"


typedef enum { /* Some useful SA-1100 baud rates */
	baud1k2 = 191,
	baud9k6 = 23,
	baud19k2 = 11,
	baud38k4 = 5,
	baud57k6 = 3,
	baud115k2 = 1
} eBauds;


/* Function protos */
void SerialInit(eBauds baudrate);

void SerialOutputByte(const char c);
void SerialOutputString(const char *s);
void SerialOutputHex(const u32 h);
void SerialOutputDec(const u32 d);
void SerialOutputBlock(const char *buf, int bufsize);

int  SerialInputByte(char *c);
int  SerialInputString(char *s, const int len, const int timeout);
int  SerialInputBlock(char *buf, int bufsize, const int timeout);

#endif
