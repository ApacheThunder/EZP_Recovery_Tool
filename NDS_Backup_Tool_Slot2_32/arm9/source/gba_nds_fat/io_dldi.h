/*
	io_dldi.h 

	Reserved space for new drivers
	
	This software is completely free. No warranty is provided.
	If you use it, please give me credit and email me about your
	project at chishm@hotmail.com

	See gba_nds_fat.txt for help and license details.
*/

#ifndef IO_DLDI_H
#define IO_DLDI_H

#define WAIT_CR	REG_EXMEMCNT

// 'DLDI'
#define DEVICE_TYPE_DLDD 0x49444C44

#include "disc_io.h"
#ifdef NDS
#include <nds/memory.h>
//#include <nds/registers_alt.h>
#endif

extern IO_INTERFACE _io_dldi;

// export interface
static inline LPIO_INTERFACE DLDI_GetInterface(void) {
#ifdef NDS
//	WAIT_CR &= ~(ARM9_OWNS_ROM | ARM9_OWNS_CARD);
	WAIT_CR &= ~(0x8080);
#endif // defined NDS
	return &_io_dldi;
}


static inline const char* DLDI_GetAdapterName(void) {
  u32 adr=(u32)&_io_dldi;
  return((const char*)(adr-0x50));
}


#endif	// define IO_DLDI_H
