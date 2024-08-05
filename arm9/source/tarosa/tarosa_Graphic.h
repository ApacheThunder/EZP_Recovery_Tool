#ifdef __cplusplus
extern "C" {
#endif
//************************************************************************
//
//�@�摜�����֌W�̃v���O�����̃w�b�_   2007.05.03
//
//************************************************************************

//MainScreen�ɓ_��`�悷��
void Pixel( u16* screen, s16 x, s16 y, u16 color );

//SubScreen�ɓ_��`�悷��
void Pixel_SUB(uint16* screen, s16 x, s16 y, uint16 palet );

//�T�u��ʂ��p���b�g�R�[�h�œh��Ԃ��܂�
void ClearBG_SUB(uint16* screen, uint16 palet);

//���C����ʂ��J���[�R�[�h�œh��Ԃ��܂�
void ClearBG(uint16* screen,uint16 color);

//���C����ʂɐ��������܂�
void Line(u16* screen,int x0,int y0,int x1,int y1,int color);

//�T�u��ʂɐ��������܂�
void Line_Sub(u16* screen,int x0,int y0,int x1,int y1,u8 lpal);

//���C���X�N���[���̉摜(256�F)��
// �T�u�X�N���[���ɃR�s�[���܂��B
void	ScreenCopy( uint16* mainscrn, uint16* subscrn );

//�T�u�X�N���[����8�h�b�g��ɃX�N���[�����܂�
void	ScRl8Dot_SUB( uint16* screen, u8 backpal );

//�T�u�X�N���[����n�h�b�g�㉺�ɃX�N���[�����܂�
void	ScRLnDot_SUB( uint16* screen, int n, u8 backpal );

//���C���X�N���[����8�h�b�g��ɃX�N���[�����܂�
void	ScRl8Dot( uint16* screen, uint16 backcolor );

//���C���X�N���[����n�h�b�g�㉺�ɃX�N���[�����܂�
void	ScRLnDot( uint16* screen, int n, uint16 backcolor );

//�摜�f�[�^�����C����ʂ̔C�ӂ̈ʒu�Ƀ��[�h����
void	PicLoadMS( uint16 *screen, uint16 *sourceData, uint16 x, uint16 y, uint16 picwide, uint16 pichight );

//�摜�f�[�^���T�u��ʂ̔C�ӂ̈ʒu�Ƀ��[�h����
void	PicLoadSS( uint16 *screen, uint16 *sourceData, uint16 subx, uint16 suby, uint16 soux, uint16 souy, uint16 picwide, uint16 pichight );

//���C���X�N���[���Ƀ{�b�N�X�������܂�
void DrawBox(u16* screen, int x0, int y0, int x1, int y1,  uint16 color, int full);

//�T�u�X�N���[���Ƀ{�b�N�X�������܂�
void DrawBox_SUB(u16* screen, int x0, int y0, int x1, int y1, uint16 bpalet, int full);

//�T�u��ʂ̈�Ԗ��邢�p���b�g�R�[�h������
uint16	maxPaletCode( uint16* subpalet );

//�T�u��ʂ̈�ԈÂ��p���b�g�R�[�h������
uint16	minPaletCode( uint16 *subpalet );

//�T�u��ʂ�(x0,y0)-(x1,y1)�͈͂̃p���b�g(bpal)��cpal�p���b�g�ԍ���
void	ColorSwap_SUB( u16* screen, s16 x0, s16 y0, s16 x1, s16 y1, uint16 bpalet, uint16 cpalet );

//�T�u��ʏ�̓_�̃p���b�g�ԍ��𓾂�
uint16	Point_SUB( u16* screen, s16 x, s16 y );

// 2006.09.09�ǉ���
//���C����ʏ�̐F�𓾂�
int	Point( u16* screen, s16 x, s16 y );

// 2006.09.18�ǉ���
//���C����ʂ�(x0,y0)-(x1,y1)�͈͂̐Fbcolor��ccolor�F��
void	ColorSwap( u16* screen, s16 x0, s16 y0, s16 x1, s16 y1, uint16 bcolor, uint16 ccolor );

//2007.05.03�ǉ�
//���C����ʂ̔C�ӂ̈ʒu�Ƀ��[�h����B���ߐF�Ȃǂ��w��ł���
void	PicLoadMSo( uint16 *screen, uint16 *sourceData, uint16 x, uint16 y, uint16 picwide, uint16 pichight, int tcf, uint16 tc );
//�T�u��ʂ̔C�ӂ̈ʒu�Ƀ��[�h����B���ߐF�Ȃǂ��w��ł���
void	PicLoadSSo( uint16 *screen, uint16 *sourceData, uint16 subx, uint16 suby, uint16 picwide, uint16 pichight,	int tcf, uint16 cpalet );

#ifdef __cplusplus
}
#endif
