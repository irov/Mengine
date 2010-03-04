
#	include <stdio.h>

#	include "MengeStdioWrapper.h"

#	include <cstring>
#	include <algorithm>

#	include "FileEngine.h"
#	include "Logger/Logger.h"
#	include "FileEngine.h"

namespace Menge
{
	static FILE* Menge_fopen_impl( const char* _filename, const char* _mode )
	{
		if( std::strchr( _mode, 'w' ) != NULL )
		{
			MENGE_LOG_ERROR( "Error: (Menge_fopen) invalid 'mode' '%s'"
				, _mode 
				);

			return NULL;
		}


		String filename( _filename );
		std::replace( filename.begin(), filename.end(), '\\', '/' );

		FileInputInterface* mengeFile = FileEngine::hostage()
									->openFileInput( "", filename );

		return reinterpret_cast<FILE*>( mengeFile );
	}

	static int Menge_fseek_impl( FILE* _file, long int _offset, int _origin )
	{
		FileInput* mengeFile = reinterpret_cast<FileInput*>( _file );
		if( mengeFile == NULL )
		{
			return 1;
		}
		int offsBegin = 0;
		if( _origin == SEEK_CUR )
		{
			offsBegin = mengeFile->tell();
		}
		else if( _origin == SEEK_END )
		{
			offsBegin = mengeFile->size();
		}
		mengeFile->seek( offsBegin + _offset );
		return 0;
	}

	static long int Menge_ftell_impl( FILE* _file )
	{
		FileInput* mengeFile = reinterpret_cast<FileInput*>( _file );
		if( mengeFile == NULL )
		{
			return -1L;
		}
		return mengeFile->tell();
	}

	static size_t Menge_fread_impl( void* _ptr, size_t _size, size_t _count, FILE* _file )
	{
		FileInput* mengeFile = reinterpret_cast<FileInput*>( _file );
		if( mengeFile == NULL )
		{
			return 0;
		}
		int bytesRead = 0;
		bytesRead = mengeFile->read( _ptr, _size * _count );
		return static_cast<size_t>( bytesRead );
	}

	static int Menge_fclose_impl( FILE* _file )
	{
		FileInput* mengeFile = reinterpret_cast<FileInput*>( _file );
		if( mengeFile == NULL )
		{
			return EOF;
		}
		FileEngine::hostage()
			->closeFileInput( mengeFile );
		return 0;
	}

	static int Menge_getc_impl( FILE* _file )
	{
		FileInput* mengeFile = reinterpret_cast<FileInput*>( _file );
		if( mengeFile == NULL )
		{
			return EOF;
		}
		char charbuf[1];
		int ret = mengeFile->read( charbuf, 1 );
		if( ret == 0 )
		{
			charbuf[0] = EOF;
		}
		return charbuf[0];
	}

	
} // namespace Menge

typedef FILE* (*Tfopen)( const char*, const char* );
Tfopen Menge_fopen_ptr = &Menge::Menge_fopen_impl;

typedef int (*Tfseek)( FILE*, long int, int );
Tfseek Menge_fseek_ptr = &Menge::Menge_fseek_impl;

typedef long int (*Tftell)( FILE* );
Tftell Menge_ftell_ptr = &Menge::Menge_ftell_impl;

typedef size_t (*Tfread)( void *, size_t, size_t, FILE * );
Tfread Menge_fread_ptr = &Menge::Menge_fread_impl;

typedef int (*Tfclose)( FILE * );
Tfclose Menge_fclose_ptr = &Menge::Menge_fclose_impl;

typedef int (*Tgetc)( FILE * );
Tgetc Menge_getc_ptr = &Menge::Menge_getc_impl;

#ifdef __cplusplus
extern "C"
{
#endif

FILE* Menge_fopen( const char* _filename, const char* _mode )
{
	return (*Menge_fopen_ptr)( _filename, _mode );
}

int Menge_fseek( FILE* _file, long int _offset, int _origin )
{
	return (*Menge_fseek_ptr)( _file, _offset, _origin );
}

long int Menge_ftell( FILE* _file )
{
	return (*Menge_ftell_ptr)( _file );
}

size_t Menge_fread( void * _ptr, size_t _size, size_t _count, FILE * _file )
{
	return (*Menge_fread_ptr)( _ptr, _size, _count, _file );
}

int Menge_fclose ( FILE * _file )
{
	return (*Menge_fclose_ptr)( _file );
}

int Menge_getc ( FILE * _file )
{
	return (*Menge_getc_ptr)( _file );
}

long Menge_PyMarshal_ReadLongFromFile( FILE* _file )
{
	long longNum[1] = { 0L };
	Menge_fread( longNum, sizeof(long), 1, _file );
	return longNum[0];
}

int Menge_PyMarshal_ReadShortFromFile( FILE* _file )
{
	short x = 0;
	Menge_fread( &x, sizeof(short), 1, _file );
	x |= -(x & 0x8000); 
	return x;
}

#ifdef __cplusplus
}
#endif
