#include "FileDirectory.h"
#include <iostream>
#include <iomanip>
#include <bitset>

using namespace std;

FileDirectory::FileDirectory()
{
	for (unsigned short int &sector : FAT16)
		sector = 0;

	for (unsigned char &character : data)
		character = 0;

	for (int i = 0; i < 4; i++)
		for (int j = 0; j < 32; j++)
			fileDirectory[i][j] = 0;

	for (auto &str : fileDirectory)
		for(auto &chr : str)
			chr = 0;
}

bool FileDirectory::create(char filename[], char ext[], int numberBytes)
{
	bool unusedEntry = false;
	bool memoryIsFull = false;
	int allocatedMemory = 0;
	int size;
	int j;

	//Check if there are still space for file directory
	for (int i = 0; i < 4; i++) 
	{
		if (fileDirectory[i][0] == 0)
		{
			unusedEntry = true;
		}
		else
		{
			//Compare the filename
			size = strlen(filename);

			for (j = 0; j < size; j++) 
				if (filename[j] != fileDirectory[i][j]) break;

			if (j == size)
			{
				//compare the file extension
				char extension[4] = "\0";
				for (int k = 0; k < 3; k++)
					extension[k] = fileDirectory[i][8 + k];

				//Exit if filename exist in directory
				if (strcmp(extension, ext) == 0)
				{
					cout << "This directory already contains a file name '" 
						<< filename << '.' << ext << '\'' << endl << endl;
					return false;
				}
			}
		}
	}
	
	//Check if there are space for dataFile
	unsigned short int numOfNeededSector = numberBytes / 4;
	numOfNeededSector += numberBytes % 4 == 0 ? 0 : 1;
	unsigned short int availableSector = 0;
	
	if (unusedEntry)
		for (unsigned short int &sector : FAT16)
		{
			if (sector == 0 || sector == 1)
			{
				if (++availableSector > numOfNeededSector)
					return true;
			}
		}
	else
	{
		cout << "There are no more space inside the directory.\n" << endl;
		return false;
	}
	cout << "There are not enough memory space for \"" << filename 
		<< '.' << ext << "'."<< endl << endl;

	return false;
}

bool FileDirectory::read(char filename[], char ext[])
{
	unsigned short int i;

	if (isFound(filename, ext, i))
	{
		//Print date
		unsigned short int date;
		unsigned int year, month, day;
		date = (fileDirectory[i][25] << 8) + fileDirectory[i][24];
		day = date & 0x001F;
		month = (date & 0x01E0) >> 5;
		year = ((fileDirectory[i][25] & 0xFE) >> 1) + 1980;
		cout << setfill('0') << setw(2) << month << '/' << day << '/' << year << '\t';
		//Print time
		unsigned short int time;
		time = (fileDirectory[i][23] << 8) + fileDirectory[i][22];
		cout << setw(2) << setfill('0') << right << ((time & 0xF800) >> 11);
		cout << ":";
		cout << setw(2) << setfill('0') << right << ((time & 0x07E0) >> 5);
		cout << ":";
		cout << setw(2) << setfill('0') << right << ((time & 0x001F) << 1);
		cout << "\t";
		//Print data size
		unsigned int numberBytes;
		numberBytes = fileDirectory[i][31];
		numberBytes <<= 8;
		numberBytes += fileDirectory[i][30];
		numberBytes <<= 8;
		numberBytes += fileDirectory[i][29];
		numberBytes <<= 8;
		numberBytes += fileDirectory[i][28];
		cout << numberBytes << '\t';
		//print filename + ext
		for (int j = 0; j < 8 && fileDirectory[i][j] != 0; j++) cout << fileDirectory[i][j];
		cout << '.';
		for (int j = 8; j < 11; j++) cout << fileDirectory[i][j];

		cout << endl;
		printClusters(filename, ext);
		printData(filename, ext);

		return true;
	}

	return false;
}

void FileDirectory::write(char filename[], char ext[], int numberBytes, char dat[], int year,
	int month, int day, int hour, int minute, int second)
{
	int emptyIndex;

	//Look for the exmpty location at fileDirectory
	for (emptyIndex = 0; emptyIndex < 4; emptyIndex++)
		if (fileDirectory[emptyIndex][0] == 0)
			break;

	//Look for the next unused entry (0 or 1) in the FAT - 16
	unsigned short int firstClusterAddress;
	unsigned short int allocatedBytes;
	unsigned short int allocatedDataIndex, emptyDataIndex;
	
	allocatedDataIndex = 0;

	for (int i = 2; i < 256; i++)
	{
		if (FAT16[i] == 0 || FAT16[i] == 1)
		{
			firstClusterAddress = i;
			break;
		}
	}

	emptyDataIndex = firstClusterAddress * 4;
	FAT16[firstClusterAddress] = 255;
	for (int i = 0; i < 4 && allocatedDataIndex < numberBytes; i++, emptyDataIndex++)
		data[emptyDataIndex] = dat[allocatedDataIndex++];

	//seach until all clusters are found
	allocatedBytes = 4;
	unsigned short int nextClusterAddress, currentClusterAddress;

	currentClusterAddress = firstClusterAddress;

	while (allocatedBytes < numberBytes)
	{
		for (int i = firstClusterAddress + 1; i < 256; i++)
		{
			if (FAT16[i] == 0 || FAT16[i] == 1)
			{
				nextClusterAddress = i;
				break;
			}
		}

		FAT16[currentClusterAddress] = nextClusterAddress;
		currentClusterAddress = nextClusterAddress;
		FAT16[currentClusterAddress] = 255;

		emptyDataIndex = currentClusterAddress * 4;
		for (int i = 0; i < 4 && allocatedDataIndex < numberBytes; i++, emptyDataIndex++)
		{
			data[emptyDataIndex] = dat[allocatedDataIndex++];
		}
		
		allocatedBytes += 4;
	}
	
	//Write name first letter in low byte
	for (int j = 0; j < 8; j++) fileDirectory[emptyIndex][j] = filename[j];
	//write ext [8:10]
	for (int j = 8; j < 11; j++) fileDirectory[emptyIndex][j] = ext[j-8];
	//write date into directory [25:24], input year first
	unsigned short int date;
	date = year - 1980;
	date <<= 4;
	date |= month;
	date <<= 5;
	date |= day;
	fileDirectory[emptyIndex][25] = date >> 8;
	fileDirectory[emptyIndex][24] = date;
	//write time [23:22]
	unsigned short int time;
	time = hour;
	time <<= 6;
	time |= minute;
	time <<= 5;
	time |= second / 2;
	fileDirectory[emptyIndex][23] = time >> 8;
	fileDirectory[emptyIndex][22] = time;
	//write first sector address [27:26]
	fileDirectory[emptyIndex][27] = firstClusterAddress >> 8;
	fileDirectory[emptyIndex][26] = firstClusterAddress;
	//write file length into directory[31:28]
	fileDirectory[emptyIndex][31] = numberBytes >> 24;
	fileDirectory[emptyIndex][30] = numberBytes >> 16;
	fileDirectory[emptyIndex][29] = numberBytes >> 8;
	fileDirectory[emptyIndex][28] = numberBytes;
}

bool FileDirectory::deleteFile(char filename[], char ext[]) {
	unsigned short int index;

	if (isFound(filename, ext, index))		//if file is found
	{
		//clear sector
		unsigned short int delSecAddress, nextSecAddress;
		delSecAddress = fileDirectory[index][27];
		delSecAddress <<= 8;
		delSecAddress = fileDirectory[index][26];

		while (delSecAddress != 255)
		{
			nextSecAddress = FAT16[delSecAddress];
			FAT16[delSecAddress] = 0;
			delSecAddress = nextSecAddress;
		}

		//clear directory
		for (int j = 0; j < 32; j++)
			fileDirectory[index][j] = 0;
	}
	else
	{
		cout << "filename not found!" << endl;
	}

	return true;
}


void FileDirectory::printClusters(char filename[], char ext[]) {
	unsigned short int index;

	if (isFound(filename, ext, index))		//if file is found
	{
		unsigned short int secAddress;
		secAddress = fileDirectory[index][27];
		secAddress <<= 8;
		secAddress = fileDirectory[index][26];

		while (true)
		{
			cout << secAddress;
			secAddress = FAT16[secAddress];
			if (secAddress != 255)
			{
				cout << "->";
			}
			else
			{
				cout << endl;
				break;
			}
		}
	}
	else
	{
		cout << "filename not found!" << endl;
	}
}

void FileDirectory::printDirectory() 
{
	cout << "Print Directory:" << endl;

	for (int i = 0; i < 4; i++)
	{
		if (fileDirectory[i][0] != 0)
		{
			//Print date
			unsigned short int date;
			unsigned int year, month, day;
			date = (fileDirectory[i][25] << 8) + fileDirectory[i][24];
			day = date & 0x001F;
			month = (date & 0x01E0) >> 5;
			year = ((fileDirectory[i][25] & 0xFE) >> 1) + 1980;
			cout << setfill('0') << setw(2) << month << '/' << day << '/' << year << '\t';

			//Print time
			unsigned short int time;
			time = (fileDirectory[i][23] << 8) + fileDirectory[i][22];
			cout << setw(2) << setfill('0') << right << ((time & 0xF800) >> 11);
			cout << ":";
			cout << setw(2) << setfill('0') << right << ((time & 0x07E0) >> 5);
			cout << ":";
			cout << setw(2) << setfill('0') << right << ((time & 0x001F) << 1);
			cout << "\t";

			//Print data size
			unsigned int numberBytes;
			numberBytes = fileDirectory[i][31];
			numberBytes <<= 8;
			numberBytes += fileDirectory[i][30];
			numberBytes <<= 8;
			numberBytes += fileDirectory[i][29];
			numberBytes <<= 8;
			numberBytes += fileDirectory[i][28];
			cout << numberBytes << '\t';

			//print filename + ext
			for (int j = 0; j < 8 && fileDirectory[i][j] != 0; j++) cout << fileDirectory[i][j];
			cout << '.';
			for (int j = 8; j < 11; j++) cout << fileDirectory[i][j];
			cout << endl;
		}
	}
}

void FileDirectory::printData(char filename[], char ext[]) {

	/*
	for (int i = 0; i < 256; i++) 
			cout << setw(3) << setfill(' ') << left << i << "| " << data[i] << endl;
	*/
	
	unsigned short int index;

	if (isFound(filename, ext, index))		//if file is found
	{
		cout << "Data at \"" << filename << "\":" << endl;

		unsigned short int secAddress;
		secAddress = fileDirectory[index][27];
		secAddress <<= 8;
		secAddress = fileDirectory[index][26];

		while (secAddress != 255)
		{
			//cout << secAddress;
			for (int i = 0; i < 4; i++)
				cout << data[secAddress * 4 + i];

			secAddress = FAT16[secAddress];
		}

		cout << endl;
	}
	else
	{
		cout << "filename not found!" << endl;
	}

	cout << endl;
}

//Private function
bool FileDirectory::isFound(char filename[], char ext[], unsigned short int &index)
{
	int size = strlen(filename);
	int j;

	for (index = 0; index < 4; index++)
	{
		//check for filename
		for (j = 0; j < size; j++)
			if (filename[j] != fileDirectory[index][j]) break;

		if (j == size) 	
		{
			//compare the file extension
			char extension[4] = "\0";
			for (int k = 0; k < 3; k++)
				extension[k] = fileDirectory[index][8 + k];

			//Exit if filename exist in directory
			if (strcmp(extension, ext) == 0)
			{
				cout << "This directory already contains a file name '"
					<< filename << '.' << ext << '\'' << endl << endl;
				return true;
			}
		}
	}

	return false;
}