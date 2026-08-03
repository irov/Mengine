#include "XmlToAekConverter.h"

#include "ToolEngine/ToolEngine.h"
#include "ToolUtils/ToolUtils.h"

#include <cstdlib>
#include <string>

//////////////////////////////////////////////////////////////////////////
int main( int argc, char * argv[] )
{
    (void)argc;
    (void)argv;

    if( has_args( L"--help" ) == true || has_args( L"-h" ) == true )
    {
        message_error( "usage: AekCompiler --protocol <protocol.xml> --in <source.xml> --out <output.aek>" );

        return EXIT_SUCCESS;
    }

    const std::wstring protocol = parse_kwds( L"--protocol", std::wstring() );
    const std::wstring input = parse_kwds( L"--in", std::wstring() );
    const std::wstring output = parse_kwds( L"--out", std::wstring() );

    if( protocol.empty() == true )
    {
        message_error( "AekCompiler missing required argument: --protocol <protocol.xml>" );

        return EXIT_FAILURE;
    }

    if( input.empty() == true )
    {
        message_error( "AekCompiler missing required argument: --in <source.xml>" );

        return EXIT_FAILURE;
    }

    if( output.empty() == true )
    {
        message_error( "AekCompiler missing required argument: --out <output.aek>" );

        return EXIT_FAILURE;
    }

    const std::wstring outputDirectory = path_parent( output );

    if( outputDirectory.empty() == false && create_directories( outputDirectory ) == false )
    {
        message_error( "AekCompiler cannot create output directory: %ls", outputDirectory.c_str() );

        return EXIT_FAILURE;
    }

    if( Mengine::ToolEngineInitialize() == false )
    {
        message_error( "AekCompiler failed to initialize Mengine services" );
        return EXIT_FAILURE;
    }

    const bool successful = Mengine::writeAek( protocol.c_str(), input.c_str(), output.c_str() );

    Mengine::ToolEngineFinalize();

    return successful == true ? EXIT_SUCCESS : EXIT_FAILURE;
}
//////////////////////////////////////////////////////////////////////////
