class FileDirectory {
public:
	FileDirectory();
	bool create(char filename[], char ext[], int numberBytes);
	bool deleteFile(char filename[]);
	bool read(char filename[]);
	void write(char filename[], char ext[], int numberBytes, char dat[], int year, int month, int day,
		       int hour, int minute, int second);
	void printClusters(char filename[]);
	void printDirectory();
	void printData(char filename[]);

private:
	unsigned char fileDirectory[4][32];
	/*a max of 4 files directories . Each has 32 bytes
	Filename[7:0]					8-byte
	FIlename ext[8:10]				3-byte
	Date[25:24]
	Time[23:22]
	16-bit vector address[27:26]	2-byte
	32-bit File size[31-28]			file size up to 4Gb
	The remaining bytes are for other File attributes and future expansion.*/
	unsigned short int FAT16[256];	//256 clusters only
	unsigned char data[1024];		//256 clusters * 4 bytes/cluster = 1,024 bytes

	bool isFound(char filename[], unsigned short int &index);
};