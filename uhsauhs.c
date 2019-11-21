
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
