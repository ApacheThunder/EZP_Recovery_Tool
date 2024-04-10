#include <nds.h>
#include <fat.h>
#include <sys/stat.h>

#include <dirent.h>
#include <stdio.h>
#include <string>

using namespace std;

struct DirEntry { string name; bool isDirectory; };

// #include "disc_io.h"
// #include "gba_nds_fat.h"

#include "sd.h"

/*#define FAT_FT_END (0)
#define FAT_FT_FILE (1)
#define FAT_FT_DIR (2)

#include "unicode.h"*/


#include "tarosa/tarosa_Graphic.h"
#include "tarosa/tarosa_Shinofont.h"

extern uint16* SubScreen;

using namespace std;

//#ifdef __cplusplus
//extern "C" {
//#endif
//extern u32 inp_key();
//#ifdef __cplusplus
//}
//#endif

struct	ini_file	ini;
struct	SD_File	fs[200];

extern	char	*romsc2;

const string NDSFile = ".nds";
const string SAVFile = ".sav";

void SD_ini() {
	FILE	*ftpini;
	int	len, p, s;
	char	key[20];

	ini.save = 0;
	ini.trim = 0;
	strcpy(ini.dir, "/NDS_Backup");


	ftpini = fopen("/NDS_Backup_Tool_Slot2.ini", "rb");
	if(ftpini == NULL) {
		// FAT_mkdir(ini.dir);
		mkdir(ini.dir, 0777);
		return;
	}

	len = fread(romsc2, 0x4000, 1, ftpini);

	p = 0;
	while(p < len) {
		if(romsc2[p] == '#' || romsc2[p] == '!') {
			while(p < len) {
				if(romsc2[p] == 0x0A)
					break;
				p++;
			}
			p++;
		}

		s = 0;
		while(romsc2[p] >= 0x20 && romsc2[p] < 0x7F) {
			key[s] = romsc2[p];
			s++;
			p++;
		}
		key[s] = 0;

		if(strcmp(key, "FileDir") == 0) {
			s = 0;
			while(p < len) {
				if(romsc2[p] == 0x20 || romsc2[p] == '\t')
					p++;
				else	break;
			}
			while(p < len) {
				if(romsc2[p] <= 0x20 || romsc2[p] >= 0x7F)
					break;
				if(s < 63)
					ini.dir[s] = romsc2[p];
				s++;
				p++;
			}
			ini.dir[s] = 0;
		}

		if(strcmp(key, "SaveFile") == 0) {
			ini.save = 0;
			while(p < len) {
				if(romsc2[p] >= 0x30 && romsc2[p] <= 0x39)
					break;
				p++;
			}
			while(p < len) {
				if(romsc2[p] < 0x30 || romsc2[p] > 0x39)
					break;
				ini.save = ini.save * 10 + romsc2[p] - 0x30;
				p++;
			}
		}
		if(strcmp(key, "Trim") == 0) {
			ini.trim = 1;
		}

		while(p < len) {
			p++;
			if(romsc2[p - 1] == 0x0A)
				break;
		}
	}

	fclose(ftpini);

	if(ini.dir[0] != '/')strcpy(ini.dir, "/NDS_Backup");

	// FAT_mkdir(ini.dir);
	if(access(ini.dir, F_OK) != 0)mkdir(ini.dir, 0777);

}

extern	char	tbuf[];


static bool nameEndsWith (const string& name, const string ext) {
	/*if (name.size() == 0) return false;
	if (name.front() == '.') return false;*/
	
	if (strcasecmp(name.c_str() + name.size() - ext.size(), ext.c_str()) == 0) return true;
	return false;
}

// Todo - Fix this to modern libfat standards - Apache Thunder
int SD_FileList(int type) {
	int	num;

//	FAT_mkdir("/NDS_Backup");
	// FAT_CWD(ini.dir);
	if(access(ini.dir, F_OK) != 0)mkdir(ini.dir, 0777);
	
	DIR *pdir = opendir(ini.dir);
	struct dirent *pent;
	
	if (pdir) {
		while ((pent=readdir(pdir))!=NULL) {
	    	if(strcmp(".", pent->d_name) == 0 || strcmp("..", pent->d_name) == 0)continue;
	    	if(pent->d_type != DT_DIR) {
				bool isValidFile = false;
				switch (type) {
					case 0: { 
						if (nameEndsWith(pent->d_name, ".sav")) { isValidFile = true; } 
					}break;
					case 1: { 
						if (nameEndsWith(pent->d_name, ".nds")) { isValidFile = true; }
					}break;
					default: { isValidFile = false; } break;
				}
				if (isValidFile) {
					
					num++;
				}
			}
		}
		closedir(pdir);
		return num;
	} else {
		return 0;
	}
	
	/*u32	flen;
	char	tn[3];

	u32	FAT_FileType;

	
	if(type == 0) {
		tn[0] = 'S';
		tn[1] = 'A';
		tn[2] = 'V';
	} else {
		tn[0] = 'N';
		tn[1] = 'D';
		tn[2] = 'S';
	}


	num = 0;
	FAT_FileType = FAT_FindFirstFile(tbuf);
	while(FAT_FileType != FAT_FT_END) {
		if(FAT_FileType == FAT_FT_FILE) {
			flen = strlen(tbuf);
			if((tbuf[flen - 3] == tn[0]) && (tbuf[flen - 2] == tn[1]) && (tbuf[flen - 1] == tn[2])) {
				FAT_GetLongFilenameUnicode(fs[num].uniname, 256);
//				FAT_GetLongFilename(fs[numFiles].filename);
				strcpy(fs[num].Alias, tbuf);
				fs[num].filesize = FAT_GetFileSize();
				num++;
				if(num > 199)	break;
			}
		}
		FAT_FileType=FAT_FindNextFile(tbuf);
	}

	return(num);*/
}

