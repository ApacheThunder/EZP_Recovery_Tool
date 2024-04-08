
u32 FAT_fread_skip (u32 size, u32 count, FAT_FILE* file)
{
  void* buffer;
  u32 res;
  
  buffer=malloc(size*count);
  res=FAT_fread(buffer,size,count,file);
  free(buffer); buffer=NULL;
  
  return(res);
}

