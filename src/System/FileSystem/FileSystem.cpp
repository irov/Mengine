#	include "FileSystem.h"

#	include "FileData.h"
#	include "ZipArch.h"

#	include <functional>
#	include <algorithm>
#	include <direct.h>
#	include <assert.h>

#	pragma comment (lib, ".\\zlib\\zlibstat.lib")

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
	m_zipArchives.clear();
}

FileSystem::~FileSystem()
{
	for (TVecZipArch::const_iterator j = m_zipArchives.begin(); j != m_zipArchives.end(); ++j)
	{
		delete (*j);
	}
	m_zipArchives.clear();
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
		m_zipArchives.push_back(new ZipArchive(_filename,_prior));
		struct PriorSort{	
			bool operator()(ZipArchive*& lhs, ZipArchive*& rhs){
				return lhs->getPriority() > rhs->getPriority();
			}
		};
		std::sort(m_zipArchives.begin(),m_zipArchives.end(),PriorSort());
		return true;
	}
	return false;
} 

bool	FileSystem::unloadPak(const std::string& _filename)
{
	assert(!_filename.empty());
	for(TVecZipArch::const_iterator i = m_zipArchives.begin(); i != m_zipArchives.end(); ++i)
	{
		if((*i)->getZipName() == _filename)
		{
			(*i)->unloadZip();
			return true;
		}
	}
	return false;
}

bool	FileSystem::existFile(const std::string& _filename)
{
	assert(!_filename.empty());

	TVecZipArch::const_iterator i = std::find_if(
		m_zipArchives.begin(), 
		m_zipArchives.end(),
		std::bind2nd(std::mem_fun(&ZipArchive::haveFile),_filename.c_str())
		);

	if(i != m_zipArchives.end())
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

	TVecZipArch::const_iterator i = std::find_if(
		m_zipArchives.begin(), 
		m_zipArchives.end(),
		std::bind2nd(std::mem_fun(&ZipArchive::haveFile),_filename.c_str())
		);

	if(i != m_zipArchives.end())
	{
		return (*i)->fileRead(_filename);
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