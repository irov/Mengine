
#	pragma once

#ifdef __cplusplus
extern "C"
{
#endif
	
// wrap stdio
FILE* Menge_fopen( const char* _filename, const char* _mode );
int Menge_fseek( FILE* _file, long int _offset, int _origin );
long int Menge_ftell( FILE* _file );
size_t Menge_fread ( void * _ptr, size_t _size, size_t _count, FILE * _file );
int Menge_fclose ( FILE * _file );
int Menge_getc ( FILE * _file );

// wrap PyMarshal
long Menge_PyMarshal_ReadLongFromFile(FILE *);
int Menge_PyMarshal_ReadShortFromFile(FILE *);

#ifdef __cplusplus
}
#endif

#	define fopen( file, mode ) Menge_fopen( file, mode )
#	define fseek( file, offset, origin ) Menge_fseek( file, offset, origin )
#	define ftell( file ) Menge_ftell( file )
#	define fread( ptr, size, count, file ) Menge_fread( ptr, size, count, file )
#	define fclose( file ) Menge_fclose( file )
#	define getc( file ) Menge_getc( file )

#	define PyMarshal_ReadLongFromFile( file ) Menge_PyMarshal_ReadLongFromFile( file )
#	define PyMarshal_ReadShortFromFile( file ) Menge_PyMarshal_ReadShortFromFile( file )
