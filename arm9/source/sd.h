
struct	ini_file {
	char	dir[64];
};

extern	struct	ini_file	ini;

struct	SD_File {
	u32 filesize;
	char filename[40];
};

extern	struct	SD_File	fs[];

#ifdef __cplusplus
extern "C" {
#endif

extern	void	SD_ini();
extern	int	SD_FileList();

#ifdef __cplusplus
}
#endif

