#include <nds.h>
#include <stdlib.h>
#include <string.h>
#include <nds/card.h>

#include "CardRead.h"
#include "keytbl.h"

// Todo use libnds current reg defines - Apache Thunder
/*#define CARD_CR1       (*(vu16*)0x040001A0)
#define CARD_CR1H      (*(vu8*)0x040001A1)
#define CARD_EEPDATA   (*(vu8*)0x040001A2)
#define CARD_CR2       (*(vu32*)0x040001A4)
#define CARD_COMMAND   ((vu8*)0x040001A8)

#define CARD_DATA_RD   (*(vu32*)0x04100010)

#define CARD_BUSY       (1<<31)
#define CARD_DATA_READY (1<<23)
//*/

#define CARD_CR1       REG_AUXSPICNT
#define CARD_CR1H      REG_AUXSPICNTH
#define CARD_EEPDATA   REG_AUXSPIDATA
#define CARD_CR2       REG_ROMCTRL
#define CARD_COMMAND   REG_CARD_COMMAND
#define CARD_DATA_RD   REG_CARD_DATA_RD

// #define CARD_BUSY       (1<<31)
// #define CARD_DATA_READY (1<<23)


u32	kkkkk;
u32	llll;
u32	iiijjj;
u32	mmmnnn;
u32	EncSeed;

// = 0xE71F << 12 0x0E71F000
// 038094fc(iiijjj) = 1
// 03809500(ChipID) = 0
// 03809504(normal)
// 03809508(Ckey1)
// 0380950c(mmmnnn) = 2
// 03809510() = 0
// 03809514 header

u32	chip_id;
u32	gamecode;
u32	cr2_normal;
u32	cr2_key1;


void Read_Init() {
	llll = 0xE71F;
	iiijjj = 1;
	mmmnnn = 2;
	kkkkk = 3;

	CARD_CR2 = 0x00;
	CARD_CR1H = 0x00;
	wait_TM1(0x0A);
	CARD_CR1H = 0xC0;
	CARD_CR2 = 0x20008000;
}


int Read_dummy() {
	int	s;
	vu32	data;

	while(CARD_CR2 & CARD_BUSY);

	CARD_CR1H = 0xC0;
	CARD_COMMAND[0] = 0x9F;
	CARD_COMMAND[1] = 0x00;
	CARD_COMMAND[2] = 0x00;
	CARD_COMMAND[3] = 0x00;
	CARD_COMMAND[4] = 0x00;
	CARD_COMMAND[5] = 0x00;
	CARD_COMMAND[6] = 0x00;
	CARD_COMMAND[7] = 0x00;

	CARD_CR2 = 0xAD180000;

	s = 0;
	do {
		if((CARD_CR2 & CARD_DATA_READY)) {
			if(s < 0x2000) {
				data = CARD_DATA_RD;
				s += 4;
			}
		}
	} while(CARD_CR2 & CARD_BUSY);

	if (data == 0) { }

	return(s);

}


int Read_Header(u8* header) {
	int	s;
	u32	data;
	u32	*buf;

	while(CARD_CR2 & CARD_BUSY);

	CARD_CR1H = 0xC0;
	CARD_COMMAND[0] = 0x00;
	CARD_COMMAND[1] = 0x00;
	CARD_COMMAND[2] = 0x00;
	CARD_COMMAND[3] = 0x00;
	CARD_COMMAND[4] = 0x00;
	CARD_COMMAND[5] = 0x00;
	CARD_COMMAND[6] = 0x00;
	CARD_COMMAND[7] = 0x00;

//	CARD_CR2 = 0xAC3F1FFF;
	CARD_CR2 = 0xA93F1FFF;
	s = 0;
	buf = (u32*)header;
	do {
		if((CARD_CR2 & CARD_DATA_READY)) {
			if(s < 0x200) {
				data = CARD_DATA_RD;
				*buf = data;
				buf++;
				s += 4;
			}
		}
	} while(CARD_CR2 & CARD_BUSY);


	gamecode = *((u32*)(header + 0x0C));
	EncSeed = (u32)(header[0x13] & 0x07);
//	cr2_normal = *((u32*)(header + 0x60));
//	cr2_key1 = *((u32*)(header + 0x64));
	port_setcmd(&cr2_normal, &cr2_key1, header);

	if((header[0x63] & 0x08) == 0x08) {
		cr2_normal |= 0x08000000;
		cr2_key1 |= 0x08000000;
	}

	return(s);
}

u32 Read_CardID_1() {
	while(CARD_CR2 & CARD_BUSY);

	CARD_CR1H = 0xC0;
	CARD_COMMAND[0] = 0x90;
	CARD_COMMAND[1] = 0x00;
	CARD_COMMAND[2] = 0x00;
	CARD_COMMAND[3] = 0x00;
	CARD_COMMAND[4] = 0x00;
	CARD_COMMAND[5] = 0x00;
	CARD_COMMAND[6] = 0x00;
	CARD_COMMAND[7] = 0x00;

//	CARD_CR2 = cr2_normal | 0xA7000000;
	CARD_CR2 = 0xA7000000;

	while(!(CARD_CR2 & CARD_DATA_READY));

	chip_id = CARD_DATA_RD;
	return(chip_id);
}


bool Act_Key1() {

	while(CARD_CR2 & CARD_BUSY);

	CARD_CR1H = 0xC0;
	CARD_COMMAND[0] = 0x3C;
	CARD_COMMAND[1] = (iiijjj >> 16) & 0xFF;
	CARD_COMMAND[2] = (iiijjj >> 8) & 0xFF;
	CARD_COMMAND[3] = iiijjj & 0xFF;

	CARD_COMMAND[4] = (kkkkk >> 16) & 0x0F;
	CARD_COMMAND[5] = (kkkkk >> 8) & 0xFF;
	CARD_COMMAND[6] = kkkkk & 0xFF;

	CARD_COMMAND[7] = 0x00;

	CARD_CR2 = 0xA0010017;

	return true;
}



//====================================================


void Act_Key2() {
	u32	cmd1, cmd2;


	cmd1 = (mmmnnn << 20) | kkkkk;
	cmd2 = 0x40000000 | (llll << 12) | (mmmnnn >> 12);
	Key1_cmd(cmd1, cmd2, gamecode, (u8*)key1tbl);
	wait_TM1(0x05);

	enc_key2(mmmnnn, key2tbl[EncSeed]);

	if(chip_id < 0x80000000) {
		CARD_CR2 = cr2_key1 | 0xB0008000;
	} else {
		CARD_CR2 = 0xA0000000;
		wait_TM1(0x1E);
		CARD_CR2 = 0xA0008000;
	}

	kkkkk++;
	kkkkk &= 0xFFFFF;
}

u32 Read_CardID_2() {
	u32	cmd1, cmd2;
	u32	data;

	cmd1 = (iiijjj << 20) | kkkkk;
	cmd2 = 0x10000000 | (llll << 12) | (iiijjj >> 12);
	Key1_cmd(cmd1, cmd2, gamecode, (u8*)key1tbl);
	wait_TM1(0x05);

	if(chip_id < 0x80000000) {
		CARD_CR2 = cr2_key1 | 0xB7006000;
	} else {
		CARD_CR2 = 0xA0006000;
		wait_TM1(0x1E);
		CARD_CR2 = 0xA7006000;
	}

	while(!(CARD_CR2 & CARD_DATA_READY));
	data = CARD_DATA_RD;

	kkkkk++;
	kkkkk &= 0xFFFFF;

	return(data);
}



int Read_SData(u8* dbuf, u32 addr) {
	u32	cmd1, cmd2;
	int	s;
	u32	data;
	u32	*buf;
	int	n;

	cmd1 = (iiijjj << 20) | kkkkk;
	cmd2 = 0x20000000 | addr | (iiijjj >> 12);
	Key1_cmd(cmd1, cmd2, gamecode, (u8*)key1tbl);
	wait_TM1(0x05);


	s = 0;
	buf = (u32*)dbuf;

	if(chip_id < 0x80000000) {
		CARD_CR2 = cr2_normal | 0xB4006000;
		do {
			if((CARD_CR2 & CARD_DATA_READY)) {
				if(s < 0x1000) {
					data = CARD_DATA_RD;
					*buf = data;
					buf++;
					s += 4;
				}
			}
		} while(CARD_CR2 & CARD_BUSY);
	} else {
		CARD_CR2 = cr2_normal | 0xA0006000;
		wait_TM1(0x1E);
		for(n = 0; n < 8; n++) {
			CARD_CR2 = cr2_normal | 0xA1006000;
			do {
				if((CARD_CR2 & CARD_DATA_READY)) {
					if(s < 0x1000) {
						data = CARD_DATA_RD;
						*buf = data;
						buf++;
						s += 4;
					}
				}
			} while(CARD_CR2 & CARD_BUSY);
		}
	}

	kkkkk++;
	kkkkk &= 0xFFFFF;

	return(s);

}


void Data_Mode() {
	u32	cmd1, cmd2;


	cmd1 = (iiijjj << 20) | kkkkk;
	cmd2 = 0xA0000000 | (llll << 12) | (iiijjj >> 12);
	Key1_cmd(cmd1, cmd2, gamecode, (u8*)key1tbl);
	wait_TM1(0x05);

	enc_key2(mmmnnn, key2tbl[EncSeed]);


	if(chip_id < 0x80000000) {
		CARD_CR2 = cr2_key1 | 0xB0006000;
	} else {
		CARD_CR2 = cr2_key1 | 0xA0006000;
		wait_TM1(0x1E);
		CARD_CR2 = cr2_key1 | 0xA0006000;
	}

	kkkkk++;
	kkkkk &= 0xFFFFF;

}



int Read_Data(u8* dbuf, u32 addr) {
	
	cardParamCommand (CARD_CMD_DATA_READ, addr, (cr2_normal & 0xF8FFFFFF) | 0xA1406000, (void*)dbuf, 0x200/sizeof(u32));
	
	return 1;
}

u32 Read_CardID_3() {
	while(CARD_CR2 & CARD_BUSY);

	CARD_CR1H = 0xC0;
	CARD_COMMAND[0] = 0xB8;
	CARD_COMMAND[1] = 0x00;
	CARD_COMMAND[2] = 0x00;
	CARD_COMMAND[3] = 0x00;
	CARD_COMMAND[4] = 0x00;
	CARD_COMMAND[5] = 0x00;
	CARD_COMMAND[6] = 0x00;
	CARD_COMMAND[7] = 0x00;

//	CARD_CR2 = cr2_normal | 0xA7000000;
	CARD_CR2 = 0xA7416017;

	while(!(CARD_CR2 & CARD_DATA_READY));

	return(CARD_DATA_RD);
}


u32	Rom_Read(int type, u8* header, u8* sc1) {
	u32	id, id2, id3;
	u32	i;
	u8	*ptr;
	
	if (isDSiMode() && (REG_SCFG_EXT & BIT(31))) {
		if (REG_SCFG_MC == 0x10) {
			enableSlot1();
			for (int I = 0; I < 20; I++)swiWaitForVBlank();
		} else {
			disableSlot1();
			for (int I = 0; I < 20; I++)swiWaitForVBlank();
			enableSlot1();
			for (int I = 0; I < 20; I++)swiWaitForVBlank();
		}
	}

	if(type == 0)Read_Init();
	
	Read_dummy();

	id = Read_CardID_1();

	if(type == 0)Read_dummy();

	wait_TM1(0x0A);

	Read_Header(header);
	wait_TM1(0x1F4);

	Act_Key1();

	if(id == 0 || id == 0xFFFFFFFF)return(0xFFFFFFFF);

	Act_Key2();
	id2 = Read_CardID_2();
	if(id != id2)	return(0xFFFFFFFF);

	ptr = sc1;
	for(i = 0x4000; i < 0x8000; i += 0x1000) {
		Read_SData(ptr, i);
		ptr += 0x1000;
	}

	id2 = Read_CardID_2();
	if(id != id2)	return(0xFFFFFFFF);

	Data_Mode();

	id3 = Read_CardID_3();
	if(id != id3)	return(0xFFFFFFFF);

	encryObj(gamecode, sc1, (u8*)key1tbl);

	return(id);
}

