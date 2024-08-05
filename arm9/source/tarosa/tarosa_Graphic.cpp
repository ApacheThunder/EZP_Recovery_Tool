//************************************************************************
//
//�@�摜�����֌W�̃v���O����   2007.05.04
//
//************************************************************************
#include <nds.h>
#include <stdio.h>
#include <string.h>
#include <math.h>

#include <stdlib.h>
#include <unistd.h>

#include "tarosa_Graphic.h"

//���C���X�N���[���ƃT�u�X�N���[���̐ݒ�
uint16* MainScreen = VRAM_A;
uint16* SubScreen = (uint16*)BG_TILE_RAM_SUB(1);


/**********************************************
*                                             *
*         MainScreen�ɓ_��`�悷��            *
*                      yasuho����̃\�[�X��� *
**********************************************/
void Pixel( u16* screen, s16 x, s16 y, u16 color )
{
	if(x < 0 || x > 256-1 || y < 0 || y > 192-1) return;
	screen[y*256+x] = color;
}

/**********************************************
*                                             *
*           SubScreen�ɓ_��`�悷��           *
*                                             *
**********************************************/
void Pixel_SUB(uint16* screen, s16 x, s16 y, uint16 palet )
{
uint16	bgsubAddress, h;

	if(x < 0 || x > 256-1 || y < 0 || y > 192-1) return;

	bgsubAddress = (x>>3)*32 + ((x&7)>>1) + (y>>3)*1024 + ((y&7)<<2);
	h = screen[bgsubAddress];

	if( (x&1)==0 ){
		h = palet | (h&0xff00);
	}
	else{
		h = (palet*0x100) | (h&0xff);
	}
	screen[bgsubAddress] = h;
}

/**********************************************
*                                             *
*  �T�u��ʂ��p���b�g�R�[�h�œh��Ԃ��܂�   *
*                                             *
**********************************************/
void ClearBG_SUB(uint16* screen, uint16 palet)
{
int x=0,y=0;

	for( y=0; y<192; y++ ){
		for( x=0; x<256; x++ ){
			Pixel_SUB( screen, x, y, palet );
		}
	}
}

/**********************************************
*                                             *
*  ���C����ʂ��J���[�R�[�h�œh��Ԃ��܂�   *
*                      yasuho����̃\�[�X��� *
**********************************************/
void ClearBG(uint16* screen,uint16 color){

int x=0,y=0;
	for( y=0; y<192; y++ ){
		for( x=0; x<256; x++ ){
			screen[(y)*256 + (x)] = color;
		}
	}
}

/**********************************************
*                                             *
*          ���C����ʂɐ��������܂�           *
*                     yasuho����̃\�[�X���  *
**********************************************/
void Line(u16* screen,int x0,int y0,int x1,int y1,int color)
{
int E,x,y;
int dx,dy,sx,sy,i;

	sx = ( x1 > x0 ) ? 1 : -1;
	dx = ( x1 > x0 ) ? x1 - x0 : x0 - x1;
	sy = ( y1 > y0 ) ? 1 : -1;
	dy = ( y1 > y0 ) ? y1 - y0 : y0 - y1;

	x = x0;
	y = y0;
	/* �X����1�ȉ��̏ꍇ */
	if( dx >= dy ) {
		E = -dx;
		for( i = 0; i <= dx; i++ ) {
			Pixel(screen, x, y, color );
			x += sx;
			E += 2 * dy;
			if( E >= 0 ) {
				y += sy;
				E -= 2 * dx;
			}
		}
		/* �X����1���傫���ꍇ */
	}
	else {
		E = -dy;
		for( i = 0; i <= dy; i++ ) {
			Pixel(screen, x, y, color );
			y += sy;
			E += 2 * dx;
			if( E >= 0 ) {
				x += sx;
				E -= 2 * dy;
			}
		}
	}
}

/**********************************************
*                                             *
*          �T�u��ʂɐ��������܂�             *
*                                             *
**********************************************/
void Line_Sub(u16* screen,int x0,int y0,int x1,int y1,u8 lpal)
{
int E,x,y;
int dx,dy,sx,sy,i;

	sx = ( x1 > x0 ) ? 1 : -1;
	dx = ( x1 > x0 ) ? x1 - x0 : x0 - x1;
	sy = ( y1 > y0 ) ? 1 : -1;
	dy = ( y1 > y0 ) ? y1 - y0 : y0 - y1;

	x = x0;
	y = y0;
	/* �X����1�ȉ��̏ꍇ */
	if( dx >= dy ) {
		E = -dx;
		for( i = 0; i <= dx; i++ ) {
			Pixel_SUB(screen, x, y, lpal );
			x += sx;
			E += 2 * dy;
			if( E >= 0 ) {
				y += sy;
				E -= 2 * dx;
			}
		}
		/* �X����1���傫���ꍇ */
	}
	else {
		E = -dy;
		for( i = 0; i <= dy; i++ ) {
			Pixel_SUB(screen, x, y, lpal );
			y += sy;
			E += 2 * dx;
			if( E >= 0 ) {
				x += sx;
				E -= 2 * dy;
			}
		}
	}
}

/**********************************************
*                                             *
*      ���C���X�N���[���̉摜(256�F)��        *
*      �T�u�X�N���[���ɃR�s�[���܂��B         *
*                                             *
**********************************************/
void	ScreenCopy( uint16* mainscrn, uint16* subscrn )
{
uint16 i, x, y, k=0;
uint16 col;
uint16 paln=0;
uint16 paFlag;
	
	for( y=0; y<192; y++ ){
		for( x=0; x<256; x++ ){
			col = mainscrn[y*256+x];
			if( k==0 ){
				BG_PALETTE_SUB[0] = col;
				Pixel_SUB( subscrn, x, y, 0 );
				k++;
			}
			else{
				paFlag=0;
				if( k!=256 ){
					for( i=0; i<k; i++ ){
						if( BG_PALETTE_SUB[i]==col ){
							paln = i;
							paFlag = 1;
							break;
						}
					}
				}
				if( paFlag==0 ){
					BG_PALETTE_SUB[k] = col;
					Pixel_SUB( subscrn, x, y, k );
					k++;
					if( k>=256 )	k=256;
				}
				else{
					Pixel_SUB( subscrn, x, y, paln );
				}
			}
		}
	}
}

/**********************************************
*                                             *
*       �T�u�X�N���[����                      *
*              8�h�b�g��ɃX�N���[�����܂�    *
*                                             *
*    backpalet:�X�N���[���������h��Ԃ�   *
*              �p���b�g�J���[�̎w��           *
*                                             *
**********************************************/
void	ScRl8Dot_SUB( uint16* screen, u8 backpal )
{
uint16* strtAdd;

	strtAdd = screen;
	strtAdd += 1024;
	dmaCopy( strtAdd, screen, 47104);
	for( int j=184; j<192; j++ ){
		for( int i=0; i<256; i++ ){
			Pixel_SUB( screen, i, j, backpal );
		}
	}
}

/**********************************************
*                                             *
*       �T�u�X�N���[����                      *
*            n�h�b�g�㉺�ɃX�N���[�����܂�    *
*             ���ŏ�A���ŉ��ɃX�N���[��      *
*    backpalet:�X�N���[���������h��Ԃ�   *
*              �p���b�g�J���[�̎w��           *
*                                             *
**********************************************/
void	ScRLnDot_SUB( uint16* screen, int n, u8 backpal )
{
int		x,y;

	if( n>0 ){
		for( y=0; y<192-n; y++ ){
			for( x=0; x<256; x++ ){
				SubScreen[ (x>>3)*32 + ((x&7)>>1) + (y>>3)*1024 + ((y&7)<<2) ]
					= SubScreen[(x>>3)*32 + ((x&7)>>1) + ((y+n)>>3)*1024 + (((y+n)&7)<<2)];
			}
		}
		for( y=192-n; y<192; y++ ){
			for( int x=0; x<256; x++ ){
				Pixel_SUB( SubScreen, x, y, backpal );
			}
		}
	}
	else{
		n = -n;
		for( y=192-n; y>=0; y-- ){
			for( x=0; x<256; x++ ){
				SubScreen[ (x>>3)*32 + ((x&7)>>1) + ((y+n)>>3)*1024 + (((y+n)&7)<<2) ]
					= SubScreen[(x>>3)*32 + ((x&7)>>1) + (y>>3)*1024 + ((y&7)<<2)];
			}
		}	
		for( y=0; y<n; y++ ){
			for( x=0; x<256; x++ ){
				Pixel_SUB( SubScreen, x, y, backpal );
			}
		}
	}
}

/**********************************************
*                                             *
*       ���C���X�N���[����                    *
*              8�h�b�g��ɃX�N���[�����܂�    *
*                                             *
*    backcolor:�X�N���[���������h��Ԃ�   *
*              �o�b�N�J���[�̎w��             *
*                                             *
**********************************************/
void	ScRl8Dot( uint16* screen, uint16 backcolor )
{
uint16* strtAdd;

	strtAdd = screen;
	strtAdd += 2048;
	dmaCopy( strtAdd, screen, 47104*2);
	for( int j=184; j<192; j++ ){
		for( int i=0; i<256; i++ ){
			Pixel( screen, i, j, backcolor );
		}
	}
}

/**********************************************
*                                             *
*       ���C���X�N���[����                    *
*            n�h�b�g�㉺�ɃX�N���[�����܂�    *
*             ���ŏ�A���ŉ��ɃX�N���[��      *
*    backcolor:�X�N���[���������h��Ԃ�   *
*              �o�b�N�J���[�̎w��             *
*                                             *
**********************************************/
void	ScRLnDot( uint16* screen, int n, uint16 backcolor )
{
uint16* strtAdd;
int		x, y;

	strtAdd = MainScreen;
	if( n>0 ){
		strtAdd += 256*n;
		dmaCopy( strtAdd, MainScreen, 256*(192-n)*2);
		for( y=192-n; y<192; y++ ){
			for( x=0; x<256; x++ ){
				Pixel( MainScreen, x, y, backcolor );
			}
		}
	}
	else{
		n=-n;
		for( y=192-n; y>=0; y-- ){
			for( x=0; x<256; x++ ){
				MainScreen[(y+n)*256+x] = MainScreen[y*256+x];
			}
		}	
		for( y=0; y<n; y++ ){
			for( x=0; x<256; x++ ){
				Pixel( MainScreen, x, y, backcolor );
			}
		}	
	}
}

/**********************************************
*                                             *
*               �摜�f�[�^��                  *
*     ���C����ʂ̔C�ӂ̈ʒu�Ƀ��[�h����      *
*                                             *
**********************************************/
void	PicLoadMS( uint16 *screen, uint16 *sourceData, uint16 x, uint16 y, uint16 picwide, uint16 pichight )
{
uint16 i;
uint16 j;

	for( j=y; j<(y+pichight); j++ ){
		for( i=x; i<(x+picwide); i++ ){
			Pixel( screen, i, j, sourceData[i-x+(j-y)*picwide] | BIT(15) );
		}
	}
}

/**********************************************
*                                             *
*               �摜�f�[�^��                  *
*     ���C����ʂ̔C�ӂ̈ʒu�Ƀ��[�h����      *
*     ���ߐF���w��ł���                      *
* tcf: 0:���ߖ����A1:���̐F��AND���Ƃ�@�@�@�@*
*      2:���̐F��OR���Ƃ�                     *
*      3:���̐F��XOR���Ƃ�                    *
*      4:���ߐFtc�͕\�����Ȃ�                 *
**********************************************/
void	PicLoadMSo( uint16 *screen, uint16 *sourceData, uint16 x, uint16 y, uint16 picwide, uint16 pichight
					, int tcf, uint16 tc )
{
uint16	i;
uint16	j;
uint16	col;

	switch( tcf ){
	case 1:
		for( j=y; j<(y+pichight); j++ ){
			for( i=x; i<(x+picwide); i++ ){
				Pixel( screen, i, j, (sourceData[i-x+(j-y)*picwide] | BIT(15))&Point( screen, i, j ) );
			}
		}
		break;
	case 2:
		for( j=y; j<(y+pichight); j++ ){
			for( i=x; i<(x+picwide); i++ ){
				Pixel( screen, i, j, (sourceData[i-x+(j-y)*picwide] | BIT(15))|Point( screen, i, j ) );
			}
		}
		break;
	case 3:
		for( j=y; j<(y+pichight); j++ ){
			for( i=x; i<(x+picwide); i++ ){
				Pixel( screen, i, j, (sourceData[i-x+(j-y)*picwide] | BIT(15))^Point( screen, i, j ) );
			}
		}
		break;
	case 4:
		for( j=y; j<(y+pichight); j++ ){
			for( i=x; i<(x+picwide); i++ ){
				col = sourceData[i-x+(j-y)*picwide];
				if( tc!=col ){
					Pixel( screen, i, j, col | BIT(15) );
				}
			}
		}
		break;
	case 0:
	default:
		PicLoadMS( screen, sourceData, x, y, picwide, pichight );
		break;
	}
}


/**********************************************
*                                             *
*               �摜�f�[�^��                  *
*      �T�u��ʂ̔C�ӂ̈ʒu�Ƀ��[�h����       *
*       (�p���b�g�͌���̂��̂��g��)          *
**********************************************/
void	PicLoadSS( uint16 *screen, uint16 *sourceData, uint16 subx, uint16 suby,
							uint16 soux, uint16 souy, uint16 picwide, uint16 pichight )
{
uint16 i;
uint16 j;

	for( j=suby; j<(suby+pichight); j++ ){
		for( i=subx; i<(subx+picwide); i++ ){
			Pixel_SUB( screen, i, j, sourceData[(i-subx+soux)+(j-suby+souy)*picwide] );
		}
	}
}

/**********************************************
*                                             *
*               �摜�f�[�^��                  *
*      �T�u��ʂ̔C�ӂ̈ʒu�Ƀ��[�h����       *
*       (�p���b�g�͌���̂��̂��g��)          *
*     ���ߐF���w��ł���                      *
* tcf: 0:���ߖ����A1:���̐F��AND���Ƃ�@�@�@�@*
*      2:���̐F��OR���Ƃ�                     *
*      3:���̐F��XOR���Ƃ�                    *
*      4:���ߐFcpalet�͕\�����Ȃ�             *
**********************************************/
void	PicLoadSSo( uint16 *screen, uint16 *sourceData, uint16 subx, uint16 suby, uint16 picwide, uint16 pichight,
					int tcf, uint16 cpalet )
{
uint16	i;
uint16	j;
uint16	col;

	switch( tcf ){
	case 1:
		for( j=suby; j<(suby+pichight); j++ ){
			for( i=subx; i<(subx+picwide); i++ ){
				Pixel_SUB( screen, i, j, sourceData[(i-subx)+(j-suby)*picwide]&Point_SUB( screen, i, j ) );
			}
		}
		break;
	case 2:
		for( j=suby; j<(suby+pichight); j++ ){
			for( i=subx; i<(subx+picwide); i++ ){
				Pixel_SUB( screen, i, j, sourceData[(i-subx)+(j-suby)*picwide]|Point_SUB( screen, i, j ) );
			}
		}
		break;
	case 3:
		for( j=suby; j<(suby+pichight); j++ ){
			for( i=subx; i<(subx+picwide); i++ ){
				Pixel_SUB( screen, i, j, sourceData[(i-subx)+(j-suby)*picwide]^Point_SUB( screen, i, j ) );
			}
		}
		break;
	case 4:
		for( j=suby; j<(suby+pichight); j++ ){
			for( i=subx; i<(subx+picwide); i++ ){
				col = sourceData[(i-subx)+(j-suby)*picwide];
				if( cpalet!=col ){
					Pixel_SUB( screen, i, j, col );
				}
			}
		}
		break;
	case 0:
	default:
		PicLoadSS( screen, sourceData, subx, suby, 0, 0, picwide, pichight );
		break;
	}
}


/**********************************************
*                                             *
*     ���C���X�N���[���Ƀ{�b�N�X�������܂�    *
*              full:0 �g�̂�                  *
*                   1 �h��Ԃ�              *
*                     yasuho����̃\�[�X���  *
**********************************************/
void DrawBox(u16* screen, int x0, int y0, int x1, int y1,  uint16 color, int full)
{
	if( full==0 ){
		Line( screen, x0, y0, x1, y0, color );
		Line( screen, x0, y1, x1, y1, color );
		Line( screen, x0, y0, x0, y1, color );
		Line( screen, x1, y0, x1, y1, color );
	}
	else{
		if( y0>y1 ){
			for( int i=y1; i<=y0; i++ ){
				Line( screen, x0, i, x1, i, color );
			}
		}
		else{
			for( int i=y0; i<=y1; i++ ){
				Line( screen, x0, i, x1, i, color );
			}
		}
	}
}

/**********************************************
*                                             *
*     �T�u�X�N���[���Ƀ{�b�N�X�������܂�      *
*              full:0 �g�̂�                  *
*                   1 �h��Ԃ�              *
*                     2007.5.4                *
**********************************************/
void DrawBox_SUB( u16* screen, int x0, int y0, int x1, int y1, uint16 bpalet, int full)
{
	if( full==0 ){
		Line_Sub( screen, x0, y0, x1, y0, bpalet );
		Line_Sub( screen, x0, y1, x1, y1, bpalet );
		Line_Sub( screen, x0, y0, x0, y1, bpalet );
		Line_Sub( screen, x1, y0, x1, y1, bpalet );
	}
	else{
		if( y0>y1 ){
			for( int i=y1; i<=y0; i++ ){
				Line_Sub( screen, x0, i, x1, i, bpalet );
			}
		}
		else{
			for( int i=y0; i<=y1; i++ ){
				Line_Sub( screen, x0, i, x1, i, bpalet );
			}
		}
	}
}

/**********************************************
*                                             *
*  �T�u��ʂ̈�Ԗ��邢�p���b�g�R�[�h������   *
*      return: �p���b�g�R�[�h                 *
*                                             *
**********************************************/
uint16	maxPaletCode( uint16* subpalet )
{
int i;
int	max=0;
int r, g, b;
int	goukei;
int	maxpalet=0;

	for( i=0; i<256; i++ ){
		r = (subpalet[i]>>10)&0x1f;
		g = (subpalet[i]>>5)&0x1f;
		b = subpalet[i]&0x1f;
		goukei = r + g + b;
		if( max<goukei ){
			max = goukei;
			maxpalet = i;
		}
	}
	return( maxpalet );
}

/**********************************************
*                                             *
*   �T�u��ʂ̈�ԈÂ��p���b�g�R�[�h������    *
*      return: �p���b�g�R�[�h                 *
*                                             *
**********************************************/
uint16	minPaletCode( uint16 *subpalet )
{
int i;
int	min;
int r, g, b;
int	goukei;
int	minpalet=0;

	r = (subpalet[0]>>10)&0x1f;
	g = (subpalet[0]>>5)&0x1f;
	b = subpalet[0]&0x1f;
	min = r + g + b;
	for( i=1; i<256; i++ ){
		r = (subpalet[i]>>10)&0x1f;
		g = (subpalet[i]>>5)&0x1f;
		b = subpalet[i]&0x1f;
		goukei = r + g + b;
		if( min>goukei ){
			min = goukei;
			minpalet = i;
		}
	}
	return( minpalet );
}

/**********************************************
*                                             *
*   �T�u��ʂ�(x0,y0)-(x1,y1)�͈͂�           *
*    �p���b�g(bpal)��cpal�p���b�g�ԍ���       *
*    ����ւ��܂�                             *
*                                             *
**********************************************/
void	ColorSwap_SUB( u16* screen, s16 x0, s16 y0, s16 x1, s16 y1, uint16 bpalet, uint16 cpalet )
{
s16		x, y;

	for( y=y0+1; y<y1; y++ ){
		for( x=x0+1; x<x1; x++ ){
			if( Point_SUB( screen, x, y )==bpalet )	Pixel_SUB( screen, x, y, cpalet );
		}
	}
}

/**********************************************
*                                             *
*     �T�u��ʏ�̓_�̃p���b�g�ԍ��𓾂�      *
*                                             *
**********************************************/
uint16	Point_SUB( u16* screen, s16 x, s16 y )
{
uint16	bgsubAddress, h;

	if(x < 0 || x > 256-1 || y < 0 || y > 192-1) return( 0 );

	bgsubAddress = (x>>3)*32 + ((x&7)>>1) + (y>>3)*1024 + ((y&7)<<2);
	h = screen[bgsubAddress];

	if( (x&1)==0 ){
		return( h&0xff );
	}
	else{
		return( h>>8 );
	}
}

// 2006.09.09�ǉ���
/**********************************************
*                                             *
*           ���C����ʏ�̐F�𓾂�            *
*                                             *
**********************************************/
int	Point( u16* screen, s16 x, s16 y )
{
	if(x < 0 || x > 256-1 || y < 0 || y > 192-1) return( 0 );

	return( (int)screen[y*256+x] );
}

// 2006.09.18�ǉ���
/**********************************************
*                                             *
*   ���C����ʂ�(x0,y0)-(x1,y1)�͈͂�         *
*    �F(bcolor)��ccolor�F�ɓ���ւ��܂�       *
*                                             *
**********************************************/
void	ColorSwap( u16* screen, s16 x0, s16 y0, s16 x1, s16 y1, uint16 bcolor, uint16 ccolor )
{
s16		x, y;

	for( y=y0+1; y<y1; y++ ){
		for( x=x0+1; x<x1; x++ ){
			if( Point( screen, x, y )==bcolor )	Pixel( screen, x, y, ccolor );
		}
	}
}
