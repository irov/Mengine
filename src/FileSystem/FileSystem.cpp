#	include "FileSystem.h"

#	include "FileData.h"
#	include "ZipArch.h"

#	include <boost/functional.hpp>

#	include <algorithm>

#	include <direct.h>
#	include <assert.h>

bool initInterfaceSystem(FileSystemInterface** _ptrFileSystem)
{
	try
	{
		*_ptrFileSystem = new FileSystem();
	}
	catch (...)
	{
		return false;
	}
	
	return true;
}

void releaseInterfaceSystem(FileSystemInterface* _ptrFileSystem)
{
	delete	static_cast<FileSystem*>(_ptrFileSystem);
}

FileSystem::FileSystem()
{
	mZipArchives.clear();
}

FileSystem::~FileSystem()
{
	for (std::vector<ZipArchive*>::const_iterator j = mZipArchives.begin(); j != mZipArchives.end(); ++j)
	{
		delete (*j);
	}
	mZipArchives.clear();
}

bool FileSystem::createFolder(const std::string& _foldername)
{
	if (_foldername.empty())
	{
		return false;
	}
	if (_mkdir(_foldername.c_str()) < 0)
	{
		return false;
	}
  	return true;
}
  
bool	FileSystem::loadPak(const std::string& _filename, int _prior)
{
	assert(!_filename.empty());

	std::ifstream f(_filename.c_str(), std::ios::binary);

	if (f.is_open()) 
	{ 
		f.close();
		mZipArchives.push_back(new ZipArchive(_filename,_prior));
		struct PriorSort{	
			bool operator()(ZipArchive*& lhs, ZipArchive*& rhs){
				return lhs->GetPriority() > rhs->GetPriority();
			}
		};
		std::sort(mZipArchives.begin(),mZipArchives.end(),PriorSort());
		return true;
	}
	return false;
} 

bool	FileSystem::unloadPak(const std::string& _filename)
{
	assert(!_filename.empty());
	for(std::vector<ZipArchive*>::const_iterator i = mZipArchives.begin(); i != mZipArchives.end(); ++i)
	{
		if((*i)->GetZipName() == _filename)
		{
			(*i)->UnloadZip();
			return true;
		}
	}
	return false;
}

bool	FileSystem::existFile(const std::string& _filename)
{
	assert(!_filename.empty());

	std::vector<ZipArchive*>::const_iterator i = std::find_if(
		mZipArchives.begin(), 
		mZipArchives.end(),
		boost::bind2nd(boost::mem_fun(&ZipArchive::HaveFile),_filename)
		);

	if(i != mZipArchives.end())
	{
		return true;
	}

	static char buff[_MAX_PATH];
	if(!_getcwd(buff, _MAX_PATH))
	{
		assert(!"_getcwd error");
		return false;
	}

	strcat_s(buff,"\\");
	strcat_s(buff,_filename.c_str());

	std::ifstream	f(buff, std::ios::binary);

	if (f.is_open()) 
	{
		f.close();
		return true;
	}

	return false;
}

void FileSystem::closeFile(FileDataInterface* _fd)
{
	delete	static_cast<FileData*>(_fd);
}

FileDataInterface*	FileSystem::openFile(const std::string& _filename)
{
	assert(!_filename.empty());

	std::vector<ZipArchive*>::const_iterator i = std::find_if(
		mZipArchives.begin(), 
		mZipArchives.end(),
		boost::bind2nd(boost::mem_fun(&ZipArchive::HaveFile),_filename)
		);

	if(i != mZipArchives.end())
	{
		return (*i)->FileRead(_filename);
	}

	static char buff[_MAX_PATH];

	if(!_getcwd(buff, _MAX_PATH))
	{
		assert(!"_getcwd error");
		return false;
	}
 
	strcat_s(buff,"\\");
	strcat_s(buff,_filename.c_str());
	 
	std::ifstream	f(buff,std::ios::binary);

	if(!f.is_open())
	{
		return NULL;
	}

	f.exceptions(std::ios_base::badbit | std::ios_base::failbit | std::ios_base::eofbit );

  	FileDataInterface* tempFileData = new FileData(f);

	f.close();
	return tempFileData;
}