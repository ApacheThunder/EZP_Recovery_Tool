/*
	see:CARDME.H
*/

#include <nds/card.h>
#include <stdio.h>
#include <stdarg.h>

#include "cardme.h"

#define OFOT0 (8*1024-1)	//	8KBの末端
#define OFOT1 (2*8*1024-1)	//	16KBの末端
#define T1BLOCKSZ (16)
#define T3BLOCKSZ (256)

void EepromWaitBusy() {
	while (REG_AUXSPICNT & /*BUSY*/0x80);
}

// custom SPI function courtasy of Nat (nathaantfm)
void spiTransfer(int rlen, u8 *dst, int cmdlen, ...) {
    va_list ap;
	int i;
    // Enable transfer
    REG_AUXSPICNT = 0x8000 | 0x2000 | 0x40;

    // Send command
    va_start(ap, cmdlen);
    for (i = 0; i < cmdlen; i++) {
        REG_AUXSPIDATA = (u8)va_arg(ap, int);
        EepromWaitBusy();
    }
	
	va_end(ap);
	
    // Read data
    for (i = 0; i < rlen; i++) {
        REG_AUXSPIDATA = 0; // dummy byte
        EepromWaitBusy();
        *dst++ = REG_AUXSPIDATA;
    }

    // End transfer
    REG_AUXSPICNT = 0x40;
}

u8 cardmeCMD(u8 cmd,int address) { 
//	int address=i & 1;
	u8 ret;
	int k;
    REG_AUXSPICNT = /*E*/0x8000 | /*SEL*/0x2000 | /*MODE*/0x40; 

    REG_AUXSPICNT = 0xFFFF; 
    REG_AUXSPIDATA = cmd;	// 

    EepromWaitBusy(); 

	REG_AUXSPIDATA = (address >> 16) & 0xFF; 
	EepromWaitBusy(); 

	REG_AUXSPIDATA = (address >> 8) & 0xFF; 
	EepromWaitBusy(); 

    REG_AUXSPIDATA = (address) & 0xFF; 
    EepromWaitBusy(); 

	for(k=0;k<256;k++) {
		ret = REG_AUXSPIDATA; 
		if(ret!=0xFF)break;
		EepromWaitBusy(); 
	}
    REG_AUXSPICNT = /*MODE*/0x40; 
    return ret;
} 


u8 cardmeReadID(int i) { 
	int address=i & 1;
	u8 ret;
	int k;
    REG_AUXSPICNT = /*E*/0x8000 | /*SEL*/0x2000 | /*MODE*/0x40; 

    REG_AUXSPICNT = 0xFFFF; 
    REG_AUXSPIDATA = 0xAB;	// READ ID

    EepromWaitBusy(); 

	REG_AUXSPIDATA = (address >> 16) & 0xFF; 
	EepromWaitBusy(); 

	REG_AUXSPIDATA = (address >> 8) & 0xFF; 
    EepromWaitBusy(); 

    REG_AUXSPIDATA = (address) & 0xFF; 
    EepromWaitBusy();

	for(k=0;k<16;k++) {
		ret = REG_AUXSPIDATA; 
		if(ret!=0xFF)break;
		EepromWaitBusy(); 
	}
    REG_AUXSPICNT = /*MODE*/0x40; 
    return ret;
} 


extern	int cardmeSizeT2(void);


int cardmeGetType(void) {
	u8 c00;
	u8 c05;
	u8 c9f;
	// u8 reg;
	
	REG_EXMEMCNT &= ~0x0880;	// DS Card access ARM9:bit11=0   GBA Cart access ARM9:bit7=0
	cardmeCMD(0x03,0);
	c05=cardmeCMD(0x05,0);
	c9f=cardmeCMD(0x9f,0);
	c00=cardmeCMD(0x00,0);
	
	// Test code for dumping IDs of current save chip
	/*FILE *testFile = fopen("/saveChipID.bin", "wb");
	if (testFile) {
		// u8 chipID[3] = { c00, c05, c9f };
		u8 chipId[3];
		spiTransfer(3, &chipId, 1, 0x9F);
		fwrite(testID, 3, 1, testFile);
		fclose(testFile);
	}*/

	/*FILE *testFile2 = fopen("/registerData.bin", "wb");
	if (testFile2) {
		u8 securityReg1[1];
		u8 securityReg2[1];
		spiTransfer(1, securityReg1, 1, 0x5);
		spiTransfer(1, securityReg2, 1, 0x35);
		do {
			spiTransfer(1, &reg, 1, 0x5); // Read status register
		} while (reg & 0x1); // Write in progress
		
		fwrite(securityReg1, 1, 1, testFile2);
		fwrite(securityReg2, 1, 1, testFile2);
		fclose(testFile2);
	}*/
	
	// Disabled these checks so it will always return default incase EZP is bricked.
	// if((c00==0x00) && (c9f==0x00))return 0;	//	PassMe? 判別不可
	// if((c00==0xff) && (c05==0xff) && (c9f==0xff))return -1;

	if((c00==0xff) &&  (c05 & 0xFD) == 0xF0 && (c9f==0xff))return 1;
	if((c00==0xff) &&  (c05 & 0xFD) == 0x00 && (c9f==0xff))return 2;
	if((c00==0xff) &&  (c05 & 0xFD) == 0x00 && (c9f==0x00))return 3;
	if((c00==0xff) &&  (c05 & 0xFD) == 0x00 && (c9f==0x12))return 3;	//	NEW TYPE 3
	if((c00==0xff) &&  (c05 & 0xFD) == 0x00 && (c9f==0x13))return 3;	//	NEW TYPE 3+ 4Mbit
	if((c00==0xff) &&  (c05 & 0xFD) == 0x00 && (c9f==0x14))return 3;	//	NEW TYPE 3+ 8Mbit
	if((c00==0xff) &&  (c05 & 0xFD) == 0x00 && (c9f==0x16))return 3;	//	NEW TYPE 3+ 32Mbit
	if((c00==0xff) &&  (c05 & 0xFD) == 0xF0 && (c9f==0x16))return 3;	//	NEW TYPE 3+ 32Mbit (Used by EZP Flashcart)
	if((c00==0xff) &&  (c05 & 0xFD) == 0x84 && (c9f==0x17))return 3;	//	NEW TYPE 3+ 64Mbit

	//	/* 判定不可 */
	if(cardmeSizeT2() != 0)return 2;
	// return 0;
	return 3; // Will always return default incase EZP is bricked.
}

int cardmeSizeT3(void) {
	u8 c9f;
	c9f=cardmeCMD(0x9f,0);

	switch (c9f) {
		case 0x13: return 512*1024;		// NEW TYPE 3+ 4Mbit(512MByte)
		case 0x14: return 1024*1024;	// NEW TYPE 3+ 8Mbit(1024MByte)
		case 0x16: return 4096*1024;	// NEW TYPE 3+ 32Mbit(4MByte) // (Used by EZP Flashcart)
		case 0x17: return 8192*1024;	// NEW TYPE 3+ 64Mbit(8192MByte)
		default: return 4096*1024;		// NEW TYPE 3+ 32Mbit(4MByte) // (Used by EZP Flashcart)
		// default: return 256*1024;	// TYPE 3 2Mbit(256KByte)
	}
}

int cardmeSizeT2(void) {
	int tp=2;

	u8 buf1[1];	//	+0k data	read -> write
	u8 buf2[1];	//	+8k data	read -> read
	u8 buf3[1];	//	+0k ~data	   write
	u8 buf4[1];	//	+8k data new	comp buf2

	cardmeReadEeprom(OFOT0,buf1,1,tp);
	cardmeReadEeprom(OFOT1,buf2,1,tp);
	buf3[0]=~buf2[0];

	cardmeWriteEeprom(OFOT0,buf3,1,tp);
	cardmeReadEeprom(OFOT0,buf4,1,tp);
	cardmeWriteEeprom(OFOT0,buf1,1,tp);
	if(buf4[0]!=buf3[0])return 0; // Not type 2

	cardmeReadEeprom(OFOT1,buf4,1,tp);
	
	//	+8kも書き換わっている
	if(buf4[0]!=buf2[0])return 8*1024;	//	 8KB(64kbit)
	
	return 64*1024;		//	64KB(512kbit)
}

int cardmeSize(int tp) {
	if(-1 <= tp && tp <= 3) {
		//            -1    0    1    2         3
		int eepsz[5]={ 0,8192, 512,	64*1024,	256*1024};
		if((tp==2) || (tp==0))return cardmeSizeT2();
		if(tp==3)return cardmeSizeT3();
		tp++;
		return eepsz[tp];
	}
	return 0;
}


//////////////////////////////////////////////////////////////////////
void cardmeReadEeprom(u32 address, u8 *data, u32 length, u32 addrtype) { 
    REG_AUXSPICNT = /*E*/0x8000 | /*SEL*/0x2000 | /*MODE*/0x40; 
//    REG_AUXSPICNT = 0xFFFF; 
    REG_AUXSPIDATA = 0x03 | ((addrtype == 1) ? address>>8<<3 : 0);
    EepromWaitBusy(); 

    if (addrtype == 3) { 
		REG_AUXSPIDATA = (address >> 16) & 0xFF; 
		EepromWaitBusy(); 
    } 
    if (addrtype >= 2) { 
		REG_AUXSPIDATA = (address >> 8) & 0xFF; 
		EepromWaitBusy(); 
    } 
    REG_AUXSPIDATA = (address) & 0xFF; 
    EepromWaitBusy(); 

    while (length > 0) { 
		REG_AUXSPIDATA = 0; 
		EepromWaitBusy(); 
		*data++ = REG_AUXSPIDATA; 
		length--; 
    } 
	EepromWaitBusy(); 
    REG_AUXSPICNT = /*MODE*/0x40; 
} 
//////////////////////////////////////////////////////////////////////

//  TYPE1 4kbit EEPROM
void cardmeWriteEepromT1(u32 address, u8 *apData, u32 aLength, u32 addrtype) {
	int i;
	u32 last=address+aLength;

	while (address < last) {
		// set WEL (Write Enable Latch)
		REG_AUXSPICNT = /*E*/0x8000 | /*SEL*/0x2000 | /*MODE*/0x40;
		REG_AUXSPIDATA = 0x06; 
        EepromWaitBusy();

		REG_AUXSPICNT = /*MODE*/0x40;

		// program maximum of 16 bytes
		REG_AUXSPICNT = /*E*/0x8000 | /*SEL*/0x2000 | /*MODE*/0x40;
		//	WRITE COMMAND 0x02 + A8 << 3
		REG_AUXSPIDATA = 0x02 | (address & BIT(8)) >> (8-3) ; 
        EepromWaitBusy();
		REG_AUXSPIDATA = address & 0xFF; 
        EepromWaitBusy();


		for (i = 0; address < last && i < T1BLOCKSZ; i++, address++) { 
            REG_AUXSPIDATA = *apData; 
            EepromWaitBusy(); 
            apData++;
        }

		REG_AUXSPICNT = /*MODE*/0x40;

		// wait programming to finish
		REG_AUXSPICNT = /*E*/0x8000 | /*SEL*/0x2000 | /*MODE*/0x40;
		REG_AUXSPIDATA = 0x05; 
        EepromWaitBusy();

		do { 
			REG_AUXSPIDATA = 0; 
			EepromWaitBusy(); 
        } while (REG_AUXSPIDATA & 0x01);	// WIP (Write In Progress) ?
        EepromWaitBusy(); 
		REG_AUXSPICNT = /*MODE*/0x40;
	}
}



//  TYPE2 64kbit EEPROM
void cardmeWriteEepromT2(u32 address, u8 *apData, u32 aLength, u32 addrtype) {
	int i;
	u32 last=address+aLength;

	while (address < last) {
		// set WEL (Write Enable Latch)
		REG_AUXSPICNT = /*E*/0x8000 | /*SEL*/0x2000 | /*MODE*/0x40;
		REG_AUXSPIDATA = 0x06; 
        EepromWaitBusy();

		REG_AUXSPICNT = /*MODE*/0x40;

		// program maximum of 32 bytes
		REG_AUXSPICNT = /*E*/0x8000 | /*SEL*/0x2000 | /*MODE*/0x40;
		REG_AUXSPIDATA = 0x02; 
        EepromWaitBusy();
        REG_AUXSPIDATA = address >> 8; 
        EepromWaitBusy(); 
		REG_AUXSPIDATA = address & 0xFF; 
        EepromWaitBusy();


		for (i = 0; address < last && i < 32; i++, address++) { 
			REG_AUXSPIDATA = *apData; 
			EepromWaitBusy(); 
			apData++;
        }

		REG_AUXSPICNT = /*MODE*/0x40;

		// wait programming to finish
		REG_AUXSPICNT = /*E*/0x8000 | /*SEL*/0x2000 | /*MODE*/0x40;
		REG_AUXSPIDATA = 0x05; 
        EepromWaitBusy();

		do {
			REG_AUXSPIDATA = 0; 
			EepromWaitBusy(); 
        } while (REG_AUXSPIDATA & 0x01);	// WIP (Write In Progress) ?
        EepromWaitBusy(); 
		REG_AUXSPICNT = /*MODE*/0x40;
	}
}

//  TYPE3 2Mbit FLASH MEMORY
void cardmeWriteEepromT3(u32 address, u8 *apData, u32 aLength, u32 addrtype) {
	int i;
	u32 last=address+aLength;

	while (address < last) {
		// set WEL (Write Enable Latch)
		REG_AUXSPICNT = /*E*/0x8000 | /*SEL*/0x2000 | /*MODE*/0x40;
		REG_AUXSPIDATA = 0x06; 
        EepromWaitBusy();

		REG_AUXSPICNT = /*MODE*/0x40;

		// program maximum of 32 bytes
		REG_AUXSPICNT = /*E*/0x8000 | /*SEL*/0x2000 | /*MODE*/0x40;
		REG_AUXSPIDATA = 0x02; 
        EepromWaitBusy();
        REG_AUXSPIDATA = (address >> 16) & 0xFF; 
        EepromWaitBusy(); 
        REG_AUXSPIDATA = (address >> 8) & 0xFF; 
        EepromWaitBusy(); 
		REG_AUXSPIDATA = address & 0xFF; 
        EepromWaitBusy();


		for (i = 0; address < last && i < T3BLOCKSZ; i++, address++) {
			REG_AUXSPIDATA = *apData; 
			EepromWaitBusy(); 
			apData++;
        }

		REG_AUXSPICNT = /*MODE*/0x40;

		// wait programming to finish
		REG_AUXSPICNT = /*E*/0x8000 | /*SEL*/0x2000 | /*MODE*/0x40;
		REG_AUXSPIDATA = 0x05; 
        EepromWaitBusy();

		do {
			REG_AUXSPIDATA = 0; 
			EepromWaitBusy(); 
        } while (REG_AUXSPIDATA & 0x01);	// WIP (Write In Progress) ?
        EepromWaitBusy(); 
		REG_AUXSPICNT = /*MODE*/0x40;
	}

}

//	COMMAND Sec.erase 0xD8 
void cardmeSectorErase(u32 address) {
	// int i;
	// set WEL (Write Enable Latch)
	REG_AUXSPICNT = /*E*/0x8000 | /*SEL*/0x2000 | /*MODE*/0x40;
	REG_AUXSPIDATA = 0x06; 
    EepromWaitBusy();

	REG_AUXSPICNT = /*MODE*/0x40;

	// SectorErase 0xD8
	REG_AUXSPICNT = /*E*/0x8000 | /*SEL*/0x2000 | /*MODE*/0x40;
	REG_AUXSPIDATA = 0xD8; 
    EepromWaitBusy();
    REG_AUXSPIDATA = (address >> 16) & 0xFF; 
    EepromWaitBusy(); 
    REG_AUXSPIDATA = (address >> 8) & 0xFF; 
    EepromWaitBusy(); 
	REG_AUXSPIDATA = address & 0xFF; 
    EepromWaitBusy();

	REG_AUXSPICNT = /*MODE*/0x40;

	// wait erase to finish
	REG_AUXSPICNT = /*E*/0x8000 | /*SEL*/0x2000 | /*MODE*/0x40;
	REG_AUXSPIDATA = 0x05; 
    EepromWaitBusy();

	do {
		REG_AUXSPIDATA = 0; 
		EepromWaitBusy(); 
    } while (REG_AUXSPIDATA & 0x01);	// WIP (Write In Progress) ?
//    EepromWaitBusy(); 
	REG_AUXSPICNT = /*MODE*/0x40;
}

//	Chip Erase NEW : clear FLASH MEMORY (TYPE 3 ONLY)
int cardmeChipErase(void) {
		
	int sz;
	void cardmeSectorErase(u32 address);
	sz = cardmeSizeT3();
	
	cardmeSectorErase(0x00000);
	cardmeSectorErase(0x10000);
	cardmeSectorErase(0x20000);
	cardmeSectorErase(0x30000);
	if(sz==512*1024) {
		cardmeSectorErase(0x40000);
		cardmeSectorErase(0x50000);
		cardmeSectorErase(0x60000);
		cardmeSectorErase(0x70000);
	}
	if(sz==1024*1024) {
		cardmeSectorErase(0x80000);
		cardmeSectorErase(0x90000);
		cardmeSectorErase(0xA0000);
		cardmeSectorErase(0xB0000);
		cardmeSectorErase(0xC0000);
		cardmeSectorErase(0xD0000);
		cardmeSectorErase(0xE0000);
		cardmeSectorErase(0xF0000);
	}
	if(sz==4096*2024) {
		cardmeSectorErase(0x100000);
		cardmeSectorErase(0x110000);
		cardmeSectorErase(0x120000);
		cardmeSectorErase(0x130000);
		cardmeSectorErase(0x140000);
		cardmeSectorErase(0x150000);
		cardmeSectorErase(0x160000);
		cardmeSectorErase(0x170000);
		cardmeSectorErase(0x180000);
		cardmeSectorErase(0x190000);
		cardmeSectorErase(0x1A0000);
		cardmeSectorErase(0x1B0000);
		cardmeSectorErase(0x1C0000);
		cardmeSectorErase(0x1D0000);
		cardmeSectorErase(0x1E0000);
		cardmeSectorErase(0x1F0000);
		cardmeSectorErase(0x200000);
		cardmeSectorErase(0x210000);
		cardmeSectorErase(0x220000);
		cardmeSectorErase(0x230000);
		cardmeSectorErase(0x240000);
		cardmeSectorErase(0x250000);
		cardmeSectorErase(0x260000);
		cardmeSectorErase(0x270000);
		cardmeSectorErase(0x280000);
		cardmeSectorErase(0x290000);
		cardmeSectorErase(0x2A0000);
		cardmeSectorErase(0x2B0000);
		cardmeSectorErase(0x2C0000);
		cardmeSectorErase(0x2D0000);
		cardmeSectorErase(0x2E0000);
		cardmeSectorErase(0x2F0000);
		cardmeSectorErase(0x300000);
		cardmeSectorErase(0x310000);
		cardmeSectorErase(0x320000);
		cardmeSectorErase(0x330000);
		cardmeSectorErase(0x340000);
		cardmeSectorErase(0x350000);
		cardmeSectorErase(0x360000);
		cardmeSectorErase(0x370000);
		cardmeSectorErase(0x380000);
		cardmeSectorErase(0x390000);
		cardmeSectorErase(0x3A0000);
		cardmeSectorErase(0x3B0000);
		cardmeSectorErase(0x3C0000);
		cardmeSectorErase(0x3D0000);
		cardmeSectorErase(0x3E0000);
		cardmeSectorErase(0x3F0000);
	}
	if (sz==((4096*2024)*2)) {
		cardmeSectorErase(0x400000);
		cardmeSectorErase(0x410000);
		cardmeSectorErase(0x420000);
		cardmeSectorErase(0x430000);
		cardmeSectorErase(0x440000);
		cardmeSectorErase(0x450000);
		cardmeSectorErase(0x460000);
		cardmeSectorErase(0x470000);
		cardmeSectorErase(0x480000);
		cardmeSectorErase(0x490000);
		cardmeSectorErase(0x4A0000);
		cardmeSectorErase(0x4B0000);
		cardmeSectorErase(0x4C0000);
		cardmeSectorErase(0x4D0000);
		cardmeSectorErase(0x4E0000);
		cardmeSectorErase(0x4F0000);
		cardmeSectorErase(0x500000);
		cardmeSectorErase(0x510000);
		cardmeSectorErase(0x520000);
		cardmeSectorErase(0x530000);
		cardmeSectorErase(0x540000);
		cardmeSectorErase(0x550000);
		cardmeSectorErase(0x560000);
		cardmeSectorErase(0x570000);
		cardmeSectorErase(0x580000);
		cardmeSectorErase(0x590000);
		cardmeSectorErase(0x5A0000);
		cardmeSectorErase(0x5B0000);
		cardmeSectorErase(0x5C0000);
		cardmeSectorErase(0x5D0000);
		cardmeSectorErase(0x5E0000);
		cardmeSectorErase(0x5F0000);
		cardmeSectorErase(0x600000);
		cardmeSectorErase(0x610000);
		cardmeSectorErase(0x620000);
		cardmeSectorErase(0x630000);
		cardmeSectorErase(0x640000);
		cardmeSectorErase(0x650000);
		cardmeSectorErase(0x660000);
		cardmeSectorErase(0x670000);
		cardmeSectorErase(0x680000);
		cardmeSectorErase(0x690000);
		cardmeSectorErase(0x6A0000);
		cardmeSectorErase(0x6B0000);
		cardmeSectorErase(0x6C0000);
		cardmeSectorErase(0x6D0000);
		cardmeSectorErase(0x6E0000);
		cardmeSectorErase(0x6F0000);
		cardmeSectorErase(0x700000);
		cardmeSectorErase(0x710000);
		cardmeSectorErase(0x720000);
		cardmeSectorErase(0x730000);
		cardmeSectorErase(0x740000);
		cardmeSectorErase(0x750000);
		cardmeSectorErase(0x760000);
		cardmeSectorErase(0x770000);
		cardmeSectorErase(0x780000);
		cardmeSectorErase(0x790000);
		cardmeSectorErase(0x7A0000);
		cardmeSectorErase(0x7B0000);
		cardmeSectorErase(0x7C0000);
		cardmeSectorErase(0x7D0000);
		cardmeSectorErase(0x7E0000);
		cardmeSectorErase(0x7F0000);
	}
	return 0;
}

//	Chip Erase  : clear FLASH MEMORY (TYPE 3 ONLY)
//	COMMAND 0xC7
/*int cardmeChipErase_old(void) {
	// {
	// set WEL (Write Enable Latch)
	REG_AUXSPICNT = 0x8000 | 0x2000 | 0x40;
	REG_AUXSPIDATA = 0x06; 
    EepromWaitBusy();

	REG_AUXSPICNT = 0x40;

	// Chip Erase 0xC7
	REG_AUXSPICNT = 0x8000 | 0x2000 | 0x40;
	REG_AUXSPIDATA = 0xC7; 
    EepromWaitBusy();

	REG_AUXSPICNT = 0x40;

	// wait programming to finish
	REG_AUXSPICNT = 0x8000 | 0x2000 | 0x40;
	REG_AUXSPIDATA = 0x05; 
    EepromWaitBusy();

	do {
		REG_AUXSPIDATA = 0; 
		EepromWaitBusy(); 
    } while (REG_AUXSPIDATA & 0x01);	// WIP (Write In Progress) ?
    EepromWaitBusy(); 
	REG_AUXSPICNT = 0x40;

	// }
	return 0;
}*/


void cardmeWriteEeprom(u32 address, u8 *apData, u32 aLength, u32 addrtype) {
	switch(addrtype) {
		case 1: {
			cardmeWriteEepromT1(address,apData,aLength,addrtype);
			return;
		}break;
		case 2: {
			cardmeWriteEepromT2(address,apData,aLength,addrtype);
			return;
		}break;
		case 3: {
			cardmeWriteEepromT3(address,apData,aLength,addrtype);
			return;
		}break;
	}
}

