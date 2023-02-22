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

    PWSTR pwCmdLine = ::GetCommandLineW();

    std::wstring in = parse_kwds( pwCmdLine, L"--in", std::wstring() );
    std::wstring out = parse_kwds( pwCmdLine, L"--out", std::wstring() );
    std::wstring secure = parse_kwds( pwCmdLine, L"--secure", std::wstring() );

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

    std::string ansi_secure = std::string( secure.begin(), secure.end() );

    const char * ansi_secure_str = ansi_secure.c_str();
    size_t ansi_secure_size = ansi_secure.size();

    uint64_t sequreHash;
    Mengine::Helper::makeSHA1u64( ansi_secure_str, ansi_secure_size, &sequreHash );

    Mengine::Helper::ravingcode( sequreHash, memory_buffer, memory_size, memory_buffer );

    size_t write_size;
    bool result = write_file_memory( out.c_str(), "RGCD", memory_buffer, memory_size, &write_size );

    free_file_memory( memory_buffer );

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