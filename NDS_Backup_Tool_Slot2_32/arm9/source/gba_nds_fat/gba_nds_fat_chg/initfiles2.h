
	// FAT area all cached.
	if(filesysType==FS_FAT16){
	  u8 *buf=(u8*)fatDumpBuffer;
	  int idx;
	  for(idx=0;idx<(0x10000*sizeof(u16))/BYTE_PER_READ;idx++){
	    disc_ReadSectors(filesysFAT+idx,1,buf);
	    buf+=BYTE_PER_READ;
	  }
	}
