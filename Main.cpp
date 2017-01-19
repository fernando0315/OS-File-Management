#include <iostream>
#include <string>
#include "FileDirectory.h"

using namespace std;

int main()
{
	FileDirectory f1;
	char data[500];

	if (f1.create("file1", 4))
		f1.write("file1", "txt", 4, "1111", 1994, 10, 15, 18, 45, 35);

	strcpy_s(data, string(20, '2').c_str());
	if (f1.create("file2", 20))
		f1.write("file2", "txt", 20, data, 1994, 10, 15, 18, 45, 35);

	strcpy_s(data, string(30, '3').c_str());
	if (f1.create("file3", 30))
		f1.write("file3", "txt", 30, data, 1994, 10, 15, 18, 45, 35);

	strcpy_s(data, string(50, '4').c_str());
	if (f1.create("file4", 50))
		f1.write("file4", "txt", 50, data, 1994, 10, 15, 18, 45, 35);
	
	f1.deleteFile("file2");

	if (f1.create("file4", 50))
		f1.write("file4", "txt", 50, data, 1994, 10, 15, 18, 45, 35);

	f1.printDirectory();

	//f1.printClusters("file4");
	//f1.read("name");
	//cout << endl;
	
	//f1.deleteFile("halo");
	//f1.printClusters("name");
	//f1.write("test", "cpp", 9, "abcdefghi", 1994, 06, 15, 15, 45, 35);
	//f1.read("file2");
	//f1.read("halo");
	
	return 0;

}