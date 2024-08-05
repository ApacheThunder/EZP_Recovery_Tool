/*
    NitroHax -- Cheat tool for the Nintendo DS
    Copyright (C) 2008  Michael "Chishm" Chisholm

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

#ifndef READ_CARD_H
#define READ_CARD_H

#include <nds/ndstypes.h>
#include <nds/memory.h>
#include <stdlib.h>

#include "ndsheaderbanner.h"

#define CartTempHeaderNTR 0x027FE940
#define CartTempHeaderTWL 0x02FFFA80
#define CartTempHeaderTWLCart 0x02FFC000

#define CartTempChipIDNTR 0x027FF800
#define CartTempChipIDTWL 0x02FFFC00

#define CARD_NDS_HEADER_SIZE (0x200)
#define CARD_SECURE_AREA_OFFSET (0x4000)
#define CARD_SECURE_AREA_SIZE (0x4000)
#define CARD_DATA_OFFSET (0x8000)
#define CARD_DATA_BLOCK_SIZE (0x200)
#define MODC_AREA_SIZE          0x4000

#ifdef __cplusplus
extern "C" {
#endif

extern enum {
	ERR_NONE         = 0x00,
	ERR_STS_CLR_MEM  = 0x01,
	ERR_STS_LOAD_BIN = 0x02,
	ERR_STS_HOOK_BIN = 0x03,
	ERR_STS_START    = 0x04,
	// initCard error codes:
	ERR_LOAD_NORM = 0x11,
	ERR_LOAD_OTHR = 0x12,
	ERR_SEC_NORM  = 0x13,
	ERR_SEC_OTHR  = 0x14,
	ERR_LOGO_CRC  = 0x15,
	ERR_HEAD_CRC  = 0x16,
} ERROR_CODES;

extern bool twlBlowfish;
extern u32 cardNandRomEnd;
extern u32 cardNandRwStart;

u16 switchToTwlBlowfish(sNDSHeaderExt* ndsHeader);
u32 cardInit (sNDSHeaderExt* ndsHeader);

void cardRead (u32 src, void* dest, bool nandSave);

u32 cardGetId (void);

void cardWriteNand (void* src, u32 dest);

#ifdef __cplusplus
}
#endif

#endif // READ_CARD_H

