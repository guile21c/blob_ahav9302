/*-------------------------------------------------------------------------
 * Filename:      command.h
 * Version:       $Id: command.h,v 1.3 1999/10/03 22:03:52 erikm Exp $
 * Copyright:     Copyright (C) 1999, Erik Mouw
 * Author:        Erik Mouw <J.A.K.Mouw@its.tudelft.nl>
 * Description:   Header file for command.c
 * Created at:    Sun Aug 29 17:23:33 1999
 * Modified by:   Erik Mouw <J.A.K.Mouw@its.tudelft.nl>
 * Modified at:   Tue Sep 28 23:38:04 1999
 *-----------------------------------------------------------------------*/
/*
 * command.c: Command line functions for blob
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

#ident "$Id: command.h,v 1.3 1999/10/03 22:03:52 erikm Exp $"

#ifndef BLOB_COMMAND_H
#define BLOB_COMMAND_H

void DisplayPrompt(char *prompt);
int GetCommand(char *command, int len, int timeout);

#endif
