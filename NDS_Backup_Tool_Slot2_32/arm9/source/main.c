/*---------------------------------------------------------------------------------
	$Id: template.c,v 1.4 2005/09/17 23:15:13 wntrmute Exp $

	Basic Hello World

	$Log: template.c,v $
	Revision 1.4  2005/09/17 23:15:13  wntrmute
	corrected iprintAt in templates
	
	Revision 1.3  2005/09/05 00:32:20  wntrmute
	removed references to IPC struct
	replaced with API functions
	
	Revision 1.2  2005/08/31 01:24:21  wntrmute
	updated for new stdio support

	Revision 1.1  2005/08/03 06:29:56  wntrmute
	added templates


---------------------------------------------------------------------------------*/
#include "nds.h"
#include <nds/arm9/console.h> //basic print funcionality
//#include <nds/registers_alt.h>
//#include "wifi.h"


#include <stdio.h>
#include <stdlib.h>
#include <string.h>
//#include <fat.h>

#include "disc_io.h"
#include "gba_nds_fat.h"
//extern LPIO_INTERFACE active_interface;

#include "command.h"
#include "sd.h"

#include "maindef.h"
#include "CardRead.h"
#include "cardme.h"

//#include "linkreset_arm9.h"
#include "unicode.h"
//#include "skin.h"
#include "message.h"

#include "tarosa/tarosa_Graphic.h"
#include "tarosa/tarosa_Shinofont.h"
extern uint16* MainScreen;
extern uint16* SubScreen;
//uint16* MainScreen = VRAM_A;
//uint16* SubScreen = (uint16*)BG_TILE_RAM_SUB(1);


char	GameTitle[13];
char	Gamecode[5];
char	RomVer;
u32	Devicecapacity;
u32	UsedROMsize;
u32	romID;

int	savetype;
u32	savesize;

int	numFiles = 0;

int	CMDmode = 0;

char	*keytbl;
char	*key2tbl;
char	*key1tbl;
char	*romhead;
char	*romsc1;
char	*romsc2;

char	tbuf[256];


#define IPC_CMD_GBAMODE  1
#define IPC_CMD_TURNOFF  9
#define IPC_CMD_SR_R4TF 11
#define IPC_CMD_SR_DLMS 12

//u32	arm9fifo;

#define IPC_CMD_KEY_TBL  0x20

void Vblank()
{
}

void FIFOInit()
{
	REG_IPC_FIFO_CR = IPC_FIFO_ENABLE | IPC_FIFO_SEND_CLEAR;
}


void FIFOSend(u32 val)
{
	REG_IPC_FIFO_TX = val;
}



u32 inp_key()
{
	u32	ky;

	while(1) {
		swiWaitForVBlank();
		scanKeys();
		ky = keysDown();
		if(ky & KEY_A)	break;
		if(ky & KEY_B)	break;
	}
	while(1) {
		swiWaitForVBlank();
		scanKeys();
		if(keysHeld() != ky)	break;
	}
	return(ky);
}


extern	void	ret_menu9_R4(void);

void turn_off(int cmd)
{

//	FIFOInit();

//	if(cmd == 0)			// 電源断
		FIFOSend(IPC_CMD_TURNOFF);

/*****
	if(cmd == 1) {			// R4 Soft Reset
		FIFOSend(IPC_CMD_SR_R4TF);
		REG_IME = 0;
		REG_IE = 0;
		REG_IF = REG_IF;

		WAIT_CR = 0xE880;
		REG_IPC_SYNC = 0;
		DMA0_CR = 0;
		DMA1_CR = 0;
		DMA2_CR = 0;

		ret_menu9_R4();
	}
	if(cmd == 2) {			// DSLink Soft Reset
		FIFOSend(IPC_CMD_SR_DLMS);
		LinkReset_ARM9();
	}
****/
	while(1);
}



void err_cnf(int n1, int n2)
{
	int	len;
	int	x1, x2;
	int	y1, y2;
	int	xi, yi;
	u16	*gback;
	int	gsiz;

	len = strlen(errmsg[n1]);
	if(len < strlen(errmsg[n2]))
		len = strlen(errmsg[n2]);
	if(len < 10)	len = 10;

	x1 = (256 - len * 6) / 2 - 4;
	y1 = 4*12-6;
	x2 = x1 + len * 6 + 9;
	y2 = 8*12+3;

	gsiz = (x2-x1+1) * (y2-y1+1);
	gback = (u16*)malloc(sizeof(u16*) * gsiz);
	for( yi=y1; yi<y2+1; yi++ ){
		for( xi=x1; xi<x2+1; xi++ ){
			gback[(xi-x1)+(yi-y1)*(x2+1-x1)] = Point_SUB( SubScreen, xi, yi );
		}
	}

	DrawBox_SUB( SubScreen, x1, y1, x2, y2, 6, 0);
	DrawBox_SUB( SubScreen, x1+1, y1+1, x2-1, y2-1, 2, 1);
	DrawBox_SUB( SubScreen, x1+2, y1+2, x2-2, y2-2, 6, 0);

	ShinoPrint_SUB(SubScreen, x1 + 6, y1 + 6, (u8 *)errmsg[n1], 0, 0, 0);
	ShinoPrint_SUB(SubScreen, x1 + 6, y1 + 20, (u8 *)errmsg[n2], 0, 0, 0);
	ShinoPrint_SUB(SubScreen, x1 + (len/2)*6 - 18, y1 + 37, (u8*)errmsg[0], 0, 0, 0);


	while(!(inp_key() & KEY_A));

	for( yi=y1; yi<y2+1; yi++ ){
		for( xi=x1; xi<x2+1; xi++ ){
			Pixel_SUB(SubScreen, xi, yi, gback[(xi-x1) + (yi-y1)*(x2+1-x1)] );
		}
	}
	free(gback);

//	turn_off(0);

}


int cnf_inp(int mode, int n1, int n2)
{
	int	len;
	int	x1, x2;
	int	y1, y2;
	int	xi, yi;
	u16	*gback;
	int	gsiz;
	u32	ky;
	char	*msg1;

	if(n1 < 0)
		msg1 = tbuf;
	else	msg1 = cnfmsg[n1];
	len = strlen(msg1);
	if(len < strlen(cnfmsg[n2]))
		len = strlen(cnfmsg[n2]);
	if(len < 20)	len = 20;

	x1 = (256 - len * 6) / 2 - 4;
	y1 = 4*12-6;
	x2 = x1 + len * 6 + 9;
	y2 = 8*12+3;

	gsiz = (x2-x1+1) * (y2-y1+1);
	gback = (u16*)malloc(sizeof(u16*) * gsiz);
	for( yi=y1; yi<y2+1; yi++ ){
		for( xi=x1; xi<x2+1; xi++ ){
			gback[(xi-x1)+(yi-y1)*(x2+1-x1)] = Point_SUB( SubScreen, xi, yi );
		}
	}

	DrawBox_SUB( SubScreen, x1, y1, x2, y2, 6, 0);
	DrawBox_SUB( SubScreen, x1+1, y1+1, x2-1, y2-1, 5, 1);
	DrawBox_SUB( SubScreen, x1+2, y1+2, x2-2, y2-2, 6, 0);

	ShinoPrint_SUB(SubScreen, x1 + 6, y1 + 6, (u8 *)msg1, 0, 0, 0);
	ShinoPrint_SUB(SubScreen, x1 + 6, y1 + 20, (u8 *)cnfmsg[n2], 0, 0, 0);
	ShinoPrint_SUB(SubScreen, x1 + (len/2)*6 - 50, y1 + 37, (u8*)cnfmsg[mode], 0, 0, 0);


	ky = inp_key();

	for( yi=y1; yi<y2+1; yi++ ){
		for( xi=x1; xi<x2+1; xi++ ){
			Pixel_SUB(SubScreen, xi, yi, gback[(xi-x1) + (yi-y1)*(x2+1-x1)] );
		}
	}
	free(gback);
	return(ky);
}


u16	*gbar = NULL;
int	oldper;

void dsp_bar(int mod, int per)
{
	int	x1, x2;
	int	y1, y2;
	int	xi, yi;
	int	gsiz;

	x1 = 49;
	y1 = 142;	//70;
	x2 = 205;
	y2 = 187;	//115;

	if(per < 0) {
		gsiz = (x2-x1+1) * (y2-y1+1);
		gbar = (u16*)malloc(sizeof(u16*) * gsiz);
		for( yi=y1; yi<y2+1; yi++ ){
			for( xi=x1; xi<x2+1; xi++ ){
				gbar[(xi-x1)+(yi-y1)*(x2+1-x1)] = Point(MainScreen, xi, yi );
			}
		}

		DrawBox(MainScreen, x1, y1, x2, y2, RGB15(31,31,0), 0);
		DrawBox(MainScreen, x1+1, y1+1, x2-1, y2-1, RGB15(0,0,31), 1);
		DrawBox(MainScreen, x1+2, y1+2, x2-2, y2-2, RGB15(31,31,0), 0);

		if(per != -2)
			DrawBox(MainScreen, x1 + 28, y1 + 20, x1 + 129, y1 + 40, RGB15(31,31,31), 0);
		ShinoPrint(MainScreen, x1 + 26, y1 + 6, (u8 *)barmsg[mod], RGB15(31,31,31), RGB15(31,31,31), 0);
		oldper = -1;
		return;
	}

	if(gbar == NULL)	return;

	if(per != oldper) {
		oldper = per;
		if(per > 0)
			DrawBox(MainScreen, x1 + 29, y1 + 21, x1 + 28 + per, y1 + 39, RGB15(30,0,0), 1);
		if(per < 100)
			DrawBox(MainScreen, x1 + 28 + per + 1, y1 + 21, x1 + 128, y1 + 39, RGB15(0,30,0), 1);
		sprintf(tbuf, "%2d%%", per);
		ShinoPrint(MainScreen, x1 + 74, y1 + 24, (u8 *)tbuf, RGB15(31,31,31), RGB15(31,31,31), 0);
	}

	if(mod == -1) {
		for( yi=y1; yi<y2+1; yi++ ){
			for( xi=x1; xi<x2+1; xi++ ){
				Pixel(MainScreen, xi, yi, gbar[(xi-x1) + (yi-y1)*(x2+1-x1)] );
			}
		}
		free(gbar);
		gbar = NULL;
	}
	return;
}


int	r4tf;


void _dsp_clear()
{
//	DrawBox_SUB(SubScreen, 0, 28, 255, 114, 0, 1);
	DrawBox_SUB(SubScreen, 0, 28, 255, 114, 0, 1);

}

#define	FILELINE	10
#define	FILEY		6

void _ftp_dsp(int no, int mod, int x, int y)
{
//	int	i;
	char	dsp[40];

	Unicode2Local(fs[no].uniname, (u8*)dsp, 31);
/******
	for(i = 0; i < 31; i++)
		dsp[i] = fs[no].filename[i];

	if((dsp[i - 1] >= 0x81 && dsp[i - 1] <= 0x9F) || (dsp[i - 1] >= 0xE0 && dsp[i - 1] <= 0xEF))
		dsp[i - 1] = 0;
	dsp[i] = 0;
*********/

	if(fs[no].filesize < 1000*1000)
		sprintf(tbuf, " %-31s %6.2f KB", dsp, (float)fs[no].filesize / 1024);
	else	sprintf(tbuf, " %-31s %6.2f MB", dsp, (float)fs[no].filesize / (1024*1024));

	if(mod == 1) {
		ShinoPrint( MainScreen, x*6, y*12, (u8 *)tbuf, RGB15(31,0,0), RGB15(0,0,31), 1);
	} else	ShinoPrint( MainScreen, x*6, y*12, (u8 *)tbuf, RGB15(0,0,0), RGB15(31,31,31), 1);
}


void _ftp_sel_dsp(int no, int yc, int mod)
{
	int	x, y;
	int	st, i;
	u16	pl, pls;
	y = FILEY;
	x = 0;
	char	dsp[40];

	if(mod == 0) {
//		_dsp_clear();

		pl = pls = 1;
		if(numFiles == 0)	pl = 7;
		if(savetype < 1)
			pl = pls = 7;
		if(CMDmode == 0) {
			ShinoPrint_SUB( SubScreen, 15*6, 10*12, (u8 *)" Save Backup ", 0, 5, 1);
			ShinoPrint_SUB( SubScreen, 2*6, 11*12+6, (u8 *)t_msg[4], pl, 0, 1);
			ShinoPrint_SUB( SubScreen, 2*6, 12*12+6, (u8 *)t_msg[7], pls, 0, 1);
			ShinoPrint_SUB( SubScreen, 2*6, 13*12+6, (u8 *)t_msg[10], 1, 0, 1);
			ShinoPrint_SUB( SubScreen, 2*6, 14*12+6, (u8 *)t_msg[11], 1, 0, 1);
		}
		if(CMDmode == 1) {
			ShinoPrint_SUB( SubScreen, 15*6, 10*12, (u8 *)" Save Restore", 0, 5, 1);
			ShinoPrint_SUB( SubScreen, 2*6, 11*12+6, (u8 *)t_msg[5], pl, 0, 1);
			ShinoPrint_SUB( SubScreen, 2*6, 12*12+6, (u8 *)t_msg[8], 1, 0, 1);
			ShinoPrint_SUB( SubScreen, 2*6, 13*12+6, (u8 *)t_msg[10], 1, 0, 1);
			ShinoPrint_SUB( SubScreen, 2*6, 14*12+6, (u8 *)t_msg[12], 1, 0, 1);
		}
		if(CMDmode == 2) {
			ShinoPrint_SUB( SubScreen, 15*6, 10*12, (u8 *)"  Rom Backup ", 0, 5, 1);
			ShinoPrint_SUB( SubScreen, 2*6, 11*12+6, (u8 *)t_msg[6], pl, 0, 1);
			ShinoPrint_SUB( SubScreen, 2*6, 12*12+6, (u8 *)t_msg[9], 1, 0, 1);
			ShinoPrint_SUB( SubScreen, 2*6, 13*12+6, (u8 *)t_msg[10], 1, 0, 1);
			ShinoPrint_SUB( SubScreen, 2*6, 14*12+6, (u8 *)t_msg[13], 1, 0, 1);
		}

		DrawBox(MainScreen, 0, (FILEY-1)*12-4, 255, FILEY*12-1, RGB15(0,0,0), 1);
		DrawBox(MainScreen, 0, FILEY*12, 255, 191, RGB15(31,31,31), 1);

		strncpy(dsp, ini.dir, 32);
		dsp[32] = 0;
		if(CMDmode == 2)
			sprintf(tbuf, "[%s/] %d NDS", dsp, numFiles);
		else	sprintf(tbuf, "[%s/] %d SAV", dsp, numFiles);
		ShinoPrint(MainScreen, 0, (FILEY-1)*12-2, (u8 *)tbuf, RGB15(31,31,31), RGB15(0,0,0), 0);

	}


	st = no - yc;
	for(i = 0; i < FILELINE; i++) {
		if(i + st < numFiles) {
			if(i == yc)
				_ftp_dsp(i + st, 1, x, y + i);
			else	_ftp_dsp(i + st, 0, x, y + i);
		}
	}
}



bool set_rom()
{
//	int	len;
	int	i;
	u32	gc;

/*****
	sprintf(tbuf, "Ky2 : %02X %02X %02X %02X %02X %02X", key2tbl[0], key2tbl[1], key2tbl[2], key2tbl[3], key2tbl[4], key2tbl[5]);
	ShinoPrint_SUB( SubScreen, 5*6, 12*12, (u8 *)tbuf, 1, 0, 0);
	sprintf(tbuf, "Ky1 : %02X %02X %02X %02X %02X %02X", key1tbl[0], key1tbl[1], key1tbl[2], key1tbl[3], key1tbl[4], key1tbl[5]);
	ShinoPrint_SUB( SubScreen, 5*6, 13*12, (u8 *)tbuf, 1, 0, 0);
	sprintf(tbuf, "WAIT_CR(LANG) : %04X(%d)", WAIT_CR, PersonalData->language);
	ShinoPrint_SUB( SubScreen, 5*6, 14*12, (u8 *)tbuf, 1, 0, 0);
***/

	if(cnf_inp(0, 2, 3) & KEY_B)
		return false;

	romID = Rom_Read(0, (u8*)romhead, (u8*)romsc1);
	while(romID == 0xFFFFFFFF) {
		if(cnf_inp(0, 7, 8) & KEY_B)
			return false;

		romID = Rom_Read(1, (u8*)romhead, (u8*)romsc1);
	}


	for(i = 0; i < 12; i++) {
		if((romhead[i] >= 0x30 && romhead[i] <= 0x39) || (romhead[i] >= 0x41 && romhead[i] <= 0x5A))
			GameTitle[i] = romhead[i];
		else	GameTitle[i] = '_';
	}
	GameTitle[i] = 0;
	for(i = 11; i >=0; i--) {
		if(GameTitle[i] != '_')
			break;
	}
	if(i != 0)	GameTitle[i+1] = 0;


	for(i = 0; i < 4; i++) {
		if((romhead[i+0x0C] >= 0x30 && romhead[i+0x0C] <= 0x39) || (romhead[i+0x0C] >= 0x41 && romhead[i+0x0C] <= 0x5A))
			Gamecode[i] = romhead[i+0x0C];
		else	Gamecode[i] = '_';
	}
	Gamecode[i] = 0;

	RomVer = romhead[0x1E];

	Devicecapacity = (128 << romhead[0x14]) * 1024;
	UsedROMsize = *((u32*)(romhead + 0x80));

	savetype = cardmeGetType();
	if(savetype > 0) {
		savesize = cardmeSize(savetype);
		if(savesize == 0)	savetype = 0;
	} else	savesize = 0;


////////////////////////////////////////
gc = *((u32*)(romhead + 0x0C)) & 0x00FFFFFF;
if(savetype == 3) {				// FLASH
	if(gc == 0x455A41) {		// ZELDA_DS FLASH 4M
		savesize = 512 * 1024;
	}
	if(gc == 0x414441) {		// Pokemon_D FLASH 4M
		savesize = 512 * 1024;
	}
	if(gc == 0x415041) {		// Pokemon_P FLASH 4M
		savesize = 512 * 1024;
	}
//	if(gc == 0x593241) {		// MANIMANE FLASH 4M
//		savesize = 512 * 1024;
//	}
}

/*****************
if(savetype == 0) {				// Unknown
	if(gc == 0x425841) {		// BANDBROS DX 64M
		savesize = 8192 * 1024;
		savetype = 3;
	}
}
******************/
///////////////////////////////////////

	DrawBox_SUB(SubScreen, 6, 32, 249, 76+36, 5, 0);
	DrawBox_SUB(SubScreen, 8, 34, 247, 74+36, 5, 0);
	DrawBox_SUB(SubScreen, 9, 4*12, 246, 9*12-1, 0, 1);


	ShinoPrint_SUB( SubScreen, 2*6, 3*12, (u8 *)"< ROM Information >", 1, 0, 0);

	sprintf(tbuf, "Game Title : %s %s %02X", GameTitle, Gamecode, RomVer);
	ShinoPrint_SUB( SubScreen, 5*6, 4*12, (u8 *)tbuf, 1, 0, 0);

	sprintf(tbuf, "Chip ID : %02X %02X %02X %02X", romID & 0xFF, (romID >> 8) & 0xFF, (romID >> 16) & 0xFF, (romID >> 24) & 0xFF);
	ShinoPrint_SUB( SubScreen, 5*6, 5*12, (u8 *)tbuf, 1, 0, 0);

	sprintf(tbuf, "ROM Size(Used) : %6.2fMB(%6.2fMB)", (float)Devicecapacity / (1024*1024), (float)UsedROMsize / (1024*1024));
	ShinoPrint_SUB( SubScreen, 5*6, 6*12, (u8 *)tbuf, 1, 0, 0);

	if(savetype == -1)
		sprintf(tbuf, "SAVE Type : 0 (Not provide)");
	if(savetype == 0)
		sprintf(tbuf, "SAVE Type : Unknown");
	if(savetype == 1)
		sprintf(tbuf, "SAVE Type : EEPROM %dK(%dByte)", savesize / (1024/8), savesize);
	if(savetype == 2)
		sprintf(tbuf, "SAVE Type : EEPROM %dK(%dKByte)", savesize / (1024/8), savesize / 1024);
	if(savetype == 3)
		sprintf(tbuf, "SAVE Type : FLASH %dM(%dKByte)", savesize / (1024*1024/8), savesize / 1024);
	ShinoPrint_SUB( SubScreen, 5*6, 7*12+6, (u8 *)tbuf, 1, 0, 0);

	return true;
}



bool _ftp_sel_sub1(int *yc, int *sel)
{
	int	x, y;
	u32	repky;

	y = FILEY;
	x = 0;

	repky = keysDownRepeat();
	if((repky & KEY_UP) || (repky & KEY_DOWN)) {
		if(repky & KEY_UP) {
			if(*sel > 0) {
				if(*yc == 0) {
					(*sel)--;
					_ftp_sel_dsp(*sel, *yc, 1);
				} else {
					_ftp_dsp(*sel, 0, x, y+(*yc));
					(*yc)--;
					(*sel)--;
					_ftp_dsp(*sel, 1, x, y+(*yc));
				}
			}
		}
		if(repky & KEY_DOWN) {
			if(*sel < numFiles - 1) {
				if(*yc == FILELINE -1) {
					(*sel)++;
					_ftp_sel_dsp(*sel, *yc, 1);
				} else {
					_ftp_dsp(*sel, 0, x, y+(*yc));
					(*yc)++;
					(*sel)++;
					_ftp_dsp(*sel, 1, x, y+(*yc));
				}
			}
		}
		return true;
	}
	return false;
}

void _ftp_sel_sub2(u32 ky, int *yc, int *sel)
{
	int	x, y;
	int	st0, st1;

	y = FILEY;
	x = 0;

	if(ky & KEY_LEFT) {
		if(*sel > 0) {
			st0 = (*sel) - (*yc);
			st1 = st0 - FILELINE;
			if(st1 < 0)	st1 = 0;
			if(st1 == st0) {
				_ftp_dsp(*sel, 0, x, y+(*yc));
				*yc = 0;
				*sel = 0;
				_ftp_dsp(*sel, 1, x, y+(*yc));
			} else {
				*sel = st1 + (*yc);
				_ftp_sel_dsp(*sel, *yc, 1);
			}
		}
	}
	if(ky & KEY_RIGHT) {
		if(*sel < numFiles -1) {
			st0 = (*sel) - (*yc);
			st1 = st0 + FILELINE;
			if(st1 >= numFiles - FILELINE) {
				st1 = numFiles - FILELINE;
				if(st1 < 0)	st1 = 0;
			}
			if(st1 == st0) {
				_ftp_dsp(*sel, 0, x, y+(*yc));
				*yc = FILELINE - 1;
				if(*yc >= numFiles)
					*yc = numFiles - 1;
				*sel = st1 + (*yc);
				_ftp_dsp(*sel, 1, x, y+(*yc));
			} else {
				*sel = st1 + (*yc);
				_ftp_sel_dsp(*sel, *yc, 1);
			}
		}
	}
}


int ftp_sel()
{
	char	name[30];
	int	cmd = 0;
	int	sel;
	u32	ky;
	int	yc;
	int	x, y;

	y = FILEY;
	x = 0;
	sel = 0;
	yc = 0;

	_ftp_sel_dsp(sel, yc, 0);

	if(numFiles == 0) {
		ShinoPrint(MainScreen, 42, 90,  (u8 *)t_msg[0], RGB15(31,0,31), RGB15(0,0,31), 1);
		ShinoPrint(MainScreen, 42, 102, (u8 *)t_msg[1], RGB15(31,0,31), RGB15(0,0,31), 1);
		ShinoPrint(MainScreen, 42, 114, (u8 *)t_msg[0], RGB15(31,0,31), RGB15(0,0,31), 1);
	}

	while(1) {
		swiWaitForVBlank();
		scanKeys();
		if(numFiles > 0) {
			if(_ftp_sel_sub1(&yc, &sel))	continue;
		}

		ky = keysDown();

		if(numFiles > 0) {
			_ftp_sel_sub2(ky, &yc, &sel);
		}

		if(ky & KEY_L) {
			if(CMDmode > 0) {
				CMDmode--;
				cmd = 0;
				break;
			}
		}
		if(ky & KEY_R) {
			if(CMDmode < 2) {
				CMDmode++;
				cmd = 0;
				break;
			}
		}


		if(ky & KEY_START) {
			cmd = -1;
			break;
		}

		if(ky & KEY_X) {
			if(set_rom() == false) {
				cmd = -1;
				break;
			}
			break;
//			_ftp_sel_dsp(sel, yc, 0);
		}
//		if(ky & KEY_Y) {
//			cmd = 1;
//			break;
//		}

		if((numFiles > 0) && (ky & KEY_A)) {
			if((CMDmode == 0) && (savetype > 0)) {
				Unicode2Local(fs[sel].uniname, (u8*)name, 24);
				sprintf(tbuf, "File Name : %s", name);
				if(cnf_inp(1, -1, 5) & KEY_A) {
					if(!SaveBK_upd(fs[sel].Alias))
						err_cnf(7, 8);
					cmd = 1;
				}
				break;
			}
			if((CMDmode == 1) && (savetype > 0)) {
				if(fs[sel].filesize < savesize) {
					err_cnf(9, 10);
				} else {
					Unicode2Local(fs[sel].uniname, (u8*)name, 24);
					sprintf(tbuf, "File Name : %s", name);
					if(cnf_inp(1, -1, 6) & KEY_A) {
						if(!Save_Rest(fs[sel].Alias))
							err_cnf(7, 8);
					}
				}
			}
			if(CMDmode == 2) {
				Unicode2Local(fs[sel].uniname, (u8*)name, 24);
				sprintf(tbuf, "File Name : %s", name);
				if(cnf_inp(1, -1, 5) & KEY_A) {
					if(!RomBK_upd(fs[sel].Alias))
						err_cnf(7, 8);
					cmd = 1;
				}
				break;
			}
		}

		if(ky & KEY_B) {
			if((CMDmode == 0) && (savetype > 0)) {
				SaveBK_new(name);
				sprintf(tbuf, "File Name : %s", name);
				if(cnf_inp(1, -1, 4) & KEY_A) {
					if(!SaveBK_upd(name))
						err_cnf(7, 8);
					cmd = 1;
					break;
				}
			}
			if((CMDmode == 1) && (savetype > 0)) {
				if(cnf_inp(1, 9, 10) & KEY_A) {
					Save_Init();
				}
			}
			if(CMDmode == 2) {
				RomBK_new(name);
				sprintf(tbuf, "File Name : %s", name);
				if(cnf_inp(1, -1, 4) & KEY_A) {
					if(!RomBK_upd(name))
						err_cnf(7, 8);
					cmd = 1;
					break;
				}
			}
		}

	}

	return(cmd);
}


void sts_dsp(int n1)
{
	int	len;
	int	x1, x2;
	int	y1, y2;

	len = 32;

	x1 = (256 - len * 6) / 2 - 4;
	y1 = 5*12-6;
	x2 = x1 + len * 6 + 9;
	y2 = 7*12+3;

	if(n1 < 0) {
		DrawBox_SUB( SubScreen, x1, y1, x2, y2, 0, 1);
		return;
	}

	DrawBox_SUB( SubScreen, x1, y1, x2, y2, 6, 0);
	DrawBox_SUB( SubScreen, x1+1, y1+1, x2-1, y2-1, 5, 1);
	DrawBox_SUB( SubScreen, x1+2, y1+2, x2-2, y2-2, 6, 0);

	len = strlen(stsmsg[n1]);
	ShinoPrint_SUB(SubScreen, 128 - (len/2)*6, y1 + 12, (u8 *)stsmsg[n1], 0, 0, 0);
}



extern u32	_io_dldi;


void dsp_main()
{
	char	ct[5];

	DrawBox(MainScreen, 2, 2, 253, 53, RGB15(0,0,0), 0);
	DrawBox(MainScreen, 3, 3, 252, 52, RGB15(0,0,31), 1);
	DrawBox(MainScreen, 4, 4, 251, 51, RGB15(31,31,31), 0);

	ct[0] = _io_dldi & 0xFF;
	ct[1] = (_io_dldi >> 8) & 0xFF;
	ct[2] = (_io_dldi >> 16) & 0xFF;
	ct[3] = (_io_dldi >> 24) & 0xFF;
	ct[4] = 0;
	sprintf(tbuf, "Slot-2 Cartridge : [ %s ]", ct);
	ShinoPrint(MainScreen, 11+24, 8, (u8 *)tbuf, RGB15(31,31,31), RGB15(31,31,31), 0);
//	sprintf(tbuf, "FTP Server IP : %d.%d.%d.%d  Port %d", ini.sv_ip[0], ini.sv_ip[1], ini.sv_ip[2], ini.sv_ip[3], ini.port);
//	ShinoPrint(MainScreen, 11, 8, (u8 *)tbuf, RGB15(31,31,31), RGB15(31,31,31), 0);
//	sprintf(tbuf, "    User Name : %s", ini.user);
//	ShinoPrint(MainScreen, 11, 20, (u8 *)tbuf, RGB15(31,31,31), RGB15(31,31,31), 0);

	if(ini.save == 0)
		sprintf(tbuf, "Save File Size : Auto");
	else	sprintf(tbuf, "Save File Size : %dKB", ini.save);
	ShinoPrint(MainScreen, 60, 24, (u8 *)tbuf, RGB15(31,31,31), RGB15(31,31,31), 0);

	if(ini.trim != 0)
		sprintf(tbuf, "Rom Backup Type: Trim ");
	else	sprintf(tbuf, "Rom Backup Type: Not Trim");
	ShinoPrint(MainScreen, 60, 36, (u8 *)tbuf, RGB15(31,31,31), RGB15(31,31,31), 0);
//	ShinoPrint(MainScreen, 60, 48, (u8 *)tbuf, RGB15(16,16,16), RGB15(31,31,31), 0);


}


//extern	char	*ftpbuf;

extern	void	setLang(void);

void mainloop(void)
{
//	FAT_FILE	*r4dt;
	int	cmd;
	int	fl;

	TIMER0_DATA = 0xFF7D;
	TIMER0_CR = TIMER_ENABLE | TIMER_DIV_256;
	TIMER1_DATA = 0;
	TIMER1_CR = TIMER_ENABLE | TIMER_CASCADE | TIMER_DIV_1;

	keytbl = (char *)malloc(0x1200);
	key2tbl = keytbl + 0x2A;
	key1tbl = keytbl + 0x30;
	romhead = (char *)malloc(0x200);
	romsc1 = (char *)malloc(0x4000);
	romsc2 = (char *)malloc(0x4000);


	keysSetRepeat(20, 6);		// def. 60, 30 (delay, repeat)

	DrawBox_SUB(SubScreen, 20, 3, 235, 27, 1, 0);
	DrawBox_SUB(SubScreen, 21, 4, 234, 26, 5, 1);
	DrawBox_SUB(SubScreen, 22, 5, 233, 25, 0, 0);
	ShinoPrint_SUB( SubScreen, 9*6, 1*12-2, (u8*)"NDS Backup Tool (Slot2)", 0, 0, 0 );
	ShinoPrint_SUB( SubScreen, 33*6, 12, (u8 *)"v0.32", 0, 0, 0 );

	r4tf = 0;
//	if(_io_dldi == 0x46543452) {		// R4TF
//		if((*(vu32*)0x027FFE18) != 0x00000000)
//			r4tf = 1;
//	}
//	if(_io_dldi == 0x534D4C44)		// DLMS
//		r4tf = 2;


	setLangMsg();

	sts_dsp(0);
	if(FAT_InitFiles() == false) {
		_io_dldi = 0;
		r4tf = 0;
		err_cnf(1, 2);
		turn_off(r4tf);
	}


//	if((_io_dldi == 0x46543452) && (r4tf == 0)) {		// R4TF
//		r4dt = FAT_fopen("/_DS_MENU.DAT", "rb");
//		(*(vu32*)0x027FFE18) = r4dt->dirEntSector*512+r4dt->dirEntOffset*32;
//		FAT_fclose(r4dt);
//		r4tf = 1;
//	}

/**************
	sts_dsp(1);
	initWifi();

	sts_dsp(2);
	if(connectWifi() != true) {
		sts_dsp(-1);
		err_cnf(3, 4);

		free(key2tbl);
		free(romhead);
		free(romsc1);
		free(romsc2);
		turn_off(r4tf);
	}

	sts_dsp(3);
***/
//	WAIT_CR |= 0x8800;

//	for(vi = 0; vi < 20; vi++)
//		swiWaitForVBlank();

	FIFOSend(IPC_CMD_KEY_TBL);
	FIFOSend((u32)keytbl);

/********
	while(1) {
		while(REG_IPC_FIFO_CR & IPC_FIFO_RECV_EMPTY)
			swiWaitForVBlank();
		if(REG_IPC_FIFO_RX == 0x1111)	break;
	}
****/
//	WAIT_CR &= 0x77FF;

//	FTP_ini();
	SD_ini();
	dsp_main();

/*********
	if(FTP_Login() == false) {
		sts_dsp(-1);
//	ftpbuf[40]=0;
//	ShinoPrint_SUB( SubScreen, 2*6, 12*12, (u8 *)ftpbuf, 1, 0, 0);
		err_cnf(5, 6);
		disconnectWifi();

		free(key2tbl);
		free(romhead);
		free(romsc1);
		free(romsc2);
		turn_off(r4tf);
	}
******/
	sts_dsp(-1);


//	WAIT_CR &= 0x77FF;

	if(set_rom() == false) {
//		FTP_Logoff();
//		disconnectWifi();

		free(keytbl);
		free(romhead);
		free(romsc1);
		free(romsc2);
		turn_off(0);
	}


	DrawBox_SUB(SubScreen, 6, 125, 249, 190, 5, 0);
	DrawBox_SUB(SubScreen, 8, 127, 247, 188, 5, 0);

	DrawBox_SUB(SubScreen, 75, 115, 181, 136, 1, 0);
	DrawBox_SUB(SubScreen, 76, 116, 180, 135, 5, 1);
	DrawBox_SUB(SubScreen, 77, 117, 179, 134, 0, 0);


	CMDmode = 0;
	cmd = 0;
	fl = -1;
	while(cmd != -1) {
		if(fl != (CMDmode & 2)) {
	DrawBox(MainScreen, 0, FILEY*12, 255, 191, RGB15(31,31,31), 1);
	ShinoPrint(MainScreen, 42, 90,  (u8 *)t_msg[2], RGB15(31,31,31), RGB15(0,0,31), 1);
	ShinoPrint(MainScreen, 42, 102,  (u8 *)t_msg[3], RGB15(31,31,31), RGB15(0,0,31), 1);
	ShinoPrint(MainScreen, 42, 114, (u8 *)t_msg[2], RGB15(31,31,31), RGB15(0,0,31), 1);
			if(CMDmode == 2) {
				numFiles = SD_FileList(1);
				fl = 2;
			} else {
				numFiles = SD_FileList(0);
				fl = 0;
			}
		}

		cmd = ftp_sel();
		if(cmd == 1)	fl = -1;
	}

//	FTP_Logoff();

//	disconnectWifi();


	free(keytbl);
	free(romhead);
	free(romsc1);
	free(romsc2);

	turn_off(0);

}


//---------------------------------------------------------------------------------
int main(void) {
//---------------------------------------------------------------------------------

	int	i;

  vramSetMainBanks(VRAM_A_LCD ,  VRAM_B_LCD  , VRAM_C_SUB_BG, VRAM_D_MAIN_BG  );
	powerON(POWER_ALL);

	irqInit();
	irqSet(IRQ_VBLANK, Vblank);
	irqEnable(IRQ_VBLANK);
	FIFOInit();

 videoSetMode(MODE_FB0 | DISPLAY_BG2_ACTIVE);
 videoSetModeSub(MODE_0_2D | DISPLAY_BG0_ACTIVE );
 SUB_BG0_CR = BG_256_COLOR | BG_MAP_BASE(0) | BG_TILE_BASE(1);
 uint16* map1 = (uint16*)BG_MAP_RAM_SUB(0);
 for(i=0;i<(256*192/8/8);i++)	map1[i]=i;
 lcdMainOnTop();
	ClearBG( MainScreen, RGB15(31,31,31) );
//	ClearBG( MainScreen, RGB15(0,0,0) );

	//サブ画面の表示
	BG_PALETTE_SUB[0] = RGB15(31,31,31);		//(白)サブ画面のバックカラー
	BG_PALETTE_SUB[1] = RGB15(0,0,0);			//(黒)サブ画面のフォアカラー
	BG_PALETTE_SUB[2] = RGB15(30,0,0);			//(赤)
	BG_PALETTE_SUB[3] = RGB15(0,30,0);			//(緑)
	BG_PALETTE_SUB[4] = RGB15(0,31,31);			//(水色)
	BG_PALETTE_SUB[5] = RGB15(0,0,31);			//(青)
	BG_PALETTE_SUB[6] = RGB15(31,31,0);			//(黄)
	BG_PALETTE_SUB[7] = RGB15(24,24,24);			//(灰)

	ClearBG_SUB( SubScreen, 0 );				//バックを白に



//	videoSetMode(0);	//not using the main screen
//	videoSetModeSub(MODE_0_2D | DISPLAY_BG0_ACTIVE);	//sub bg 0 will be used to print text
//	vramSetBankC(VRAM_C_SUB_BG);

//	SUB_BG0_CR = BG_MAP_BASE(31);

//	BG_PALETTE_SUB[255] = RGB15(31,31,31);	//by default font will be rendered with color 255

	//consoleInit() is a lot more flexible but this gets you up and running quick
//	consoleInitDefault((u16*)SCREEN_BASE_BLOCK_SUB(31), (u16*)CHAR_BASE_BLOCK_SUB(0), 16);
	swiWaitForVBlank();
	swiWaitForVBlank();
	sysSetBusOwners(BUS_OWNER_ARM9,BUS_OWNER_ARM9);


//	iprintf("%s\n%s\n%s\n\n", ROMTITLE, ROMVERSION, ROMDATE);

	mainloop();

	return 0;
}


