#ifdef __cplusplus
extern "C" {
#endif

//************************************************************************
//
//  ���{��\���֘A�̃v���O�����̃w�b�_   2006.07.25
//
//************************************************************************
// SJIS�R�[�h����b�����t�H���g�f�[�^���i�[�A�h���X���v�Z����
short Sjis2Elisa( short u, short d );
// ���C����ʂɓ��_12�t�H���g��\������
void ShinoPrint( uint16* screen, int x, int y, u8 *st, u16 Fcolor, u16 Bcolor, u8 kaki );
// �T�u��ʂɓ��_12�t�H���g��\������
void ShinoPrint_SUB( uint16* screen, int x, int y, u8 *st, u16 Fpalet, u16 Bpalet, u8 kaki );

#ifdef __cplusplus
}
#endif
