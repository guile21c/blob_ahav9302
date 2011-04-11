/*-------------------------------------------------------------------------
 * Filename:      time.c
 * Version:       $Id: time.c,v 1.4 1999/10/03 22:03:52 erikm Exp $
 * Copyright:     Copyright (C) 1999, Erik Mouw
 * Author:        Erik Mouw <J.A.K.Mouw@its.tudelft.nl>
 * Description:   Some easy timer functions for blob
 * Created at:    Tue Aug 24 21:08:25 1999
 * Modified by:   Erik Mouw <J.A.K.Mouw@its.tudelft.nl>
 * Modified at:   Sun Oct  3 21:10:21 1999
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

#ident "$Id: time.c,v 1.4 1999/10/03 22:03:52 erikm Exp $"

#ifdef HAVE_CONFIG_H
# include "config.h"
#endif

/* Tell SA-1100.h to shut up; we're including it anyway. Nyah nyah ;-) */
#define __ASM_ARCH_HARDWARE_H
#include <asm/arch-sa1100/SA-1100.h>

#include "time.h"




void TimerInit(void)
{
	/* disable timer interrupts and clear the AL and HZ bits*/
	RTSR = (RTSR_AL | RTSR_HZ);

	/* put the counter to 0 */
	/* strange enough, this doesn't seem to work -- Erik */
	/* RCNR = 0x0; */
}




/* returns the time in seconds */
u32  TimerGetTime(void)
{
	return((u32) RCNR);
}
