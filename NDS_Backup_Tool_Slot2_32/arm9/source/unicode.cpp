
#include <stdio.h>

#include <NDS.h>

#include "unicode.h"
// #include "unicode_u2l_bin.h"

extern u16 unicode_u2l_bin[];

void Unicode2Local(u16 *srcstr, u8 *dststr, int len) {
//	u16	uni[128];
//	u16	uc;
	u16	dt;
	u16	*uni;

	// uni = (u16*)unicode_u2l_bin + 2;
	uni = unicode_u2l_bin + 2;
//	UTF82Unicode(srcstr, uni);
//  	uc = (*srcstr) + (*(srcstr+1) << 8);
	while(*srcstr != 0) {
		dt = uni[*srcstr];
		*dststr=(u8)(dt & 0xFF);
		dststr++;
		len--;
		dt = (dt >> 8) & 0xFF;
		if(dt != 0) {
			*dststr = (u8)dt;
			if(len == 0) {
				dststr--;
				break;
			}
			dststr++;
			len--;
		}
		if(len == 0)	break;
		srcstr++;
//	  	uc = (*srcstr) + (*(srcstr+1) << 8);
	}
	*dststr = 0;
}

void Unicode2LFN(u16 *srcstr, u8 *dststr)
{
	while(*srcstr != 0) {
		*dststr=(u8)(*srcstr & 0xFF);
		dststr++;
		srcstr++;
	}
	*dststr = 0;
}

int UnicodeLen(u16 *str)
{
	int	len = 0;

	while(*str != 0) {
		str++;
		len++;
	}
	return(len);
}

bool UnicodeCmp(u16 *s1,u16 *s2)
{
  if((s1==0)&&(s2==0)) return(true);
  if((s1==0)||(s2==0)) return(false);
  
  while(*s1==*s2){
    if((*s1==0)||(*s2==0)){
      if((*s1==0)&&(*s2==0)){
        return(true);
        }else{
        return(false);
      }
    }
    s1++;
    s2++;
  }
  return(false);
}


void UTF82Unicode(u8 *srcstr, u16 *dststr)
{
	while(*srcstr!=0) {
		u16 b0=(byte)srcstr[0], b1=(byte)srcstr[1], b2=(byte)srcstr[2];
		u16 uc;

		if(b0<0x80) {
			uc=b0;
			srcstr++;
		} else {
			if((b0&0xe0)==0xc0){		// 0b 110. ....
				uc=((b0&~0xe0)<<6)+((b1&~0xc0)<<0);
				srcstr+=2;
			} else {
				if((b0&0xf0)==0xe0) {	// 0b 1110 ....
					uc=((b0&~0xf0)<<12)+((b1&~0xc0)<<6)+((b2&~0xc0)<<0);
					srcstr += 3;
				} else {
					uc=(u16)'?';
					srcstr+=4;
				}
			}
		}

		*dststr=uc;
		dststr++;
	}
  
	*dststr=0;
}

