#include <nds.h>
#include <fat.h>
#include <sys/stat.h>

#include <dirent.h>
#include <stdio.h>
#include <string>
#include <vector>

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

const char* defaultPath = "/NDS_Backup";
const char* defaultINI = "/NDS_Backup_Tool_Slot2.ini";

extern u8 defaultSettings[];
extern u8 defaultSettingsEnd[];

bool dirEntered = false;

void SD_ini() {
	FILE *ftpini = NULL;
	int	len, p, s;
	char key[20];

	ini.save = 0;
	ini.trim = 0;
	
	strcpy(ini.dir, defaultPath);
	
	if(access(defaultINI, F_OK) == 0) { 
		ftpini = fopen(defaultINI, "rb");
	} else {
		ftpini = fopen(defaultINI, "wb");
		if (ftpini) {
			fwrite(defaultSettings, (defaultSettingsEnd - defaultSettings), 1, ftpini);
			fclose(ftpini);
		}
		// FAT_mkdir(ini.dir);
		if(access(ini.dir, F_OK) != 0)mkdir(ini.dir, 0777);
		return;
	}
	
	if (!ftpini) {
		// FAT_mkdir(ini.dir);
		if(access(ini.dir, F_OK) != 0)mkdir(ini.dir, 0777);
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
				
		if(strcmp(key, "Trim") == 0)ini.trim = 1;
		
		while(p < len) {
			p++;
			if(romsc2[p - 1] == 0x0A)break;
		}
	}

	fclose(ftpini);

	if(ini.dir[0] != '/')strcpy(ini.dir, defaultPath);

	// FAT_mkdir(ini.dir);
	if(access(ini.dir, F_OK) != 0)mkdir(ini.dir, 0777);

}

extern	char	tbuf[];


bool nameEndsWith (const string& name, const string& extension) {

	if (name.size() == 0) return false;
	if (name.front() == '.') return false;

	const string ext = extension;
	if (strcasecmp(name.c_str() + name.size() - ext.size(), ext.c_str()) == 0)return true;
	return false;
}

int SD_FileList(int type) {
	int	num = 0;

	if(access(ini.dir, F_OK) != 0)mkdir(ini.dir, 0777);
	
	struct stat st;
	chdir (ini.dir);
	
	DIR *pdir = opendir (".");
	
	const char* EXT;
	if (type == 0) { EXT = ".SAV"; } else { EXT = ".NDS"; }

	if (pdir != NULL) {
		while(true) {
			dirent* pent = readdir(pdir);
			if(pent == NULL)break;
						
			stat(pent->d_name, &st);

			if (((string)pent->d_name).compare(".") != 0 && ((st.st_mode & S_IFMT) != S_IFDIR) && nameEndsWith(pent->d_name, EXT)) {
				strcpy(fs[num].filename, pent->d_name);
				FILE *file = fopen(pent->d_name, "rb");
				if (file) {
					fseek(file, 0, SEEK_END);
					fs[num].filesize = ftell(file);
					fclose(file);
				}
				num++;
				if (num > 200)return 200;
			}
		}
		closedir(pdir);
	}
	return num;
}

