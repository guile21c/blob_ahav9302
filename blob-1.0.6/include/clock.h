/*-------------------------------------------------------------------------
 * Filename:      clock.h
 * Version:       $Id: clock.h,v 1.2 2000/03/25 13:42:52 erikm Exp $
 * Copyright:     Copyright (C) 2000, Johan Pouwelse
 * Author:        Johan Pouwelse <pouwelse@twi.tudelft.nl>
 * Description:   Clock switching functions
 * Created at:    Sat Mar 25 14:11:22 2000
 * Modified by:   Erik Mouw <J.A.K.Mouw@its.tudelft.nl>
 * Modified at:   Sat Mar 25 14:13:58 2000
 *-----------------------------------------------------------------------*/
/*
 * clock.h: Utility to set processor speed and memory parameters 
 *
 * Copyright (C) 2000  Johan Pouwelse (pouwelse@twi.tudelft.nl)
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

#ident "$Id: clock.h,v 1.2 2000/03/25 13:42:52 erikm Exp $"

#ifndef BLOB_CLOCK_H
#define BLOB_CLOCK_H

void SetClock(char *commandline);

#endif
