/*-------------------------------------------------------------------------
 * Filename:      command.c
 * Version:       $Id: command.c,v 1.4 1999/10/03 22:03:52 erikm Exp $
 * Copyright:     Copyright (C) 1999, Erik Mouw
 * Author:        Erik Mouw <J.A.K.Mouw@its.tudelft.nl>
 * Description:   Command line functions for blob
 * Created at:    Sun Aug 29 17:23:40 1999
 * Modified by:   Erik Mouw <J.A.K.Mouw@its.tudelft.nl>
 * Modified at:   Sun Oct  3 21:08:27 1999
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

#ident "$Id: command.c,v 1.4 1999/10/03 22:03:52 erikm Exp $"

#ifdef HAVE_CONFIG_H
# include "config.h"
#endif

#include "command.h"
#include "serial.h"
#include "time.h"
#include "types.h"




/* display a prompt, or the standard prompt if prompt == NULL */
void DisplayPrompt(char *prompt)
{
	if(prompt == NULL) {
		SerialOutputString(PACKAGE "> ");
	} else {
		SerialOutputString(prompt);
	}
}




/* more or less like SerialInputString(), but with echo and backspace  */
int GetCommand(char *command, int len, int timeout)
{
	u32 startTime, currentTime;
	char c;
	int i;
	int numRead;
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
				command[i++] = '\0';
				return(numRead);
			}
		}

		if((c == '\r') || (c == '\n')) {
			command[i++] = '\0';

			/* print newline */
			SerialOutputByte('\r');
			return(numRead);
		} else if(c == '\b') { /* FIXME: is this backspace? */
			if(i > 0) {
				i--;
				numRead--;
				/* cursor one position back. */
				SerialOutputString("\b \b");
			}
		} else {
			command[i++] = c;
			numRead++;

			/* print character */
			SerialOutputByte(c);
		}
	}

	return(numRead);
}
