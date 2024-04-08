
u8 FAT_GetAttrib (void)
{
	// Read in the last accessed directory entry
	disc_ReadSector ((wrkDirCluster == FAT16_ROOT_DIR_CLUSTER ? filesysRootDir : FAT_ClustToSect(wrkDirCluster)) + wrkDirSector, globalBuffer);
	
	return 	((DIR_ENT*)globalBuffer)[wrkDirOffset].attrib;
}

