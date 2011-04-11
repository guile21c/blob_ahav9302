/*-------------------------------------------------------------------------
 * Filename:      time.h
 * Version:       $Id: time.h,v 1.4 1999/10/03 22:03:52 erikm Exp $
 * Copyright:     Copyright (C) 1999, Erik Mouw
 * Author:        Erik Mouw <J.A.K.Mouw@its.tudelft.nl>
 * Description:   Timer functions
 * Created at:    Tue Aug 24 21:08:24 1999
 * Modified by:   Erik Mouw <J.A.K.Mouw@its.tudelft.nl>
 * Modified at:   Tue Sep 28 23:44:11 1999
 *-----------------------------------------------------------------------*/
/*
 * timer.c: Timer functions for blob
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

#ident "$Id: time.h,v 1.4 1999/10/03 22:03:52 erikm Exp $"

#ifndef BLOB_TIME_H
#define BLOB_TIME_H




#include "types.h"




#define TICKS_PER_SECOND 32768




/* initialise timer */
void TimerInit(void);

/* returns the time in 1/32768 seconds */
u32  TimerGetTime(void);




#endif
