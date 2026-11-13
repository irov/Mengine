#include "ToolUtils/ToolUtils.h"

#include "tinypy/tinypy.hpp"

#include <cstdint>
#include <cstdlib>
#include <vector>

//////////////////////////////////////////////////////////////////////////
static const int32_t PYTHON_COMPILER_OPTIMIZE_LEVEL = 1;
static const size_t PYTHON_COMPILER_HEADER_SIZE = sizeof( uint32_t ) * 2;
//////////////////////////////////////////////////////////////////////////
typedef std::vector<uint8_t> VectorBytes;
typedef std::vector<std::string> VectorDefinitions;
//////////////////////////////////////////////////////////////////////////
static void * allocate( void * _userData, size_t _size, size_t _alignment )
{
    (void)_userData;
    (void)_alignment;

    void * memory = std::malloc( _size );

    return memory;
}
//////////////////////////////////////////////////////////////////////////
static void * reallocate( void * _userData, void * _memory, size_t _oldSize, size_t _newSize, size_t _alignment )
{
    (void)_userData;
    (void)_oldSize;
    (void)_alignment;

    void * memory = std::realloc( _memory, _newSize );

    return memory;
}
//////////////////////////////////////////////////////////////////////////
static void deallocate( void * _userData, void * _memory, size_t _size, size_t _alignment )
{
    (void)_userData;
    (void)_size;
    (void)_alignment;

    std::free( _memory );
}
//////////////////////////////////////////////////////////////////////////
static void writeUint32( uint8_t * const _buffer, uint32_t _value )
{
    for( uint32_t index = 0; index != sizeof( _value ); ++index )
    {
        _buffer[index] = static_cast<uint8_t>(_value >> (index * 8));
    }
}
//////////////////////////////////////////////////////////////////////////
static bool writeCode( const tinypy_value_t * _code, const wchar_t * _output, uint32_t _timestamp )
{
    size_t marshalSize;
    tinypy_marshal_result_e sizeResult = tinypy_marshal_dump_code_v2( _code, nullptr, 0, &marshalSize, nullptr, nullptr );

    if( sizeResult != TINYPY_MARSHAL_OK )
    {
        message_error( "PythonCompiler: cannot size bytecode: %s", tinypy_marshal_result_name( sizeResult ) );

        return false;
    }

    size_t bufferSize = PYTHON_COMPILER_HEADER_SIZE + marshalSize;

    VectorBytes buffer( bufferSize );

    uint8_t * header = buffer.data();
    uint8_t * headerTimestamp = header + sizeof( uint32_t );
    uint8_t * body = header + PYTHON_COMPILER_HEADER_SIZE;

    tinypy_marshal_result_e dumpResult = tinypy_marshal_dump_code_v2( _code, body, marshalSize, &marshalSize, nullptr, nullptr );

    if( dumpResult != TINYPY_MARSHAL_OK )
    {
        message_error( "PythonCompiler: cannot serialize bytecode: %s", tinypy_marshal_result_name( dumpResult ) );

        return false;
    }

    writeUint32( header, TINYPY_CPYTHON_27_MAGIC );
    writeUint32( headerTimestamp, _timestamp );

    if( write_file_memory( _output, header, bufferSize ) == false )
    {
        message_error( "PythonCompiler: cannot write output '%s'", unicode_to_utf8( _output ).c_str() );

        return false;
    }

    return true;
}
//////////////////////////////////////////////////////////////////////////
static bool compileSource( tinypy_vm_t * _vm, int32_t _optimizeLevel, const tinypy_build_profile_t * _profile, const uint8_t * _source, size_t _sourceSize, const std::string & _filename, const wchar_t * _output, uint32_t _timestamp )
{
    tinypy_compile_options_t options;
    tinypy_compile_options_init( &options, TINYPY_COMPILE_EXEC );
    options.flags = TINYPY_COMPILE_FLAG_STRING_SOURCE;
    options.dont_inherit = TINYPY_TRUE;
    options.optimize_level = _optimizeLevel;
    options.feature_flags = TINYPY_COMPILE_FEATURE_PREPROCESSOR | TINYPY_COMPILE_FEATURE_META;
    options.build_profile = _profile;

    const char * filename = _filename.c_str();
    size_t filenameSize = _filename.size();

    tinypy_error_t * error = nullptr;
    tinypy_value_t * code = tinypy_compile_source( _vm, _source, _sourceSize, filename, filenameSize, &options, &error );

    if( code == nullptr )
    {
        if( error == nullptr )
        {
            message_error( "%s:0:0: compilation failed", filename );

            return false;
        }

        size_t messageSize;
        const char * message = tinypy_error_message( error, &messageSize );

        message_error( "%s:%d:%d: %.*s", filename, tinypy_error_line_number( error ), tinypy_error_column_offset( error ), static_cast<int>(messageSize), message );

        tinypy_error_release( error );

        return false;
    }

    bool successful = true;

    if( _output != nullptr )
    {
        successful = writeCode( code, _output, _timestamp );
    }

    tinypy_release( code );

    if( successful == false )
    {
        return false;
    }

    return true;
}
//////////////////////////////////////////////////////////////////////////
static bool compileFile( const uint8_t * _source, size_t _sourceSize, const std::string & _filename, const wchar_t * _output, uint32_t _timestamp, const VectorDefinitions & _definitions )
{
    int32_t optimizeLevel = _output == nullptr ? 0 : PYTHON_COMPILER_OPTIMIZE_LEVEL;

    tinypy_allocator_t allocator{};
    allocator.abi_version = TINYPY_ABI_VERSION;
    allocator.struct_size = sizeof( allocator );
    allocator.allocate = &allocate;
    allocator.reallocate = &reallocate;
    allocator.deallocate = &deallocate;

    tinypy_vm_config_t config{};
    config.abi_version = TINYPY_ABI_VERSION;
    config.struct_size = sizeof( config );
    config.allocator = &allocator;
    config.optimize_level = optimizeLevel;

    tinypy_vm_t * vm = tinypy_vm_create( &config );

    if( vm == nullptr )
    {
        message_error( "PythonCompiler: cannot initialize TinyPy" );

        return false;
    }

    std::vector<const char *> definitions;

    for( const std::string & definition : _definitions )
    {
        definitions.emplace_back( definition.c_str() );
    }

    tinypy_build_profile_t * profile = nullptr;
    tinypy_build_profile_error_t profileError;

    if( tinypy_build_profile_parse( vm, &allocator, definitions.data(), definitions.size(), nullptr, &profile, &profileError ) != TINYPY_BUILD_PROFILE_OK )
    {
        if( profileError.constant_index < definitions.size() )
        {
            message_error( "PythonCompiler: invalid constant definition '%s': %.*s", definitions[profileError.constant_index], static_cast<int>(profileError.message_size), profileError.message );
        }
        else
        {
            message_error( "PythonCompiler: invalid constant definitions: %.*s", static_cast<int>(profileError.message_size), profileError.message );
        }

        tinypy_vm_destroy( vm );

        return false;
    }

    bool successful = compileSource( vm, optimizeLevel, profile, _source, _sourceSize, _filename, _output, _timestamp );

    tinypy_build_profile_destroy( profile );
    tinypy_vm_destroy( vm );

    if( successful == false )
    {
        return false;
    }

    return true;
}
//////////////////////////////////////////////////////////////////////////
int main( int argc, char * argv[] )
{
    (void)argc;
    (void)argv;

    std::wstring input = parse_kwds( L"--in", std::wstring() );
    bool validate = has_args( L"--validate" );
    VectorDefinitions definitions;
    const std::vector<std::wstring> & arguments = get_command_line_arguments();

    for( size_t index = 1; index < arguments.size(); ++index )
    {
        const std::wstring & argument = arguments[index];

        if( argument == L"--in" || argument == L"--out" )
        {
            ++index;

            continue;
        }

        if( argument == L"--validate" )
        {
            continue;
        }

        if( argument.compare( 0, 2, L"-D" ) != 0 )
        {
            message_error( "PythonCompiler: unknown argument '%s'", unicode_to_utf8( argument ).c_str() );

            return EXIT_FAILURE;
        }

        std::wstring definition = argument.substr( 2 );

        if( definition.empty() == true )
        {
            ++index;

            if( index == arguments.size() )
            {
                message_error( "PythonCompiler: -D requires NAME=literal" );

                return EXIT_FAILURE;
            }

            definition = arguments[index];
        }

        definitions.emplace_back( unicode_to_utf8( definition ) );
    }

    if( input.empty() == true )
    {
        message_error( "Usage: PythonCompiler --in <script.py> (--out <script.pyo> | --validate) [-DNAME=literal ...]" );

        return EXIT_FAILURE;
    }

    std::wstring output;

    const wchar_t * inputPath = input.c_str();
    const wchar_t * outputPath = nullptr;

    if( validate == true )
    {
        if( has_args( L"--out" ) == true )
        {
            message_error( "PythonCompiler: --validate does not accept --out" );

            return EXIT_FAILURE;
        }
    }
    else
    {
        output = parse_kwds( L"--out", std::wstring() );

        if( output.empty() == true )
        {
            message_error( "Usage: PythonCompiler --in <script.py> (--out <script.pyo> | --validate) [-DNAME=literal ...]" );

            return EXIT_FAILURE;
        }

        outputPath = output.c_str();

        if( file_paths_equivalent( inputPath, outputPath ) == true )
        {
            message_error( "PythonCompiler: input and output must differ" );

            return EXIT_FAILURE;
        }
    }

    std::string filename = unicode_to_utf8( input );

    uint8_t * source;
    size_t sourceSize;
    uint32_t timestamp;
    if( read_file_memory( inputPath, &source, &sourceSize, &timestamp ) == false )
    {
        message_error( "PythonCompiler: cannot read input '%s'", filename.c_str() );

        return EXIT_FAILURE;
    }

    bool successful = compileFile( source, sourceSize, filename, outputPath, timestamp, definitions );

    std::free( source );

    if( successful == false )
    {
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}
