#include <iostream>
#include "FileDirectory.h"

using namespace std;

int main()
{
	FileDirectory f1;

	
	f1.write("halo", "cpp", 3, "tje", 1994, 10, 15, 15, 45, 35);
	f1.write("name", "cpp", 5, "farad", 2015, 8, 20, 15, 45, 35);
	//f1.write("gerr", "cpp", 5, "tesrt", 1994, 06, 15, 15, 45, 35);
	//f1.write("reyh", "cpp", 5, "tesrt", 1994, 06, 15, 15, 45, 35);
	
	f1.printDirectory();
	cout << endl;

	f1.read("name");
	cout << endl;
	
	f1.deleteFile("halo");
	//f1.printClusters("name");
	f1.write("test", "cpp", 9, "abcdefghi", 1994, 06, 15, 15, 45, 35);
	f1.read("test");
	//f1.read("halo");
	
	return 0;

}