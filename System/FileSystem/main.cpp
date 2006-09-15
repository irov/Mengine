#include "FileSystem.h"

int main()
{

	FileSystemInterface* fs;
	initFileSystem(&fs);
	//bool is_ok = fs->createFolder( "D:\\2934");
 
	if(!fs->loadPak( "C:\\just smth.zip", 1))
	{
	

	}
	
	if(!fs->loadPak( "C:\\6s.zip", 75))
	{
	
	}

	if(!fs->loadPak( "C:\\6s.zip", 5))
	{
	
	}


	FileDataInterface * first = fs->openFile("vldmtdll.lib");
	bool is_ok = fs->existFile("1.txt");
	FileDataInterface * second = fs->openFile("2.txt");
	is_ok = fs->existFile("2.txt");
	fs->closeFile(first);
	fs->closeFile(second);

	releaseFileSystem(fs);

	return 0;
};