#include <nds.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "read_card.h"


//#include "ftp.h"
//#include "CardRead.h"
// #include "disc_io.h"
// #include "gba_nds_fat.h"

#include "cardme.h"
#include "command.h"
#include "sd.h"

extern	char	GameTitle[13];
extern	char	Gamecode[5];
extern	char	RomVer;
extern	u32	Devicecapacity;
extern	u32	UsedROMsize;
extern bool useNewCardlib;

extern	int	savetype;
extern	u32	savesize;

extern	char	*romhead;
extern	char	*romsc1;
extern	char	*romsc2;


#define FAT_FT_END (0)
#define FAT_FT_FILE (1)
#define FAT_FT_DIR (2)


DTCM_DATA u8	savebuf[512];
DTCM_DATA char	fname[256];

//extern	u32	arm9fifo;
extern "C" {
//	void FIFOSend(u32 val);
	void dsp_bar(int mod, int per);
	int	Read_Data(u8* dbuf, u32 addr);
}


int SaveBK_upd(char *name) {

	FILE	*savFile;
//	int	ret;
	u32	add;
	int	per;
	u32	siz;

	dsp_bar(0, -1);
	sprintf(fname, "%s/%s", ini.dir, name);
	savFile = fopen(fname, "wb");
	if(savFile == NULL) {
		dsp_bar(-1, 0);
		return false;
	}

	siz = ini.save * 1024;
	if(siz < savesize)
		siz = savesize;

	for(add = 0; add < savesize; add += 512) {
		cardmeReadEeprom(add, savebuf, 512, savetype); 
		per = (add * 100) / siz;
		dsp_bar(0, per);
		fwrite((char *)savebuf, 512, 1, savFile);
	}

	memset((char *)savebuf, 0xFF, 512);
	for(; add < siz; add += 512) {
		per = (add * 100) / siz;
		dsp_bar(0, per);
		fwrite((char *)savebuf, 512, 1, savFile);
	}
	dsp_bar(0, 100);

	fclose(savFile);
	dsp_bar(-1, 0);

//	if(FTP_FileSize(name) != (int)siz)
//		return false;

	return true;
}


void SaveBK_new(char *name) {

	int	no;

	dsp_bar(2, -2);

	for(no = 0; no < 100; no++) {
		sprintf(name, "%s_%s_%02d.sav", GameTitle, Gamecode, no);

		sprintf(fname, "%s/%s", ini.dir, name);
		// if(FAT_FileExists(fname) != FAT_FT_FILE)
		if(access(fname, F_OK) != 0)
			break;
	}

	if(no == 100) {
		no = 0;
		sprintf(name, "%s_%s_%02d.sav", GameTitle, Gamecode, no);
	}

	dsp_bar(-1, 0);

	return;
}

/***
extern "C" {
extern void ShinoPrint_SUB( uint16* screen, int x, int y, u8 *st, u16 Fpalet, u16 Bpalet, u8 kaki );
extern	u32 inp_key();
}
extern uint16* SubScreen;
***/


int Save_Rest(char *name) {

	FILE	*savFile;
//	int	ret;
	u32	add;
	int	per;
	int	len;

	if(savetype == 3) {
		dsp_bar(3, -1);
		for(add = 0; add < savesize; add += 0x10000) {
			per = (add * 100) / savesize;
			dsp_bar(3, per);
			cardmeSectorErase(add);
		}
		dsp_bar(-1, 0);
	}

	dsp_bar(1, -1);
	sprintf(fname, "%s/%s", ini.dir, name);
	
	if (access(fname, F_OK) != 0) {
		dsp_bar(-1, 0);
		return false;
	}
	
	savFile = fopen(fname, "rb");
	if(!savFile) {
		dsp_bar(-1, 0);
		return false;
	}

	u32 fileLength = 0;
	
	fseek(savFile, 0, SEEK_END);
	fileLength = ftell(savFile);
	fseek(savFile, 0, SEEK_SET);
	
	if(fileLength != savesize) {
		dsp_bar(-1, 0);
		return false;
	}
	

	for(add = 0; add < savesize; ) {
		per = (add * 100) / savesize;
		dsp_bar(1, per);
		len = savesize - add;
		if(len > 512)	len = 512;
		// len = fread((char *)savebuf, len, 1, savFile);
		len = fread((char *)savebuf, 1, len, savFile);
		if(len > 0) {
			cardmeWriteEeprom(add, savebuf, len, savetype);
			add += len;
		}
	}
	
	dsp_bar(1, 100);

	fclose(savFile);
	dsp_bar(-1, 0);

	return true;
}

int Save_Init() {

	u32	add;
	int	per;
	int	len;

	if(savetype == 3) {
		dsp_bar(3, -1);
		for(add = 0; add < savesize; add += 0x10000) {
			per = (add * 100) / savesize;
			dsp_bar(3, per);
			cardmeSectorErase(add);
		}
		dsp_bar(3, 100);
		dsp_bar(-1, 0);
		return true;
	}

	dsp_bar(3, -1);
	memset((char *)savebuf, 0xFF, 512);
	for(add = 0; add < savesize; ) {
		per = (add * 100) / savesize;
		dsp_bar(3, per);
		len = savesize - add;
		if(len > 512)	len = 512;
		cardmeWriteEeprom(add, savebuf, len, savetype);
		add += len;
	}
	dsp_bar(3, 100);
	dsp_bar(-1, 0);

	return true;
}


int RomBK_upd(char *name) {

	FILE	*ndsFile;

	u32	add;
	int	per;
	u32	siz, len;

	dsp_bar(4, -1);

	sprintf(fname, "%s/%s", ini.dir, name);

	siz = Devicecapacity;
	if(ini.trim != 0) {
		siz = UsedROMsize;
		siz = ((siz + 511) / 512) * 512;
//		siz = ((siz + 1023) / 1024) * 1024;
	}

	if(useNewCardlib) {
		ndsFile = fopen(fname, "wb");
		if(ndsFile == NULL) {
			dsp_bar(-1, 0);
			return false;
		}
	
		dsp_bar(4, 0);
	
		// Modern card lib builds the header/secure area regions on it's own and provides them if read offsets are in the related regions expected.
		// Thus offset will be set to 0 for this read operation
		for(add = 0; add < siz; add += 0x4000) {
			for(len = 0; len < 0x4000; len += 512) {
				if(add + len >= siz)break;
				cardRead(add + len, (u32*)((u8*)(romsc2 + len)), false);
			}
			per = (int)(((u64)(add + len) * 100) / siz);
			dsp_bar(4, per);
			fwrite((char *)romsc2, len, 1, ndsFile);
		}
	
		dsp_bar(4, 100);
	
		fclose(ndsFile);
		dsp_bar(-1, 0);
	} else {
		ndsFile = fopen(fname, "wb");
		if(ndsFile == NULL) {
			dsp_bar(-1, 0);
			return false;
		}
	
		dsp_bar(4, 0);
		fwrite((char *)romhead, 512, 1, ndsFile);
	
		memset((char *)romsc2, 0x00, 0x3e00);
		add = 0x4000;
		per = (int)(((u64)add * 100) / siz);
		dsp_bar(4, per);
		fwrite((char *)romsc2, 0x3E00, 1, ndsFile);
	
		add = 0x8000;
		per = (int)(((u64)add * 100) / siz);
		dsp_bar(4, per);
		fwrite((char *)romsc1, 0x4000, 1, ndsFile);
	
		for(add = 0x8000; add < siz; add += 0x4000) {
			for(len = 0; len < 0x4000; len += 512) {
				if(add + len >= siz)	break;
				Read_Data((u8*)(romsc2 + len), add + len);
			}
			per = (int)(((u64)(add + len) * 100) / siz);
			dsp_bar(4, per);
			fwrite((char *)romsc2, len, 1, ndsFile);
		}
	
		dsp_bar(4, 100);
	
		fclose(ndsFile);
		dsp_bar(-1, 0);

//		if(FTP_FileSize(name) != (int)siz)
//			return false;
	}
	return true;
}


void RomBK_new(char *name) {

	sprintf(name, "%s_%s%02X.nds", GameTitle, Gamecode, RomVer);

	return;
}

