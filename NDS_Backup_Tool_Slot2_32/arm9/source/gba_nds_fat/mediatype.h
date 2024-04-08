
#ifndef MediaType_h
#define MediaType_h

#ifdef __cplusplus
extern "C" {
#endif

typedef enum {DIMT_NONE=0,DIMT_M3CF,DIMT_M3SD,DIMT_MPCF,DIMT_SCCF,DIMT_SCSD,DIMT_FCSR,DIMT_EZSD,DIMT_MMCF,DIMT_SCMS,DIMT_EWSD,DIMT_NMMC,DIMT_NJSD,DIMT_R4TF} EDIMediaType;

extern EDIMediaType DIMediaType;
extern const char *DIMediaName;

#ifdef __cplusplus
}
#endif

#endif
