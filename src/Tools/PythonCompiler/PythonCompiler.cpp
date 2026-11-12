#include "ToolUtils/ToolUtils.h"

#include "tinypy/tinypy.hpp"

#include <cstdint>
#include <cstdlib>
#include <vector>

//////////////////////////////////////////////////////////////////////////
static const int32_t PYTHON_COMPILER_OPTIMIZE_LEVEL = 1;
static const char PYTHON_COMPILER_DEBUG_FLAG[] = "_DEVELOPMENT";
static const size_t PYTHON_COMPILER_HEADER_SIZE = sizeof( uint32_t ) * 2;
//////////////////////////////////////////////////////////////////////////
typedef std::vector<uint8_t> VectorBytes;
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
static bool compileSource( tinypy_vm_t * _vm, const tinypy_build_profile_t * _profile, const uint8_t * _source, size_t _sourceSize, const std::string & _filename, const wchar_t * _output, uint32_t _timestamp )
{
    tinypy_compile_options_t options;
    tinypy_compile_options_init( &options, TINYPY_COMPILE_EXEC );
    options.flags = TINYPY_COMPILE_FLAG_STRING_SOURCE;
    options.dont_inherit = TINYPY_TRUE;
    options.optimize_level = tinypy_build_profile_optimize_level( _profile );
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
static bool compileFile( const uint8_t * _source, size_t _sourceSize, const std::string & _filename, const wchar_t * _output, uint32_t _timestamp )
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

    tinypy_build_constant_t debugFlag{};
    debugFlag.abi_version = TINYPY_COMPILER_ABI_VERSION;
    debugFlag.struct_size = sizeof( debugFlag );
    debugFlag.name = PYTHON_COMPILER_DEBUG_FLAG;
    debugFlag.name_size = sizeof( PYTHON_COMPILER_DEBUG_FLAG ) - 1;
    tinypy_build_value_init( &debugFlag.value, TINYPY_BUILD_VALUE_DEBUG_FLAG );

    tinypy_build_profile_t * profile = nullptr;
    tinypy_build_profile_result_e result = tinypy_build_profile_create( &allocator, optimizeLevel, &debugFlag, 1, nullptr, &profile, nullptr );

    if( result != TINYPY_BUILD_PROFILE_OK )
    {
        message_error( "PythonCompiler: cannot create build profile" );

        tinypy_vm_destroy( vm );

        return false;
    }

    bool successful = compileSource( vm, profile, _source, _sourceSize, _filename, _output, _timestamp );

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

    if( input.empty() == true )
    {
        message_error( "Usage: PythonCompiler --in <script.py> --out <script.pyo> | --in <script.py> --validate (Debug profile)" );

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
            message_error( "Usage: PythonCompiler --in <script.py> --out <script.pyo> | --in <script.py> --validate (Debug profile)" );

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

    bool successful = compileFile( source, sourceSize, filename, outputPath, timestamp );

    std::free( source );

    if( successful == false )
    {
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}
