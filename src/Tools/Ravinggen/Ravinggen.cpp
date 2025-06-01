#include "Environment/Windows/WindowsIncluder.h"

#include "ToolUtils/ToolUtils.h"

#include "Kernel/Ravingcode.h"
#include "Kernel/SHA1.h"

#include <string>
#include <vector>
#include <stdio.h>

//////////////////////////////////////////////////////////////////////////
int main( int argc, char * argv[] )
{
    MENGINE_UNUSED( argc );
    MENGINE_UNUSED( argv );

    std::wstring in = parse_kwds( L"--in", std::wstring() );
    std::wstring out = parse_kwds( L"--out", std::wstring() );
    std::wstring secure = parse_kwds( L"--secure", std::wstring() );

    if( in.empty() == true )
    {
        message_error( "not found 'in' param" );

        return EXIT_FAILURE;
    }

    if( out.empty() == true )
    {
        message_error( "not found 'out' param" );

        return EXIT_FAILURE;
    }

    if( secure.empty() == true )
    {
        message_error( "not found 'secure' param" );

        return EXIT_FAILURE;
    }

    size_t memory_size;
    uint8_t * memory_buffer;
    if( read_file_memory( in.c_str(), &memory_buffer, &memory_size ) == false )
    {
        message_error( "invalid read %ls"
            , in.c_str()
        );

        return EXIT_FAILURE;
    }

    char ansi_secure_str[2048 + 1];
    size_t ansi_secure_size = unicode_to_utf8( ansi_secure_str, 2048, secure.c_str(), secure.size() );

    uint64_t sequreHash;
    Mengine::Helper::makeSHA1( ansi_secure_str, ansi_secure_size, reinterpret_cast<uint8_t *>(&sequreHash), sizeof( sequreHash ) );

    Mengine::Helper::ravingcode( &sequreHash, 1, memory_buffer, memory_size, memory_buffer );

    size_t write_size;
    bool result = move_file_magic_memory( out.c_str(), "RGCD", memory_buffer, memory_size, &write_size );

    if( result == false )
    {
        message_error( "invalid write %ls"
            , out.c_str()
        );

        return EXIT_FAILURE;
    }

    message_error( "successful ravinggen" );

    return EXIT_SUCCESS;
}