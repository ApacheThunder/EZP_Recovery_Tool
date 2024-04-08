/*

	disc_io.c

	uniformed io-interface to work with Chishm's FAT library

	Written by MightyMax
  
	Modified by Chishm:
	2005-11-06
		* Added WAIT_CR modifications for NDS

	Modified by www.neoflash.com:
	2006-02-03
		* Added SUPPORT_* defines, comment out any of the SUPPORT_* defines in disc_io.h to remove support
		  for the given interface and stop code being linked to the binary

	    * Added support for MK2 MMC interface

		* Added disc_Cache* functions

	Modified by Chishm:
	2006-02-05
		* Added Supercard SD support
*/

#include "disc_io.h"

#ifdef NDS
	#include <nds.h>
#endif


// Include known io-interfaces:
#include "io_dldi.h"

#define WAIT_CR	REG_EXMEMCNT


// Keep a pointer to the active interface
LPIO_INTERFACE active_interface = 0;


/*

	Disc Cache functions
	2006-02-03:
		Added by www.neoflash.com 

*/

#ifdef DISC_CACHE

#include <string.h>

#define CACHE_FREE 0xFFFFFFFF
	
static u8 cacheBuffer[ DISC_CACHE_COUNT * 512 ];

static struct {
	u32 sector;
	u32 dirty;
	u32 count;
} cache[ DISC_CACHE_COUNT ];

static u32 disc_CacheFind(u32 sector) {
	u32 i;
	
	for( i = 0; i < DISC_CACHE_COUNT; i++ )	{
		if( cache[ i ].sector == sector )
			return i;
	}
	
	return CACHE_FREE;
}

static u32 disc_CacheFindFree(void) {
	
	u32 i = 0, j;
	u32 count = -1;
	
	for( j = 0; j < DISC_CACHE_COUNT; j++ )	{

		if( cache[ j ].sector == CACHE_FREE ) {
			i = j;
			break;
		}

		if( cache[ j ].count < count ) {
			count = cache[ j ].count;
			i = j;
		}
	}
	
	if( cache[ i ].sector != CACHE_FREE && cache[i].dirty != 0 ) {

		active_interface->fn_WriteSectors( cache[ i ].sector, 1, &cacheBuffer[ i * 512 ] );
		/* todo: handle write error here */

		cache[ i ].sector = CACHE_FREE;
		cache[ i ].dirty = 0;
		cache[ i ].count = 0;
	}

	return i;
}

void disc_CacheInit(void)	{

	u32 i;

	for( i = 0; i < DISC_CACHE_COUNT; i++ )	{
		cache[ i ].sector = CACHE_FREE;
		cache[ i ].dirty = 0;
		cache[ i ].count = 0;
	}

}

bool disc_CacheFlush(void)	{

	u32 i;

	if( !active_interface )	return false;

	for( i = 0; i < DISC_CACHE_COUNT; i++ )	{
		if( cache[ i ].sector != CACHE_FREE && cache[ i ].dirty != 0 )	{
			if( active_interface->fn_WriteSectors( cache[ i ].sector, 1, &cacheBuffer[ i * 512 ] ) == false )
				return false;

			cache[ i ].dirty = 0;
		}
	}
	return true;
}

bool disc_CacheReadSector( void *buffer, u32 sector) {
	u32 i = disc_CacheFind( sector );
	if( i == CACHE_FREE ) {
		i = disc_CacheFindFree();
		cache[ i ].sector = sector;
		if( active_interface->fn_ReadSectors( sector, 1, &cacheBuffer[ i * 512 ] ) == false )
			return false;
	}
	memcpy( buffer, &cacheBuffer[ i * 512 ], 512 );
	cache[ i ].count++;
	return true;
}

bool disc_CacheWriteSector( void *buffer, u32 sector ) {
	u32 i = disc_CacheFind( sector );
	if( i == CACHE_FREE ) {
		i = disc_CacheFindFree();
		cache [ i ].sector = sector;
	}
	memcpy( &cacheBuffer[ i * 512 ], buffer, 512 );
	cache[ i ].dirty=1;
	cache[ i ].count++;
	return true;
}

#endif

/*

	Hardware level disc funtions

*/

//#include "../_console.h"

#include "mediatype.h"

EDIMediaType DIMediaType=DIMT_NONE;
const char *DIMediaName="No adapter.";
char DIMediaID[5]="NONE";

bool disc_Init(void) 
{
#ifdef DISC_CACHE
	disc_CacheInit();
#endif

    if (active_interface != 0) return true;
    
//    WAIT_CR &= ~(ARM9_OWNS_ROM | ARM9_OWNS_CARD);
	WAIT_CR &= ~(0x8080);

    active_interface=DLDI_GetInterface();
    if(active_interface!=0){
      DIMediaType=(EDIMediaType)active_interface->ul_ioType;
      DIMediaName=DLDI_GetAdapterName();
      *(u32*)&DIMediaID[0]=active_interface->ul_ioType;
      *(u32*)&DIMediaID[4]=0;
      
//      _consolePrintOne("active_interface = 0x%08x\n",(u32)active_interface);
//      _consolePrintOne("DeviceID=%s\n",(u32)DIMediaID);
//      _consolePrintOne("DeviceName=%s\n",(u32)DIMediaName);
//      _consolePrintOne("StartUp=0x%08x\n",(u32)active_interface->fn_StartUp);
      
      if(active_interface->fn_StartUp!=0){
        if(active_interface->fn_StartUp()==true) return true;
      }
    }
    
	// could not find a working IO Interface
	active_interface = 0 ;
	return false ;
} 

bool disc_IsInserted(void) 
{
	if (active_interface) return active_interface->fn_IsInserted() ;
	return false ;
} 

bool disc_ReadSectors(u32 sector, u8 numSecs, void* buffer) 
{
#ifdef DISC_CACHE
	u8 *p=(u8*)buffer;
	u32 i;
	u32 inumSecs=numSecs;
	if(numSecs==0)
		inumSecs=256;
	for( i = 0; i<inumSecs; i++)	{
		if( disc_CacheReadSector( &p[i*512], sector + i ) == false )
			return false;
	}
	return true;
#else
	if (active_interface) return active_interface->fn_ReadSectors(sector,numSecs,buffer) ;
	return false ;
#endif
} 

bool disc_WriteSectors(u32 sector, u8 numSecs, void* buffer) 
{
#ifdef DISC_CACHE
	u8 *p=(u8*)buffer;
	u32 i;
	u32 inumSecs=numSecs;
	if(numSecs==0)
		inumSecs=256;
	for( i = 0; i<inumSecs; i++)	{
		if( disc_CacheWriteSector( &p[i*512], sector + i ) == false )
			return false;
	}
	return true;
#else
	if (active_interface) return active_interface->fn_WriteSectors(sector,numSecs,buffer) ;
	return false ;
#endif
} 

bool disc_ClearStatus(void) 
{
	if (active_interface) return active_interface->fn_ClearStatus() ;
	return false ;
} 

bool disc_Shutdown(void) 
{
#ifdef DISC_CACHE
	disc_CacheFlush();
#endif
	if (active_interface) active_interface->fn_Shutdown() ;
	active_interface = 0 ;
	return true ;
} 

u32	disc_HostType (void)
{
	if (active_interface) {
		return active_interface->ul_ioType;
	} else {
		return 0;
	}
}

