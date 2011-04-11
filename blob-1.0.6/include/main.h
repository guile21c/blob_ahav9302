/*-------------------------------------------------------------------------
 * Filename:      main.h
 * Version:       $Id: main.h,v 1.5 1999/10/03 22:03:52 erikm Exp $
 * Copyright:     Copyright (C) 1999, Jan-Derk Bakker
 * Author:        Jan-Derk Bakker <J.D.Bakker@its.tudelft.nl>
 * Description:   Header file for main.c
 * Created at:    Mon Aug 23 20:00:00 1999
 * Modified by:   Erik Mouw <J.A.K.Mouw@its.tudelft.nl>
 * Modified at:   Sun Oct  3 21:44:01 1999
 *-----------------------------------------------------------------------*/
/*
 * main.h: Header file for main.c
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

#ident "$Id: main.h,v 1.5 1999/10/03 22:03:52 erikm Exp $"

#ifndef BLOB_MAIN_H
#define BLOB_MAIN_H




#define KERNEL_RAM_BASE      (0xC0008000)
#define RAMDISK_RAM_BASE     (0xC0400000)

#define KERNEL_BLOCK_OFFSET  (0x00008000)
#define RAMDISK_BLOCK_OFFSET (0x00400000)



#endif
