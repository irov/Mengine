#include "FileSystem.h"

#	include "ZipArch.h"

int main()
{
	ZipArchive za("C:\\swept.zip",1);


	FileSystemInterface* fs;
	initInterfaceSystem(&fs);
	//bool is_ok = fs->createFolder( "D:\\2934");
 
	if(!fs->loadPak( "C:\\swept.zip", 1))
	{
	

	}
	

	FileDataInterface * first = fs->openFile("3D swept.doc");

	bool is_ok = fs->existFile("1.txt");
	FileDataInterface * second = fs->openFile("2.txt");
	is_ok = fs->existFile("2.txt");
	fs->closeFile(first);
	fs->closeFile(second);

	releaseInterfaceSystem(fs);

	return 0;
};