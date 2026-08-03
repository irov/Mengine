#include "Convert.h"

#include "ToolEngine/ToolEngine.h"
#include "ToolUtils/ToolUtils.h"

#include "Kernel/ConstStringHelper.h"

#include "jansson.h"

#include <cstdlib>
#include <string>
#include <vector>

//////////////////////////////////////////////////////////////////////////
static std::string toUtf8( const std::wstring & _value )
{
    std::vector<char> buffer( _value.size() * 4 + 1, '\0' );
    const size_t size = unicode_to_utf8( buffer.data(), buffer.size() - 1, _value.c_str(), _value.size() );

    return std::string( buffer.data(), size );
}
//////////////////////////////////////////////////////////////////////////
static bool jsonValueToString( json_t * _value, std::string * const _result )
{
    if( json_is_string( _value ) != 0 )
    {
        *_result = json_string_value( _value );
        return true;
    }

    if( json_is_true( _value ) != 0 )
    {
        *_result = "True";
        return true;
    }

    if( json_is_false( _value ) != 0 )
    {
        *_result = "False";
        return true;
    }

    if( json_is_null( _value ) != 0 )
    {
        *_result = "None";
        return true;
    }

    char * dump = json_dumps( _value, JSON_ENCODE_ANY | JSON_COMPACT );

    if( dump == nullptr )
    {
        return false;
    }

    *_result = dump;
    free( dump );

    return true;
}
//////////////////////////////////////////////////////////////////////////
static bool parseParams( const std::wstring & _paramsJson, Mengine::Params * const _params )
{
    if( _paramsJson.empty() == true )
    {
        return true;
    }

    const std::string paramsJson = toUtf8( _paramsJson );
    json_error_t error;
    json_t * root = json_loadb( paramsJson.data(), paramsJson.size(), 0, &error );

    if( root == nullptr )
    {
        message_error( "invalid --params-json at line %d column %d: %s", error.line, error.column, error.text );
        return false;
    }

    if( json_is_object( root ) == 0 )
    {
        message_error( "--params-json must be a JSON object" );
        json_decref( root );
        return false;
    }

    const char * key;
    json_t * value;
    json_object_foreach( root, key, value )
    {
        std::string stringValue;

        if( jsonValueToString( value, &stringValue ) == false )
        {
            message_error( "unable to convert JSON parameter '%s' to a string", key );
            json_decref( root );
            return false;
        }

        ( *_params )[Mengine::Helper::stringizeString( key )] = Mengine::ParamString( stringValue );
    }

    json_decref( root );
    return true;
}
//////////////////////////////////////////////////////////////////////////
int main( int argc, char * argv[] )
{
    (void)argc;
    (void)argv;

    if( has_args( L"--help" ) == true || has_args( L"-h" ) == true )
    {
        message_error( "usage: MengineConverter --converter <name> --in <source> --out <output> [--params-json <object>]" );

        return EXIT_SUCCESS;
    }

    const std::wstring converter = parse_kwds( L"--converter", std::wstring() );
    const std::wstring input = parse_kwds( L"--in", std::wstring() );
    const std::wstring output = parse_kwds( L"--out", std::wstring() );
    const std::wstring paramsJson = parse_kwds( L"--params-json", std::wstring( L"{}" ) );

    if( converter.empty() == true )
    {
        message_error( "MengineConverter missing required argument: --converter <name>" );

        return EXIT_FAILURE;
    }

    if( input.empty() == true )
    {
        message_error( "MengineConverter missing required argument: --in <source>" );

        return EXIT_FAILURE;
    }

    if( output.empty() == true )
    {
        message_error( "MengineConverter missing required argument: --out <output>" );

        return EXIT_FAILURE;
    }

    const std::wstring outputDirectory = path_parent( output );

    if( outputDirectory.empty() == false && create_directories( outputDirectory ) == false )
    {
        message_error( "MengineConverter cannot create output directory: %ls", outputDirectory.c_str() );

        return EXIT_FAILURE;
    }

    if( Mengine::ToolEngineInitialize() == false )
    {
        message_error( "MengineConverter failed to initialize Mengine services" );
        return EXIT_FAILURE;
    }

    bool successful = false;

    {
        Mengine::Params params;

        if( parseParams( paramsJson, &params ) == true )
        {
            successful = Mengine::convert( input.c_str(), output.c_str(), converter.c_str(), params );
        }
    }

    Mengine::ToolEngineFinalize();
    return successful == true ? EXIT_SUCCESS : EXIT_FAILURE;
}
//////////////////////////////////////////////////////////////////////////
