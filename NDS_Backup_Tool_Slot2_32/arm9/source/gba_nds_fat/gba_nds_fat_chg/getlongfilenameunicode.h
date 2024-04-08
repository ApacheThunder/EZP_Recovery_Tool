/*-----------------------------------------------------------------
FAT_GetLongFilenameUnicode
Get the long name of the last file or directory retrived with 
	GetDirEntry. Also works for FindFirstFile and FindNextFile
char* filename: OUT will be filled with the filename, should be at
	least 256 bytes long
bool return OUT: return true if successful
-----------------------------------------------------------------*/
bool FAT_GetLongFilenameUnicode (Tunicode* filename,int MaxCount)
{
	// already exists lfn array.
//	if (!lfnExists) return false;
	
	if (filename == NULL) return false;
	
	int i;
	for (i =0; (i < MAX_FILENAME_LENGTH - 1) && (i < MaxCount - 1) && (lfnNameUnicode[i] != 0); i++)
	{
		filename[i] = lfnNameUnicode[i];
	}
	filename[i] = 0;
	
	return true;
}
