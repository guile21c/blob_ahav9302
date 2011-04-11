/*-------------------------------------------------------------------------
 * Filename:      clock.c
 * Version:       $Id: clock.c,v 1.2 2000/03/25 13:42:52 erikm Exp $
 * Copyright:     Copyright (C) 2000, Johan Pouwelse
 * Author:        Johan Pouwelse <pouwelse@twi.tudelft.nl>
 * Description:   Clock switching functions
 * Created at:    Sat Mar 25 14:11:22 2000
 * Modified by:   Erik Mouw <J.A.K.Mouw@its.tudelft.nl>
 * Modified at:   Sat Mar 25 14:12:42 2000
 *-----------------------------------------------------------------------*/
/*
 * clock.c: Utility to set clock speed and DRAM parameters 
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

#ident "$Id: clock.c,v 1.2 2000/03/25 13:42:52 erikm Exp $"

#ifdef HAVE_CONFIG_H
# include "config.h"
#endif

#include "types.h"
#include "util.h"
#include "serial.h"
#include "time.h"

#define __ASM_ARCH_HARDWARE_H
#include <asm/arch-sa1100/SA-1100.h>

/* It should be possible to improve this function */
/* Determine if the given character is a 0 to 9 or a to z,
   these characters can be converted to a hexidecimal number
*/

/* Struct with the SA-1100 PLL + DRAM parameter registers */
enum {
	SA_PPCR,
	SA_MDCNFG,
	SA_MDCAS0,
	SA_MDCAS1,
	SA_MDCAS2
};


int MyIsXDigit(char isdigit) {
  if ((isdigit >= '0' && isdigit <= '9' ) ||
      (isdigit >= 'a' && isdigit <= 'f'))
     return 1;
  else
     return 0;
} /* MyIsXDigit */

int MyXDigitValue(char isdigit) {
  if (isdigit >= '0' && isdigit <= '9' )
    return isdigit - '0';
  if (isdigit >= 'a' && isdigit <= 'f')
    return 10 + isdigit - 'a';
  return -1;
} /* MyXDigitValue */

/* Converts 8 characters 0-9a-f into a 4 byte hexadecimal value */
char *GetHexValue(char *commandline,u32 *value) {
  int i;

  if(commandline[0] == '\0') {
    SerialOutputString(__FUNCTION__ ": zero length string\n");
    return(NULL);
  }

  *value=0x00;
  if (MyStrNCmp(commandline, " 0x", 3) == 0) {
    commandline += 3;
  } else if(commandline[0] == ' ') {
    commandline += 1;
  } else {
    SerialOutputString(__FUNCTION__ ":value not hexdecimal\n");
    return NULL;
  }
  
  SerialOutputString("char: ");
  SerialOutputString(commandline);
  SerialOutputByte(' ');
  
  for (i=0; i<8;i++) {
    if (*commandline == '\0') {
      SerialOutputString(__FUNCTION__ ":hex value not 32 bits\n");
      return NULL;
    } else {
      if (MyIsXDigit(*commandline) == 0) {
        SerialOutputString("hex value contains invalid characters\n");
        return NULL;
      } else {
        *value |= (u32) MyXDigitValue(*commandline) << ((7-i)*4);
      }
    }

    commandline++;
  }

  SerialOutputString("hex 0x");
  SerialOutputHex(*value);
  SerialOutputByte('\r');

  return commandline;
} /* GetHexValue */

void SetClock(char *commandline) {
int i;
u32 regs[5];
u32 startTime, currentTime;

  for(i = 0; i < 5; i++) {
    commandline = GetHexValue(commandline, &regs[i]);

#warning "FIXME: GetHexValue() returns NULL after the fifth call..."

    if((commandline == NULL) && (i != 4)) {
      SerialOutputString(__FUNCTION__ ": can't get hex values\n");
      return;
    }
  }

#if 0      
  printf (" write %lx to %x \n",regs[SA_PPCR], _PPCR);
  printf (" write %lx to %x \n",regs[SA_MDCNFG], _MDCNFG);
  printf (" write %lx to %x \n",regs[SA_MDCAS0], _MDCAS0);
  printf (" write %lx to %x \n",regs[SA_MDCAS1], _MDCAS1);
  printf (" write %lx to %x \n",regs[SA_MDCAS2], _MDCAS2);
#endif
  /* we go slower, so first set PLL register */
  PPCR = regs[SA_PPCR];
  MDCNFG = regs[SA_MDCNFG];
  MDCAS0 = regs[SA_MDCAS0];
  MDCAS1 = regs[SA_MDCAS1];
  MDCAS2 = regs[SA_MDCAS2];

  /* sleep for a second */
  startTime = TimerGetTime();

  for(;;) {
    currentTime = TimerGetTime();
    if((currentTime - startTime) > (u32)TICKS_PER_SECOND)
      return;
  }
} /* SetClock */
