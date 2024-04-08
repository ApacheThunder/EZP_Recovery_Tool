
static u32 ClusterBuffer[64*1024/4]; // max ClusterSize=64kb

static void memcopy(void *pdst,void *psrc,u32 len)
{
  u8 *pdst8=(u8*)pdst;
  u8 *psrc8=(u8*)psrc;
  
  u32 idx;
  for(idx=0;idx<len;idx++){
    pdst8[idx]=psrc8[idx];
  }
}

u32 FAT_fread (void* buffer, u32 size, u32 count, FAT_FILE* file)
{
	int curByte;
	int curSect;
	u32 curClus;
	u32 tempNextCluster;
	
	int tempVar;

	char* data = (char*)buffer;

	u32 length = size * count;
	u32 remain;

	bool flagNoError = true;

	// Can't read non-existant files
	if ((file == NULL) || (file->inUse == false) || size == 0 || count == 0 || buffer == NULL)
		return 0;

	// Can only read files openned for reading
	if (!file->read)
		return 0;

	// Don't read past end of file
	if (length + file->curPos > file->length)
		length = file->length - file->curPos;

	remain = length;

	curByte = file->curByte;
	curSect = file->curSect;
	curClus = file->curClus;

	// Align to sector
	tempVar = BYTE_PER_READ - curByte;
	if (tempVar > remain)
		tempVar = remain;

	if ((tempVar < BYTE_PER_READ) && flagNoError) 
	{
		memcopy(data, &(file->readBuffer[curByte]), tempVar);
		remain -= tempVar;
		data += tempVar;

		curByte += tempVar;
		if (curByte >= BYTE_PER_READ) 
		{
			curByte = 0;
			curSect++;
		}
	}

	// align to cluster
	// tempVar is number of sectors to read
	if (remain > (filesysSecPerClus - curSect) * BYTE_PER_READ) 
	{
		tempVar = filesysSecPerClus - curSect;
	} else {
		tempVar = remain / BYTE_PER_READ;
	}

	if ((tempVar > 0) && flagNoError)
	{
		if((((u32)data&1)==0)&&(0x02000000<=(u32)data)&&((u32)data<02400000)){
		  disc_ReadSectors ( curSect + FAT_ClustToSect(curClus), tempVar, data);
		  }else{
		  disc_ReadSectors ( curSect + FAT_ClustToSect(curClus), tempVar, ClusterBuffer);
		  memcopy(data, ClusterBuffer, tempVar * BYTE_PER_READ);
		}
		
		data += tempVar * BYTE_PER_READ;
		remain -= tempVar * BYTE_PER_READ;

		curSect += tempVar;
	}

	// Move onto next cluster
	// It should get to here without reading anything if a cluster is due to be allocated
	if (curSect >= filesysSecPerClus)
	{
		tempNextCluster = FAT_NextCluster(curClus);
		if ((remain == 0) && (tempNextCluster == CLUSTER_EOF))
		{
			curSect = filesysSecPerClus;
		} else {
			curSect = 0;
			curClus = tempNextCluster;
			if (curClus == CLUSTER_FREE)
			{
				flagNoError = false;
			}
		}
	}

	// Read in whole clusters
	while ((remain >= filesysBytePerClus) && flagNoError)
	{
		if((((u32)data&1)==0)&&(0x02000000<=(u32)data)&&((u32)data<02400000)){
		  disc_ReadSectors (FAT_ClustToSect(curClus), filesysSecPerClus, data);
		  }else{
		  disc_ReadSectors (FAT_ClustToSect(curClus), filesysSecPerClus, ClusterBuffer);
		  memcopy(data, ClusterBuffer, filesysBytePerClus);
		}
		
		data += filesysBytePerClus;
		remain -= filesysBytePerClus;

		// Advance to next cluster
		tempNextCluster = FAT_NextCluster(curClus);
		if ((remain == 0) && (tempNextCluster == CLUSTER_EOF))
		{
			curSect = filesysSecPerClus;
		} else {
			curSect = 0;
			curClus = tempNextCluster;
			if (curClus == CLUSTER_FREE)
			{
				flagNoError = false;
			}
		}
	}

	// Read remaining sectors
	tempVar = remain / BYTE_PER_READ; // Number of sectors left
	if ((tempVar > 0) && flagNoError)
	{
		if((((u32)data&1)==0)&&(0x02000000<=(u32)data)&&((u32)data<02400000)){
		  disc_ReadSectors (FAT_ClustToSect(curClus), tempVar, data);
		  }else{
		  disc_ReadSectors (FAT_ClustToSect(curClus), tempVar, ClusterBuffer);
		  memcopy(data, ClusterBuffer, tempVar * BYTE_PER_READ);
		}
		
		data += tempVar * BYTE_PER_READ;
		remain -= tempVar * BYTE_PER_READ;
		curSect += tempVar;
	}

	// Last remaining sector
	// Check if sector wanted is different to the one started with
	if ( ((file->curByte + length) >= BYTE_PER_READ) && flagNoError)
	{
		disc_ReadSector( curSect + FAT_ClustToSect( curClus), file->readBuffer);
		if (remain > 0)
		{
			memcopy(data, file->readBuffer, remain);
			curByte += remain;
			remain = 0;
		}
	}

	// Length read is the wanted length minus the stuff not read
	length = length - remain;

	// Update file information
	file->curByte = curByte;
	file->curSect = curSect;
	file->curClus = curClus;
	file->curPos = file->curPos + length;
	return length;
}
