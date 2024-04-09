/*---------------------------------------------------------------------------------

	default ARM7 core

		Copyright (C) 2005 - 2010
		Michael Noland (joat)
		Jason Rogers (dovoto)
		Dave Murphy (WinterMute)

	This software is provided 'as-is', without any express or implied
	warranty.  In no event will the authors be held liable for any
	damages arising from the use of this software.

	Permission is granted to anyone to use this software for any
	purpose, including commercial applications, and to alter it and
	redistribute it freely, subject to the following restrictions:

	1.	The origin of this software must not be misrepresented; you
		must not claim that you wrote the original software. If you use
		this software in a product, an acknowledgment in the product
		documentation would be appreciated but is not required.

	2.	Altered source versions must be plainly marked as such, and
		must not be misrepresented as being the original software.

	3.	This notice may not be removed or altered from any source
		distribution.

---------------------------------------------------------------------------------*/
#include <nds.h>
// #include <dswifi7.h>
// #include <dswifi7.h>
// #include <maxmod7.h>

// #define IPC_CMD_GBAMODE  1
#define IPC_CMD_TURNOFF  9
// #define IPC_CMD_SR_R4TF 11
// #define IPC_CMD_SR_DLMS 12

//u32	arm9fifo;

#define IPC_CMD_KEY_TBL  0x20

extern	void	ARM7_Bios(u8 *tbl);

// volatile bool exitflag = false;

extern	u32	chip_id;

void FIFOInit() {
	REG_IPC_FIFO_CR = IPC_FIFO_ENABLE | IPC_FIFO_SEND_CLEAR;
}


void FIFO_Receive() {
	u32	fifo;
	u8	*ptr;

	while(!(REG_IPC_FIFO_CR & IPC_FIFO_RECV_EMPTY)) {
		fifo = REG_IPC_FIFO_RX;
		
		switch(fifo) 
		{
/***********
			case IPC_CMD_GBAMODE:
				gbaMode();
				while(1);
				break;
**********/
			case IPC_CMD_TURNOFF:
				systemShutDown();
				while(1)swiWaitForVBlank();
				break;
/***********
			case IPC_CMD_SR_R4TF:
				ret_menu7_R4();
				while(1);
				break;
			case IPC_CMD_SR_DLMS:
				LinkReset_ARM7();
				while(1);
				break;
***********/

			case IPC_CMD_KEY_TBL:
				while((REG_IPC_FIFO_CR & IPC_FIFO_RECV_EMPTY))swiWaitForVBlank();
				fifo = REG_IPC_FIFO_RX;
				ptr = (u8*)fifo;	
				ARM7_Bios(ptr);
//				key_table(ptr);
//				SendArm9Command(0x1111);
				break;

		}
	}
}


//---------------------------------------------------------------------------------
void VblankHandler(void) {
//---------------------------------------------------------------------------------
	// Wifi_Update();
}


//---------------------------------------------------------------------------------
void VcountHandler() {
//---------------------------------------------------------------------------------
	inputGetAndSend();
}

//---------------------------------------------------------------------------------
int main() {
//---------------------------------------------------------------------------------
	// clear sound registers
	/*dmaFillWords(0, (void*)0x04000400, 0x100);

	REG_SOUNDCNT |= SOUND_ENABLE;
	writePowerManagement(PM_CONTROL_REG, ( readPowerManagement(PM_CONTROL_REG) & ~PM_SOUND_MUTE ) | PM_SOUND_AMP );
	powerOn(POWER_SOUND);*/

	readUserSettings();
	ledBlink(0);
	
	irqInit();
	// Start the RTC tracking IRQ
	initClockIRQ();
	// fifoInit();
	FIFOInit();
	touchInit();

	// mmInstall(FIFO_MAXMOD);

	SetYtrigger(80);

	// installWifiFIFO();
	// installSoundFIFO();

	// installSystemFIFO();

	irqSet(IRQ_VCOUNT, VcountHandler);
	irqEnable(IRQ_VBLANK | IRQ_VCOUNT);
	irqSet(IRQ_FIFO_NOT_EMPTY, FIFO_Receive);
	irqEnable(IRQ_FIFO_NOT_EMPTY);
	REG_IPC_FIFO_CR = IPC_FIFO_ENABLE | IPC_FIFO_RECV_IRQ;

	// Keep the ARM7 mostly idle
	while(1)swiWaitForVBlank();
	
	return 0;
}

