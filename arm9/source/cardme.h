/*
	cardme by SaTa. for Public Domain
	2005/10/30
		NEW TYPE 2 ASPJ has 512kbit(64Kbyte)EEPROM
	2005/10/21
		NEW TYPE 3 
		cardmeChipErase use cardmeSectorErase
*/
#ifndef CARDME_HEADER_FIX_NDSLIB_HELP__WATASI_MOMOI_SUKIDESU_YOROSIKU
#define CARDME_HEADER_FIX_NDSLIB_HELP__WATASI_MOMOI_SUKIDESU_YOROSIKU

#include <nds.h>
#include <string.h>
//#include "dbg.h"

#ifdef __cplusplus
extern "C" {
#endif

void spiTransfer(int rlen, u8 *dst, int cmdlen, ...);

void cardmeReadEeprom(u32 address, u8 *data, u32 length, u32 addrtype);
void cardmeSectorErase(u32 address);
// int cardmeChipErase_old(void);
int cardmeChipErase(void);	//	USE TO TYPE 3 FLASH MEMORY ONLY



int cardmeGetType(void);	//
/*
 -1:no card or no EEPROM
  0:unknown                 PassMe?
  1:TYPE 1  4Kbit(512Byte)  EEPROM
  2:TYPE 2 64Kbit(8KByte)or 512kbit(64Kbyte)   EEPROM
  3:TYPE 3  2Mbit(256KByte) FLASH MEMORY
*/
int cardmeSize(int tp);
/*
	tp:1~3
	return:size (byte)
*/
u8 cardmeReadID(int i);	//	don't work ??
u8 cardmeCMD(u8 cmd,int address);

void cardmeReadHeader(uint8 * header);
void cardmeWriteEeprom(u32 address, u8 *data, u32 length, u32 addrtype);

#ifdef __cplusplus
}
#endif

#endif //CARDME_HEADER_FIX_NDSLIB_HELP__WATASI_MOMOI_SUKIDESU_YOROSIKU

