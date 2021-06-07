#include "ToolUtils/ToolUtils.h"

#include "Plugins/DazzlePlugin/DazzleInterface.h"

#include "Kernel/Blobject.h"

#include "lz4.h"
#include "lz4hc.h"

#include <vector>
#include <string>
#include <sstream>

//////////////////////////////////////////////////////////////////////////
typedef std::vector<uint8_t> Blobject;
//////////////////////////////////////////////////////////////////////////
extern "C"
{
    //////////////////////////////////////////////////////////////////////////
    void * LZ4_malloc( size_t _size )
    {
        void * p = malloc( _size );

        return p;
    }
    //////////////////////////////////////////////////////////////////////////
    void * LZ4_calloc( size_t _count, size_t _size )
    {
        void * p = calloc( _count, _size );

        return p;
    }
    //////////////////////////////////////////////////////////////////////////
    void  LZ4_free( void * _ptr )
    {
        free( _ptr );
    }
    //////////////////////////////////////////////////////////////////////////
}
//////////////////////////////////////////////////////////////////////////
static bool loadFile( const std::wstring & _filepath, Blobject & _buffer )
{
    FILE * f = ::_wfopen( _filepath.c_str(), L"rb" );

    if( f == NULL )
    {
        message_error( "invalid _wfopen %ls"
            , _filepath.c_str()
        );

        return false;
    }

    ::fseek( f, 0, SEEK_END );
    int f_size = ::ftell( f );
    ::rewind( f );

    if( f_size == 0 )
    {
        ::fclose( f );

        message_error( "invalid size %ls"
            , _filepath.c_str()
        );

        return false;
    }

    _buffer.resize( f_size );

    uint8_t * memory = &_buffer[0];

    ::fread( memory, 1, f_size, f );
    ::fclose( f );

    return true;
}
//////////////////////////////////////////////////////////////////////////
static bool writeCompress( const std::wstring & _filepath, const Blobject & _buffer, Mengine::magic_number_type numberz, Mengine::magic_version_type versionz )
{
    FILE * fz = ::_wfopen( _filepath.c_str(), L"wb" );

    if( fz == NULL )
    {
        message_error( "invalid _wfopen '%ls'"
            , _filepath.c_str()
        );

        return false;
    }

    ::fwrite( &numberz, sizeof( numberz ), 1, fz );
    ::fwrite( &versionz, sizeof( versionz ), 1, fz );

    uint32_t value_crc32 = 0;
    ::fwrite( &value_crc32, sizeof( value_crc32 ), 1, fz );

    uint32_t buffer_size = (uint32_t)_buffer.size();

    uint32_t write_uncompressSize = buffer_size;
    ::fwrite( &write_uncompressSize, sizeof( write_uncompressSize ), 1, fz );

    int lz_size = ::LZ4_compressBound( buffer_size );

    std::vector<char> lz4_buffer( lz_size );
    char * lz4_memory = &lz4_buffer[0];

    char * dst_buffer = (char *)lz4_memory;
    const char * src_buffer = (const char *)&_buffer[0];

    int compressSize = ::LZ4_compress_HC( src_buffer, dst_buffer, buffer_size, lz_size, LZ4HC_CLEVEL_MAX );

    if( compressSize < 0 )
    {
        ::fclose( fz );

        message_error( "invalid compress '%ls'"
            , _filepath.c_str()
        );

        return false;
    }

    uint32_t write_compressSize = (uint32_t)compressSize;
    ::fwrite( &write_compressSize, sizeof( write_compressSize ), 1, fz );

    ::fwrite( dst_buffer, compressSize, 1, fz );
    ::fclose( fz );

    return true;
}
//////////////////////////////////////////////////////////////////////////
int APIENTRY wWinMain( HINSTANCE hInstance, HINSTANCE hPrevInstance, PWSTR lpCmdLine, int nShowCmd )
{
    MENGINE_UNUSED( hInstance );
    MENGINE_UNUSED( hPrevInstance );
    MENGINE_UNUSED( nShowCmd );

    std::wstring in_path = parse_kwds( lpCmdLine, L"--in_path", std::wstring() );
    std::wstring out_path = parse_kwds( lpCmdLine, L"--out_path", std::wstring() );

    if( in_path.empty() == true )
    {
        message_error( "not found 'in' param" );

        return EXIT_FAILURE;
    }

    if( out_path.empty() == true )
    {
        message_error( "not found 'out' param" );

        return EXIT_FAILURE;
    }

    Blobject buffer;
    if( loadFile( in_path, buffer ) == false )
    {
        return EXIT_FAILURE;
    }

    Mengine::magic_number_type numberz = GET_MAGIC_NUMBER( Mengine::MAGIC_DZZ );
    Mengine::magic_version_type versionz = GET_MAGIC_VERSION( Mengine::MAGIC_DZZ );

    if( writeCompress( out_path, buffer, numberz, versionz ) == false )
    {
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}