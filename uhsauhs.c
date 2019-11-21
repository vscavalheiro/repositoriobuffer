
int createNewFile(char* filename, struct t2fs_record* record, int type);

int createNewFile(char* filename, struct t2fs_record* record, int type) {

	if (mounted_partition == -1) {
		return ERROR;
	}

	int Inode = allocBlockOrInode(0, mounted_partition);
	if(Inode < 0) {
		return Inode;
	}
	
	struct t2fs_record newRecord = {
		.TypeVal = type,
		.inodeNumber = Inode
	};
	
	strcpy(newRecord.name, filename);
	

	struct t2fs_inode newInode = {
		.blocksFileSize = 0,
		.bytesFileSize = 0,
		.dataPtr = { 0 },
		.singleIndPtr = 0,
		.doubleIndPtr = 0,
		.RefCounter = 0
	};

	int erro = 0;
	if ((erro = writeInode(Inode, newInode, mounted_partition)))
		return erro;

	if ((erro = writeDirEntry(newRecord)))
		return erro;

	//DEBUG("#ERRO findFileByName: arquivo nao encontrado\n");
	*record = newRecord;

	return 0;
}



//----------------------------------------------------------------------------------------------------
int writeDirEntry(struct t2fs_record record) {
	if (mounted_partition == -1) {
		return ERROR;
	}

	struct t2fs_inode* inode;
	inode = readInode(0, mounted_partition);

	

	int erro = 0;
	if (!inode.blocksFileSize || !(inode.bytesFileSize % (inode.blocksFileSize * 256 * SUPER_BLOCO[mounted_partition].blockSize))) {
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

	DWORD indiceDir = (inode.bytesFileSize - ((inode.blocksFileSize - 1) * 256 * SUPER_BLOCO[mounted_partition].blockSize)) / sizeof(struct t2fs_record);

	unsigned char* buffer = (unsigned char*)malloc(256 * SUPER_BLOCO[mounted_partition].blockSize);

	int indx = 0;
	if ((indx = readBlockFromInode(inode.blocksFileSize - 1, inode, SUPER_BLOCO[mounted_partition].blockSize, mounted_partition, buffer)) < 0) {
		return indx;
	}

	struct t2fs_record* tmpArray = (struct t2fs_record*)buffer;
	tmpArray[indiceDir] = record;
	inode.bytesFileSize += sizeof(struct t2fs_record);

	DWORD setor_inicial = 0;
	partitionSectors(mounted_partition, &setor_inicial, NULL);
	DWORD writeIndex = setor_inicial + indx * SUPER_BLOCO[mounted_partition].blockSize;

	for (int i = 0; i < SUPER_BLOCO[mounted_partition].blockSize; i++)
		write_sector(writeIndex + i, &buffer[i * 256]);
	if ((erro = writeInode(0, inode, mounted_partition))) {
		return erro;
	}

	free(buffer);

	return 0;
}

