#ifdef __cplusplus
extern "C" {
#endif

extern void Unicode2Local(u16 *srcstr, u8 *dststr, int len);
extern void Unicode2LFN(u16 *srcstr, u8 *dststr);
extern int UnicodeLen(u16 *str);
extern bool UnicodeCmp(u16 *s1,u16 *s2);
extern void UTF82Unicode(u8 *srcstr, u16 *dststr);

#ifdef __cplusplus
}
#endif

