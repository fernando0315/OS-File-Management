#include <iostream>
#include <string>
#include "FileDirectory.h"

using namespace std;

int main()
{
	FileDirectory f1;
	char data[500];
	unsigned int size;

	size = 40;
	strcpy_s(data, string(size, '2').c_str());
	if (f1.create("file1", size))
		f1.write("file1", "txt", size, data, 1994, 10, 15, 18, 45, 35);

	size = 200;
	strcpy_s(data, string(size, '2').c_str());
	if (f1.create("file2", size))
		f1.write("file2", "txt", size, data, 1994, 10, 15, 18, 45, 35);

	size = 300;
	strcpy_s(data, string(size, '3').c_str());
	if (f1.create("file3", size))
		f1.write("file3", "txt", size, data, 1994, 10, 15, 18, 45, 35);

	size = 500;
	strcpy_s(data, string(size, '4').c_str());
	if (f1.create("file4", size))
		f1.write("file4", "txt", size, data, 1994, 10, 15, 18, 45, 35);
	
	//f1.deleteFile("file2");

	strcpy_s(data, string(size, '4').c_str());
	if (f1.create("file4", size))
		f1.write("file4", "txt", size, data, 1994, 10, 15, 18, 45, 35);

	//f1.printData("");
	f1.printDirectory();

	//f1.printClusters("file2");
	//f1.read("file3");
	//cout << endl;
	
	//f1.deleteFile("halo");
	//f1.printClusters("name");
	//f1.write("test", "cpp", 9, "abcdefghi", 1994, 06, 15, 15, 45, 35);
	//f1.read("file2");
	//f1.read("halo");
	
	return 0;

}