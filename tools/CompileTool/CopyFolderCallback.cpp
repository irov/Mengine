#	include "CopyFolderCallback.h"
#	include "Menge\Utils.h"

#	include <atlstr.h>
#	include <algorithm>

#	include "CharConvUtils.h"

//////////////////////////////////////////////////////////////////////////
CopyFolderCallback::CopyFolderCallback()
{}
//////////////////////////////////////////////////////////////////////////
CopyFolderCallback::~CopyFolderCallback()
{}
//////////////////////////////////////////////////////////////////////////
void CopyFolderCallback::pushExtFilter( const Menge::String & _extension )
{
	m_extFilter.push_back( _extension );
	std::sort( m_extFilter.begin(), m_extFilter.end() );
}
//////////////////////////////////////////////////////////////////////////
void CopyFolderCallback::pushFileFilter( const Menge::String & _file )
{
	m_fileFilter.push_back( _file );
	std::sort( m_fileFilter.begin(), m_fileFilter.end() );
}
//////////////////////////////////////////////////////////////////////////
void CopyFolderCallback::pushFolderFilter( const Menge::String & _folder )
{
	m_folderFilter.push_back( _folder );
	std::sort( m_folderFilter.begin(), m_folderFilter.end() );
}
//////////////////////////////////////////////////////////////////////////
bool CopyFolderCallback::isExtFilter( const Menge::String & _extension )
{
	return std::binary_search( m_extFilter.begin(), m_extFilter.end(), _extension );
}
//////////////////////////////////////////////////////////////////////////
bool CopyFolderCallback::isFileFilter( const Menge::String & _file )
{
	return std::binary_search( m_fileFilter.begin(), m_fileFilter.end(), _file );
}
//////////////////////////////////////////////////////////////////////////
bool CopyFolderCallback::isFolderFilter( const Menge::String & _folder )
{
	return std::binary_search( m_folderFilter.begin(), m_folderFilter.end(), _folder );
}
//////////////////////////////////////////////////////////////////////////
bool CopyFolderCallback::execute( const std::string & _src, const std::string & _dst, Compiler * )
{
	Menge::StringW src_wchar = CharConvUtils::UTF8ToWChar("..\\" + _src);
	Menge::StringW dst_wchar = CharConvUtils::UTF8ToWChar(_dst);

	bool result = _recursiveCopyFolder( src_wchar, dst_wchar );

	return result;
}
//////////////////////////////////////////////////////////////////////////
Menge::String CopyFolderCallback::_getExtension( const Menge::String & _filename )
{
	size_t pos = _filename.find_last_of(".");

	if( pos != Menge::String::npos )
	{
		return _filename.substr(pos,_filename.length());
	}

	return Menge::Utils::emptyString();
}
//////////////////////////////////////////////////////////////////////////
bool CopyFolderCallback::_recursiveCopyFolder( Menge::StringW _path, Menge::StringW _newPath )
{
	BOOL result = TRUE;

	int res = _wmkdir( _newPath.c_str() );

	if( !res )
	{
		result = FALSE;
	}

	_path     += _T("\\");
	_newPath  += _T("\\");
    
	Menge::StringW pathMask = _path + _T("*.*");
        
	WIN32_FIND_DATA ffData  = {};

	HANDLE hFind = FindFirstFile( pathMask.c_str(), &ffData );

	if ( hFind == INVALID_HANDLE_VALUE )
	{
		return FALSE;
	}

	Menge::StringW fullPath;
	Menge::StringW newFullPath;

	while (hFind && FindNextFile(hFind, &ffData)) 
	{
		fullPath    = _path    + ffData.cFileName;
		newFullPath = _newPath + ffData.cFileName;

		bool isFile = !(ffData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY);

		if( isFile ) 
		{
			if( isFileFilter( CharConvUtils::WCharToAnsi(ffData.cFileName)) )
			{
				continue;
			}

			Menge::String extension = _getExtension(CharConvUtils::WCharToAnsi(ffData.cFileName));

			if( isExtFilter(extension) )
			{
				continue;
			}

			BOOL isCopyOk = CopyFile( fullPath.c_str(), newFullPath.c_str(), FALSE );
			result = isCopyOk;
		}
		else 
		{ 
			if( (_tcscmp(ffData.cFileName, _T(".")) != 0) &&
				(_tcscmp(ffData.cFileName, _T("..")) != 0) ) 
			{
				if( isFolderFilter(CharConvUtils::WCharToAnsi(ffData.cFileName)) )
				{
					continue;
				}

				BOOL isCopyOk = _recursiveCopyFolder(fullPath, newFullPath);
				result = isCopyOk;
			}
		}
	}

	FindClose( hFind );

	return result == TRUE;
}
//////////////////////////////////////////////////////////////////////////