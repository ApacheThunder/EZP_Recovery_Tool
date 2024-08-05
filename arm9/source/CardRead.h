#ifdef __cplusplus
extern "C" {
#endif

extern	void	wait_TM1(u32 tim);
extern	void	port_setcmd(u32* normal, u32* key1, u8* header);
extern	int	Key1_cmd(u32 cmd1, u32 cmd2, u32 id, u8* tbl);
extern	void	enc_key2(u32 mmmnnn, u8 key2);
extern	void	encryObj(u32 Gamecode, u8* sc1, u8* tbl);

extern	void	Read_Init(void);
extern	int	Read_dummy(void);
extern	int	Read_Header(u8* header);
extern	u32	Read_CardID_1(void);
extern	bool	Act_Key1(void);

extern	void	Act_Key2(void);
extern	u32	Read_CardID_2(void);
extern	int	Read_SData(u8* dbuf, u32 addr);
extern	void	Data_Mode(void);

extern	int	Read_Data(u8* dbuf, u32 addr);
extern	u32	Read_CardID_3(void);

extern	u32	Rom_Read(int type, u8* header, u8* sc1);
#ifdef __cplusplus
}
#endif

