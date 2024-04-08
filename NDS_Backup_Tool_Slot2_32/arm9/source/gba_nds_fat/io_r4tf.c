#include "io_r4tf.h"

#ifdef SUPPORT_R4TF

#include <nds/card.h>

void cardWaitReady(u32 flags, u8 *command)
{
	bool ready = false;

	do {
		cardWriteCommand(command);
		CARD_CR2 = flags;
		do {
			if (CARD_CR2 & CARD_DATA_READY)
				if (!CARD_DATA_RD) ready = true;
		} while (CARD_CR2 & CARD_BUSY);
	} while (!ready);
}

void bytecardPolledTransfer(uint32 flags, uint32 * destination, uint32 length, uint8 * command) {
	u32 data;;
	cardWriteCommand(command);
	CARD_CR2 = flags;
	uint32 * target = destination + length;
	do {
		// Read data if available
		if (CARD_CR2 & CARD_DATA_READY) {
			data=CARD_DATA_RD;
			if (destination < target) {
				((uint8*)destination)[0] = data & 0xff;
				((uint8*)destination)[1] = (data >> 8) & 0xff;
				((uint8*)destination)[2] = (data >> 16) & 0xff;
				((uint8*)destination)[3] = (data >> 24) & 0xff;
			}
			destination++;
		}
	} while (CARD_CR2 & CARD_BUSY);
}

void LogicCardRead(u32 address, u32 *destination, u32 length)
{
	u8 command[8];

	command[7] = 0xb9;
	command[6] = (address >> 24) & 0xff;
	command[5] = (address >> 16) & 0xff;
	command[4] = (address >> 8)  & 0xff;
	command[3] =  address        & 0xff;
	command[2] = 0;
	command[1] = 0;
	command[0] = 0;
	cardWaitReady(0xa7586000, command);
	command[7] = 0xba;
	if ((u32)destination & 0x03)
		bytecardPolledTransfer(0xa1586000, destination, length, command);
	else
		cardPolledTransfer(0xa1586000, destination, length, command);
}

u32 ReadCardInfo()
{
	u8 command[8];
	u32 ret;

	command[7] = 0xb0;
	command[6] = 0;
	command[5] = 0;
	command[4] = 0;
	command[3] = 0;
	command[2] = 0;
	command[1] = 0;
	command[0] = 0;
	cardPolledTransfer(0xa7586000, &ret, 1, command);
	return ret;
}

void LogicCardWrite(u32 address, u32 *source, u32 length)
{
	u8 command[8];
	u32 data = 0;

	command[7] = 0xbb;
	command[6] = (address >> 24) & 0xff;
	command[5] = (address >> 16) & 0xff;
	command[4] = (address >> 8)  & 0xff;
	command[3] =  address        & 0xff;
	command[2] = 0;
	command[1] = 0;
	command[0] = 0;
	cardWriteCommand(command);
	CARD_CR2 = 0xe1586000;
	u32 * target = source + length;
	do {
		// Write data if ready
		if (CARD_CR2 & CARD_DATA_READY) {
			if (source < target) {
				if ((u32)source & 0x03)
					data = ((uint8*)source)[0] | (((uint8*)source)[1] << 8) | (((uint8*)source)[2] << 16) | (((uint8*)source)[3] << 24);
				else
					data = *source;
			}
			source++;
			CARD_DATA_RD = data;
		}
	} while (CARD_CR2 & CARD_BUSY);
	command[7] = 0xbc;
	cardWaitReady(0xa7586000, command);
}

bool R4TF_StartUp(void)
{
	u32 CardInfo;

	CardInfo = ReadCardInfo();
	return ((CardInfo & 0x07) == 0x04);
}

bool R4TF_IsInserted(void)
{
	u32 CardInfo;

	CardInfo = ReadCardInfo();
	return ((CardInfo & 0x07) == 0x04);
}

bool R4TF_ReadSectors(u32 sector, u8 numSecs, void* buffer)
{
	u32 *u32_buffer = (u32*)buffer, i;

	for (i = 0; i < numSecs; i++) {
		LogicCardRead(sector << 9, u32_buffer, 128);
		sector++;
		u32_buffer += 128;
	}
	return true;
}

bool R4TF_WriteSectors(u32 sector, u8 numSecs, void* buffer)
{
	u32 *u32_buffer = (u32*)buffer, i;

	for (i = 0; i < numSecs; i++) {
		LogicCardWrite(sector << 9, u32_buffer, 128);
		sector++;
		u32_buffer += 128;
	}
	return true;
}

bool R4TF_ClearStatus(void)
{
	return true;
}

bool R4TF_Shutdown(void)
{
	return true;
}

IO_INTERFACE io_r4tf = {
	DEVICE_TYPE_R4TF,
	FEATURE_MEDIUM_CANREAD | FEATURE_MEDIUM_CANWRITE | FEATURE_SLOT_NDS,
	(FN_MEDIUM_STARTUP)&R4TF_StartUp,
	(FN_MEDIUM_ISINSERTED)&R4TF_IsInserted,
	(FN_MEDIUM_READSECTORS)&R4TF_ReadSectors,
	(FN_MEDIUM_WRITESECTORS)&R4TF_WriteSectors,
	(FN_MEDIUM_CLEARSTATUS)&R4TF_ClearStatus,
	(FN_MEDIUM_SHUTDOWN)&R4TF_Shutdown
};

LPIO_INTERFACE R4TF_GetInterface(void) {
	return &io_r4tf;
};

#endif
