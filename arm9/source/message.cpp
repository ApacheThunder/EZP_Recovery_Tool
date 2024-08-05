
#include <stdio.h>

#include <NDS.h>

#include "message.h"

char	*errmsg[14];
char	*cnfmsg[15];
char	*barmsg[5];
char	*t_msg[20];
char	*stsmsg[4];

const	char*	errmsg_j[14] = {
			"(A):確認",					// 0
			"FATの初期化に失敗しました",			// 1
			"適切なDLDIパッチを行ってください",		// 2
			"",		// 3
			"",			// 4
			"",			// 5
			"iniファイルの設定を確認してください",		// 6
			"ファイルの転送が失敗しました",			// 7
			"再度、やり直してください",			// 8
			"指定したSAVファイルは、ROMのSAVE領域",		// 9
			"より小さいファイルです",			// 10
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
			"(A):ＯＫ, (B):終了",				// 0
			"(A):実行, (B):取消",				// 1
			"Slot-1から現在のカードを抜いて",		// 2
			"目的のカードをセットしてください",		// 3
			"このファイル名でファイルを作成します",		// 4
			"このファイル名でファイルを上書きします",	// 5
			"このファイル名のファイルを読込みます",		// 6
			"Slot-1のカードが認識できません",		// 7
			"一度抜いてから再度セットしてください",		// 8
			"Slot-1のROMのSAVE領域を初期化します",		// 9
			"DSi Enhanced カードが検出されました。",		// 10
			"取り出して再度挿入して完了してください。",		// 11
			"TWL カードは従来の CardLib ではサポートされていません。",		// 12
			"",								// 13
			"(A):ＯＫ, (B):無視する"				// 14
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
			"　Save Backup...  ",				// 0
			"  Save Restore... ",				// 1
			"ファイルを検索中..",				// 2
			"  Save Erasing... ",				// 3
			"　 Rom Backup...  ",				// 4
};

static const	char*	barmsg_e[5] = {
			"　Save Backup...  ",				// 0
			"  Save Restore... ",				// 1
			" File searching.. ",				// 2
			"  Save Erasing... ",				// 3
			"　 Rom Backup...  ",				// 4
};



static const	char*	t_msg_j[20] = {
			"                            ",			// 0
//			" == ファイルがありません == ",			// 1
			" ==    利用不可    == ",			// 1
			"                            ",			// 2
			" == ファイルリスト取得中 == ",			// 3
			"(A):指定したSAVファイルの内容を更新    ",	// 4
			"(A):指定したSAVファイルからカードに書込",	// 5
			"(A):指定したNDSファイルの内容を更新    ",	// 6
			"(B):新規のSAVファイルを作成",			// 7
			"(B):ROMのSAVE領域を初期化  ",			// 8
			"(B):新規のNDSファイルを作成",			// 9
			"(X):ROM交換",					// 10
			"(R):コマンドモードの変更     ",		// 11
			"(L)/(R):コマンドモードの変更",			// 12
			"(L):コマンドモードの変更     ",		// 13
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
			"(B):New SAV file create.　　　　　　",		// 7
			"(B):SAVE area of ROM is initialized.",		// 8
			"(B):New NDS file create.　　　　　　",		// 9
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
			"FATの初期化中....",
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

