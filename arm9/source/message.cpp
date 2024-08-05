
#include <stdio.h>

#include <NDS.h>

#include "message.h"

char	*errmsg[14];
char	*cnfmsg[15];
char	*barmsg[5];
char	*t_msg[20];
char	*stsmsg[4];

const	char*	errmsg_j[14] = {
			"(A):�m�F",					// 0
			"FAT�̏������Ɏ��s���܂���",			// 1
			"�K�؂�DLDI�p�b�`���s���Ă�������",		// 2
			"",		// 3
			"",			// 4
			"",			// 5
			"ini�t�@�C���̐ݒ���m�F���Ă�������",		// 6
			"�t�@�C���̓]�������s���܂���",			// 7
			"�ēx�A��蒼���Ă�������",			// 8
			"�w�肵��SAV�t�@�C���́AROM��SAVE�̈�",		// 9
			"��菬�����t�@�C���ł�",			// 10
			"",		// 11
			"",		// 12
			"",		// 13
};

static const	char*	errmsg_e[14] = {
			"(A):Confirm",					// 0
			"FAT initialization failed.",			// 1
			"Please apply the appropriate DLDI Patch.",	// 2
			"",	// 3
			"",		// 4
			"",		// 5
			"Confirm the setting of the ini file.",		// 6
			"Data transfer failed.",			// 7
			"Please do over again.",			// 8
			"The specified SAV file is a file that is",	// 9
			"smaller than the SAVE area of ROM.",		// 10
			"",		// 11
			"",		// 12
			"",		// 13
};


static const	char*	cnfmsg_j[15] = {
			"(A):�n�j, (B):�I��",				// 0
			"(A):���s, (B):���",				// 1
			"Slot-1���猻�݂̃J�[�h�𔲂���",		// 2
			"�ړI�̃J�[�h���Z�b�g���Ă�������",		// 3
			"���̃t�@�C�����Ńt�@�C�����쐬���܂�",		// 4
			"���̃t�@�C�����Ńt�@�C�����㏑�����܂�",	// 5
			"���̃t�@�C�����̃t�@�C����Ǎ��݂܂�",		// 6
			"Slot-1�̃J�[�h���F���ł��܂���",		// 7
			"��x�����Ă���ēx�Z�b�g���Ă�������",		// 8
			"Slot-1��ROM��SAVE�̈�����������܂�",		// 9
			"DSi Enhanced �J�[�h�����o����܂����B",		// 10
			"���o���čēx�}�����Ċ������Ă��������B",		// 11
			"TWL �J�[�h�͏]���� CardLib �ł̓T�|�[�g����Ă��܂���B",		// 12
			"",								// 13
			"(A):�n�j, (B):��������"				// 14
};

static const	char*	cnfmsg_e[15] = {
			"(A):OK, (B):Exit",							// 0
			"(A):Run, (B):Cancel",						// 1
			"Please pull out a present card",			// 2
			"from Slot-1 and set a target card.",		// 3
			"The file is made by this file name.",		// 4
			"Replace with the file of this file name.",	// 5
			"The file is read by this file name.",		// 6
			"The card of Slot-1 cannot be recognized.",	// 7
			"Please pull out once, and set it again.",	// 8
			"The SAVE area of ROM is initialized.",		// 9
			"DSi Enhanced card detected.",				// 10
			"Please eject and reinsert to finish.",		// 11
			"TWL card not supported on legacy cardLib.",// 12
			"",											// 13
			"(A):OK, (B):Ignore"							// 14
};


static const	char*	barmsg_j[5] = {
			"�@Save Backup...  ",				// 0
			"  Save Restore... ",				// 1
			"�t�@�C����������..",				// 2
			"  Save Erasing... ",				// 3
			"�@ Rom Backup...  ",				// 4
};

static const	char*	barmsg_e[5] = {
			"�@Save Backup...  ",				// 0
			"  Save Restore... ",				// 1
			" File searching.. ",				// 2
			"  Save Erasing... ",				// 3
			"�@ Rom Backup...  ",				// 4
};



static const	char*	t_msg_j[20] = {
			"                            ",			// 0
//			" == �t�@�C��������܂��� == ",			// 1
			" ==    ���p�s��    == ",			// 1
			"                            ",			// 2
			" == �t�@�C�����X�g�擾�� == ",			// 3
			"(A):�w�肵��SAV�t�@�C���̓��e���X�V    ",	// 4
			"(A):�w�肵��SAV�t�@�C������J�[�h�ɏ���",	// 5
			"(A):�w�肵��NDS�t�@�C���̓��e���X�V    ",	// 6
			"(B):�V�K��SAV�t�@�C�����쐬",			// 7
			"(B):ROM��SAVE�̈��������  ",			// 8
			"(B):�V�K��NDS�t�@�C�����쐬",			// 9
			"(X):ROM����",					// 10
			"(R):�R�}���h���[�h�̕ύX     ",		// 11
			"(L)/(R):�R�}���h���[�h�̕ύX",			// 12
			"(L):�R�}���h���[�h�̕ύX     ",		// 13
			"",
			"",
			"",
			"",
			"",
			""
};

static const	char*	t_msg_e[20] = {
			"                          ",			// 0
			"  ==  File not found  ==  ",			// 1
			"                            ",			// 2
			" == Reading of file list == ",			// 3
			"(A):Specified SAV file is updated.    ",	// 4
			"(A):Restore data in Card from SAV file",	// 5
			"(A):Specified NDS file is updated.    ",	// 6
			"(B):New SAV file create.�@�@�@�@�@�@",		// 7
			"(B):SAVE area of ROM is initialized.",		// 8
			"(B):New NDS file create.�@�@�@�@�@�@",		// 9
			"(X):Change ROM",			// 10
			"(R):Change Command mode    ",		// 11
			"(L)/(R):Change Command mode",		// 12
			"(L):Change Command mode    ",		// 13
			"",
			"",
			"",
			"",
			"",
			""
};


static const	char*	stsmsg_j[4] = {
			"FAT�̏�������....",
			"",
			"",
			"",
};

static const	char*	stsmsg_e[4] = {
			"FAT Initialize....",
			"",
			"",
			""
};

void setLangMsg() {
	u32	UserLang = 0;
	int	i;

	UserLang = PersonalData->language;

	if(UserLang != 0) {
		for(i = 0; i < 14; i++)errmsg[i] = (char*)errmsg_e[i];
		for(i = 0; i < 15; i++)cnfmsg[i] = (char*)cnfmsg_e[i];
		for(i = 0; i < 5; i++)barmsg[i] = (char*)barmsg_e[i];
		for(i = 0; i < 20; i++)t_msg[i] = (char*)t_msg_e[i];
		for(i = 0; i < 4; i++)stsmsg[i] = (char*)stsmsg_e[i];
		return;
	}

	for(i = 0; i < 14; i++)errmsg[i] = (char*)errmsg_j[i];
	for(i = 0; i < 15; i++)cnfmsg[i] = (char*)cnfmsg_j[i];
	for(i = 0; i < 5; i++)barmsg[i] = (char*)barmsg_j[i];
	for(i = 0; i < 20; i++)t_msg[i] = (char*)t_msg_j[i];
	for(i = 0; i < 4; i++)stsmsg[i] = (char*)stsmsg_j[i];
}

