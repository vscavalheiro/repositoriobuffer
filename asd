int writeDirEntry(struct t2fs_record record) {
	if (mounted_partition == -1) {
		return ERROR;
	}
	struct t2fs_inode* inode;
	inode = readInode(0, mounted_partition);
	int erro = 0;
	if (!(inode->blocksFileSize) || !(inode->bytesFileSize % (inode->blocksFileSize * 256 * SUPER_BLOCO[mounted_partition].blockSize))) {
		// Alocar novo bloco
	int indexBlk = allocBlockOrInode(1, mounted_partition);
	if (indexBlk < 0) {
		return indexBlk;
	}
	//DEBUG("#INFO: indexBlk %u\n", indexBlk);
	if ((erro = addBlockOnInode(&inode, SUPER_BLOCO[mounted_partition].blockSize, indexBlk))) {
		return erro;
	}
	}
	DWORD indiceDir = (inode->bytesFileSize - ((inode->blocksFileSize - 1) * 256 * SUPER_BLOCO[mounted_partition].blockSize)) / sizeof(struct t2fs_record);
	unsigned char* buffer = (unsigned char*)malloc(256 * SUPER_BLOCO[mounted_partition].blockSize);
	int indx = 0;
	if ((indx = readBlockFromInode(inode->blocksFileSize - 1, inode, SUPER_BLOCO[mounted_partition].blockSize, mounted_partition, buffer)) < 0) {
		return indx;
	}
	struct t2fs_record* tmpArray = (struct t2fs_record*)buffer;
	tmpArray[indiceDir] = record;
	inode->bytesFileSize += sizeof(struct t2fs_record);
	DWORD setor_inicial = 0;
	partitionSectors(mounted_partition, &setor_inicial, NULL);
	DWORD writeIndex = setor_inicial + indx * SUPER_BLOCO[mounted_partition].blockSize;
	for (int i = 0; i < SUPER_BLOCO[mounted_partition].blockSize; i++)
		write_sector(writeIndex + i, &buffer[i * 256]);
	if ((erro = writeInode(0, *inode, mounted_partition))) {
		return erro;
	}
	free(buffer);
	return 0;
}



int opendir2(void){
	if(mounted_partition < 0){
		return -1;	
	}
	dirOpened =1;
	lastDirListed = 0;
	
	return 0;
}
int readdir2(DIRENT2* dentry)
{	return -1;
}
int closedir2(void){	
	if(mounted_partition < 0){
		return -1;	
	}
	dirOpened = 0;
	
	return 0;
}
