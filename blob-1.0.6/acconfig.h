/*-------------------------------------------------------------------------
 * Filename:      acconfig.h
 * Version:       $Id: acconfig.h,v 1.3 1999/10/03 22:03:52 erikm Exp $
 * Copyright:     Copyright (C) 1999, Erik Mouw
 * Author:        Erik Mouw <J.A.K.Mouw@its.tudelft.nl>
 * Description:   Definitions for GNU autoconf/automake/libtool
 * Created at:    Wed Jul 28 22:35:51 1999
 * Modified by:   Erik Mouw <J.A.K.Mouw@its.tudelft.nl>
 * Modified at:   Sun Oct  3 18:47:48 1999
 *-----------------------------------------------------------------------*/
/*
 * acconfig.h: Definitions for GNU autoconf/automake/libtool
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
/* 
 *
 * WARNING:
 *   Don't try to edit config.h.in or config.h, because your changes
 *   will be overwritten once you run automake, autoconf or
 *   configure. Edit acconfig.h instead.
 *
 */

#ident "$Id: acconfig.h,v 1.3 1999/10/03 22:03:52 erikm Exp $"

#ifndef BLOB_CONFIG_H
#define BLOB_CONFIG_H

@TOP@

/* Define the package name here */
#undef PACKAGE

/* Define the package version here */
#undef VERSION

/* Define to enable run-time debug information */
#undef BLOB_DEBUG

@BOTTOM@

#endif
