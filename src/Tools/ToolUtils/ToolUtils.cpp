#	include "ToolUtils.h"

//////////////////////////////////////////////////////////////////////////
void message_error( const char * _format, ... )
{
	va_list argList;

	va_start( argList, _format );

	char str[2048];

	vsnprintf( str, 2048 - 1, _format, argList );

	va_end( argList );

	printf( str );
}
//////////////////////////////////////////////////////////////////////////
void parse_arg( const std::wstring & _str, bool & _value )
{
	uint32_t value;
	swscanf( _str.c_str(), L"%u", &value );

	_value = (value != 0);
}
//////////////////////////////////////////////////////////////////////////
void parse_arg( const std::wstring & _str, uint32_t & _value )
{
	uint32_t value;
	swscanf( _str.c_str(), L"%u", &value );

	_value = value;
}
//////////////////////////////////////////////////////////////////////////
void parse_arg( const std::wstring & _str, float & _value )
{
	float value;
	swscanf( _str.c_str(), L"%f", &value );

	_value = value;
}
//////////////////////////////////////////////////////////////////////////
void parse_arg( const std::wstring & _str, double & _value )
{
	double value;
	swscanf( _str.c_str(), L"%lf", &value );

	_value = value;
}
//////////////////////////////////////////////////////////////////////////
void parse_arg( const std::wstring & _str, std::wstring & _value )
{
	_value = _str;
}
//////////////////////////////////////////////////////////////////////////
void ForcePathQuoteSpaces( WCHAR * _quotePath, const std::wstring & _path )
{
	if( _path.empty() == true )
	{
		wcscpy_s( _quotePath, 2, L"" );

		return;
	}

	std::wstring true_path = _path;

	if( _path[0] == L'/' )
	{
		true_path[0] = true_path[1];
		true_path[1] = L':';
	}

	const WCHAR * pathBuffer = true_path.c_str();
	size_t pathSize = true_path.size();

	PathCanonicalize( _quotePath, pathBuffer );
	if( PathQuoteSpaces( _quotePath ) == FALSE )
	{
		wmemmove( _quotePath + 1, _quotePath, pathSize );
		_quotePath[0] = '\"';
		_quotePath[pathSize + 1] = '\"';
		_quotePath[pathSize + 2] = 0;
	}
};
//////////////////////////////////////////////////////////////////////////
int ForceRemoveDirectory( LPCTSTR dir )
{
	size_t len = wcslen( dir ) + 2; // required to set 2 nulls at end of argument to SHFileOperation.
	wchar_t * tempdir = (wchar_t*)malloc( len * sizeof( wchar_t ) );
	memset( tempdir, 0, len * sizeof( wchar_t ) );
	wcscpy( tempdir, dir );

	SHFILEOPSTRUCT file_op = {
		NULL,
		FO_DELETE,
		tempdir,
		L"",
		FOF_NOCONFIRMATION |
		FOF_NOERRORUI |
		FOF_SILENT,
		false,
		0,
		L"" };

	int ret = SHFileOperation( &file_op );

	free( tempdir );

	return ret;
}
//////////////////////////////////////////////////////////////////////////
bool SelectFile( LPCTSTR _dir, Files & _files )
{
	WIN32_FIND_DATA fd;

	HANDLE hFind = ::FindFirstFile( _dir, &fd );

	if( hFind == INVALID_HANDLE_VALUE )
	{
		return false;
	}
	
	do
	{
		if( fd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY )
		{
			continue;
		}

		_files.push_back( fd.cFileName );
	} while( ::FindNextFile( hFind, &fd ) );

	::FindClose( hFind );
	
	return true;
}
