#include "ToolUtils.h"

#include <algorithm>
#include <codecvt>
#include <cerrno>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <cwchar>
#include <filesystem>
#include <locale>

#if defined(__APPLE__)
#   include <crt_externs.h>
#   include <fcntl.h>
#   include <fnmatch.h>
#   include <sys/wait.h>
#   include <unistd.h>
#endif

//////////////////////////////////////////////////////////////////////////
static std::string wide_to_utf8( const wchar_t * _value, size_t _size = std::wstring::npos )
{
    const std::wstring value = _size == std::wstring::npos
        ? std::wstring( _value )
        : std::wstring( _value, _size );

    std::wstring_convert<std::codecvt_utf8<wchar_t>> converter;
    return converter.to_bytes( value );
}
//////////////////////////////////////////////////////////////////////////
static std::wstring utf8_to_wide( const char * _value )
{
    std::wstring_convert<std::codecvt_utf8<wchar_t>> converter;
    return converter.from_bytes( _value );
}
//////////////////////////////////////////////////////////////////////////
static FILE * open_wide_file( const wchar_t * _path, const wchar_t * _mode )
{
#if defined(_WIN32)
    return ::_wfopen( _path, _mode );
#else
    const std::string path = wide_to_utf8( _path );
    const std::string mode = wide_to_utf8( _mode );
    return ::fopen( path.c_str(), mode.c_str() );
#endif
}
//////////////////////////////////////////////////////////////////////////
#if !defined(_WIN32)
FILE * _wfopen( const wchar_t * _path, const wchar_t * _mode )
{
    return open_wide_file( _path, _mode );
}
//////////////////////////////////////////////////////////////////////////
errno_t _wfopen_s( FILE ** _file, const wchar_t * _path, const wchar_t * _mode )
{
    *_file = open_wide_file( _path, _mode );
    return *_file == nullptr ? errno : 0;
}
//////////////////////////////////////////////////////////////////////////
int _wremove( const wchar_t * _path )
{
    const std::string path = wide_to_utf8( _path );
    return ::remove( path.c_str() );
}
//////////////////////////////////////////////////////////////////////////
int PathCanonicalize( wchar_t * _destination, const wchar_t * _source )
{
    if( _destination == nullptr || _source == nullptr )
    {
        return 0;
    }

    const std::filesystem::path normalized = std::filesystem::path( wide_to_utf8( _source ) ).lexically_normal();
    const std::wstring value = utf8_to_wide( normalized.string().c_str() );

    if( value.size() >= MAX_PATH )
    {
        return 0;
    }

    std::wmemcpy( _destination, value.c_str(), value.size() + 1 );
    return 1;
}
//////////////////////////////////////////////////////////////////////////
void PathUnquoteSpaces( wchar_t * _path )
{
    if( _path == nullptr )
    {
        return;
    }

    const size_t size = std::wcslen( _path );

    if( size >= 2 && _path[0] == L'\"' && _path[size - 1] == L'\"' )
    {
        std::wmemmove( _path, _path + 1, size - 2 );
        _path[size - 2] = L'\0';
    }
}
//////////////////////////////////////////////////////////////////////////
#endif

//////////////////////////////////////////////////////////////////////////
void message_error( const char * _format, ... )
{
    va_list argList;
    va_start( argList, _format );

    char str[2048 + 1] = {'\0'};
    ::vsnprintf( str, 2048, _format, argList );

    va_end( argList );

    ::printf( "%s\n", str );
}
//////////////////////////////////////////////////////////////////////////
size_t unicode_to_utf8( char * _utf8, size_t _capacity, const wchar_t * _unicode, size_t _size )
{
    if( _capacity == 0 )
    {
        return 0;
    }

#if defined(_WIN32)
    const int utf8_size = ::WideCharToMultiByte(
        CP_UTF8,
        WC_ERR_INVALID_CHARS,
        _unicode,
        static_cast<int>( _size ),
        _utf8,
        static_cast<int>( _capacity - 1 ),
        nullptr,
        nullptr );

    if( utf8_size <= 0 )
    {
        _utf8[0] = '\0';
        return 0;
    }

    _utf8[utf8_size] = '\0';
    return static_cast<size_t>( utf8_size );
#else
    const std::string utf8 = wide_to_utf8( _unicode, _size );
    const size_t copy_size = std::min( utf8.size(), _capacity - 1 );
    ::memcpy( _utf8, utf8.data(), copy_size );
    _utf8[copy_size] = '\0';
    return copy_size;
#endif
}
//////////////////////////////////////////////////////////////////////////
std::string unicode_to_utf8( const std::wstring & _unicode )
{
    return wide_to_utf8( _unicode.c_str(), _unicode.size() );
}
//////////////////////////////////////////////////////////////////////////
std::wstring utf8_to_unicode( const std::string & _utf8 )
{
    return utf8_to_wide( _utf8.c_str() );
}
//////////////////////////////////////////////////////////////////////////
std::wstring path_join( const std::wstring & _left, const std::wstring & _right )
{
#if defined(_WIN32)
    std::filesystem::path path( _left );
    path /= _right;
    return path.lexically_normal().wstring();
#else
    std::filesystem::path path( wide_to_utf8( _left.c_str(), _left.size() ) );
    path /= wide_to_utf8( _right.c_str(), _right.size() );
    return utf8_to_wide( path.lexically_normal().string().c_str() );
#endif
}
//////////////////////////////////////////////////////////////////////////
std::wstring path_parent( const std::wstring & _path )
{
#if defined(_WIN32)
    return std::filesystem::path( _path ).parent_path().wstring();
#else
    const std::filesystem::path path( wide_to_utf8( _path.c_str(), _path.size() ) );
    return utf8_to_wide( path.parent_path().string().c_str() );
#endif
}
//////////////////////////////////////////////////////////////////////////
std::wstring get_temporary_directory()
{
    const std::filesystem::path path = std::filesystem::temp_directory_path();

#if defined(_WIN32)
    return path.wstring();
#else
    return utf8_to_wide( path.string().c_str() );
#endif
}
//////////////////////////////////////////////////////////////////////////
bool create_directories( const std::wstring & _path )
{
    std::error_code error;

#if defined(_WIN32)
    std::filesystem::create_directories( std::filesystem::path( _path ), error );
#else
    std::filesystem::create_directories( std::filesystem::path( wide_to_utf8( _path.c_str(), _path.size() ) ), error );
#endif

    return error ? false : true;
}
//////////////////////////////////////////////////////////////////////////
bool execute_process( const std::wstring & _executable, const std::vector<std::wstring> & _arguments, const std::wstring & _logPath, int * const _exitCode )
{
#if defined(_WIN32)
    auto quoteArgument = []( const std::wstring & _argument )
    {
        if( _argument.find_first_of( L" \t\"" ) == std::wstring::npos )
        {
            return _argument;
        }

        std::wstring result = L"\"";
        size_t backslashes = 0;

        for( const wchar_t character : _argument )
        {
            if( character == L'\\' )
            {
                ++backslashes;
                continue;
            }

            if( character == L'\"' )
            {
                result.append( backslashes * 2 + 1, L'\\' );
                result += character;
                backslashes = 0;
                continue;
            }

            result.append( backslashes, L'\\' );
            backslashes = 0;
            result += character;
        }

        result.append( backslashes * 2, L'\\' );
        result += L'\"';
        return result;
    };

    std::wstring commandLine = quoteArgument( _executable );

    for( const std::wstring & argument : _arguments )
    {
        commandLine += L' ';
        commandLine += quoteArgument( argument );
    }

    std::vector<wchar_t> mutableCommandLine( commandLine.begin(), commandLine.end() );
    mutableCommandLine.push_back( L'\0' );

    HANDLE log = INVALID_HANDLE_VALUE;
    SECURITY_ATTRIBUTES security = {};
    security.nLength = sizeof( security );
    security.bInheritHandle = TRUE;

    STARTUPINFOW startup = {};
    startup.cb = sizeof( startup );
    startup.wShowWindow = SW_HIDE;

    if( _logPath.empty() == false )
    {
        log = ::CreateFileW( _logPath.c_str(), FILE_APPEND_DATA, FILE_SHARE_READ | FILE_SHARE_WRITE, &security, OPEN_ALWAYS, FILE_ATTRIBUTE_NORMAL, nullptr );

        if( log == INVALID_HANDLE_VALUE )
        {
            return false;
        }

        startup.dwFlags = STARTF_USESTDHANDLES;
        startup.hStdOutput = log;
        startup.hStdError = log;
    }

    PROCESS_INFORMATION process = {};
    const BOOL created = ::CreateProcessW( _executable.c_str(), mutableCommandLine.data(), nullptr, nullptr, log != INVALID_HANDLE_VALUE, CREATE_NO_WINDOW, nullptr, nullptr, &startup, &process );

    if( created == FALSE )
    {
        if( log != INVALID_HANDLE_VALUE )
        {
            ::CloseHandle( log );
        }

        return false;
    }

    ::CloseHandle( process.hThread );
    ::WaitForSingleObject( process.hProcess, INFINITE );

    DWORD exitCode = EXIT_FAILURE;
    ::GetExitCodeProcess( process.hProcess, &exitCode );
    ::CloseHandle( process.hProcess );

    if( log != INVALID_HANDLE_VALUE )
    {
        ::CloseHandle( log );
    }

    *_exitCode = static_cast<int>( exitCode );
    return true;
#else
    const std::string executable = wide_to_utf8( _executable.c_str(), _executable.size() );
    std::vector<std::string> utf8Arguments;
    utf8Arguments.reserve( _arguments.size() + 1 );
    utf8Arguments.emplace_back( executable );

    for( const std::wstring & argument : _arguments )
    {
        utf8Arguments.emplace_back( wide_to_utf8( argument.c_str(), argument.size() ) );
    }

    std::vector<char *> processArguments;
    processArguments.reserve( utf8Arguments.size() + 1 );

    for( std::string & argument : utf8Arguments )
    {
        processArguments.emplace_back( argument.data() );
    }

    processArguments.emplace_back( nullptr );

    const pid_t process = ::fork();

    if( process == -1 )
    {
        return false;
    }

    if( process == 0 )
    {
        if( _logPath.empty() == false )
        {
            const std::string logPath = wide_to_utf8( _logPath.c_str(), _logPath.size() );
            const int log = ::open( logPath.c_str(), O_CREAT | O_WRONLY | O_APPEND, 0666 );

            if( log == -1 )
            {
                _exit( 126 );
            }

            ::dup2( log, STDOUT_FILENO );
            ::dup2( log, STDERR_FILENO );
            ::close( log );
        }

        ::execvp( executable.c_str(), processArguments.data() );
        _exit( 127 );
    }

    int status = 0;

    while( ::waitpid( process, &status, 0 ) == -1 )
    {
        if( errno != EINTR )
        {
            return false;
        }
    }

    if( WIFEXITED( status ) != 0 )
    {
        *_exitCode = WEXITSTATUS( status );
    }
    else if( WIFSIGNALED( status ) != 0 )
    {
        *_exitCode = 128 + WTERMSIG( status );
    }
    else
    {
        *_exitCode = EXIT_FAILURE;
    }

    return true;
#endif
}
//////////////////////////////////////////////////////////////////////////
void parse_arg( const std::wstring & _str, bool & _value )
{
    uint32_t value = 0;
    ::swscanf( _str.c_str(), L"%u", &value );
    _value = value != 0;
}
//////////////////////////////////////////////////////////////////////////
void parse_arg( const std::wstring & _str, uint32_t & _value )
{
    uint32_t value = 0;
    ::swscanf( _str.c_str(), L"%u", &value );
    _value = value;
}
//////////////////////////////////////////////////////////////////////////
void parse_arg( const std::wstring & _str, float & _value )
{
    float value = 0.f;
    ::swscanf( _str.c_str(), L"%g", &value );
    _value = value;
}
//////////////////////////////////////////////////////////////////////////
void parse_arg( const std::wstring & _str, double & _value )
{
    double value = 0.0;
    ::swscanf( _str.c_str(), L"%lf", &value );
    _value = value;
}
//////////////////////////////////////////////////////////////////////////
void parse_arg( const std::wstring & _str, std::wstring & _value )
{
    _value = _str;
}
//////////////////////////////////////////////////////////////////////////
const std::vector<std::wstring> & get_command_line_arguments()
{
    static const std::vector<std::wstring> arguments = []()
    {
        std::vector<std::wstring> result;

#if defined(_WIN32)
        int count = 0;
        LPWSTR * values = ::CommandLineToArgvW( ::GetCommandLineW(), &count );

        if( values != nullptr )
        {
            result.assign( values, values + count );
            ::LocalFree( values );
        }
#elif defined(__APPLE__)
        const int count = *_NSGetArgc();
        char ** values = *_NSGetArgv();

        for( int index = 0; index != count; ++index )
        {
            result.emplace_back( utf8_to_wide( values[index] ) );
        }
#endif

        return result;
    }();

    return arguments;
}
//////////////////////////////////////////////////////////////////////////
bool has_args( const wchar_t * _key )
{
    const std::vector<std::wstring> & arguments = get_command_line_arguments();
    return std::find( arguments.begin(), arguments.end(), _key ) != arguments.end();
}
//////////////////////////////////////////////////////////////////////////
void ForcePathQuoteSpaces( WCHAR * _quotePath, const std::wstring & _path )
{
    if( _path.empty() == true )
    {
        _quotePath[0] = L'\0';
        return;
    }

#if defined(_WIN32)
    std::wstring true_path = _path;

    if( true_path.size() > 1 && true_path[0] == L'/' )
    {
        true_path[0] = true_path[1];
        true_path[1] = L':';
    }

    ::PathCanonicalizeW( _quotePath, true_path.c_str() );

    if( _quotePath[0] == L'"' )
    {
        return;
    }

    const size_t path_size = ::wcslen( _quotePath );
    ::wmemmove( _quotePath + 1, _quotePath, path_size );
    _quotePath[0] = L'"';
    _quotePath[path_size + 1] = L'"';
    _quotePath[path_size + 2] = L'\0';
#else
    const std::wstring quoted = L"\"" + std::filesystem::path( _path ).lexically_normal().wstring() + L"\"";
    ::wcsncpy( _quotePath, quoted.c_str(), MAX_PATH - 1 );
    _quotePath[MAX_PATH - 1] = L'\0';
#endif
}
//////////////////////////////////////////////////////////////////////////
int ForceRemoveDirectory( LPCTSTR _directory )
{
#if defined(_WIN32)
    const size_t length = ::wcslen( _directory ) + 2;
    std::vector<wchar_t> path( length, L'\0' );
    ::wcscpy_s( path.data(), length, _directory );

    SHFILEOPSTRUCTW operation = {};
    operation.wFunc = FO_DELETE;
    operation.pFrom = path.data();
    operation.fFlags = FOF_NOCONFIRMATION | FOF_NOERRORUI | FOF_SILENT;
    return ::SHFileOperationW( &operation );
#else
    std::error_code error;
    std::filesystem::remove_all( std::filesystem::path( _directory ), error );
    return error ? -1 : 0;
#endif
}
//////////////////////////////////////////////////////////////////////////
bool SelectFile( LPCTSTR _wildcard, Files & _files )
{
#if defined(_WIN32)
    WIN32_FIND_DATAW data;
    HANDLE find = ::FindFirstFileW( _wildcard, &data );

    if( find == INVALID_HANDLE_VALUE )
    {
        return false;
    }

    do
    {
        if( (data.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) == 0 )
        {
            _files.emplace_back( data.cFileName );
        }
    } while( ::FindNextFileW( find, &data ) != FALSE );

    ::FindClose( find );
    return true;
#else
    const std::filesystem::path wildcard_path( _wildcard );
    const std::filesystem::path directory = wildcard_path.parent_path();
    const std::string pattern = wildcard_path.filename().string();
    std::error_code error;

    for( const std::filesystem::directory_entry & entry : std::filesystem::directory_iterator( directory, error ) )
    {
        if( entry.is_regular_file() == true && ::fnmatch( pattern.c_str(), entry.path().filename().string().c_str(), 0 ) == 0 )
        {
            _files.emplace_back( entry.path().filename().wstring() );
        }
    }

    return error ? false : true;
#endif
}
//////////////////////////////////////////////////////////////////////////
bool read_file_to_memory( const wchar_t * _path, uint8_t * _buffer, size_t _capacity, size_t * const _size )
{
    FILE * file = open_wide_file( _path, L"rb" );

    if( file == nullptr )
    {
        return false;
    }

    ::fseek( file, 0, SEEK_END );
    const long file_size = ::ftell( file );
    ::rewind( file );

    const size_t read_size = std::min( static_cast<size_t>( file_size ), _capacity );
    const size_t read = ::fread( _buffer, 1, read_size, file );
    ::fclose( file );

    *_size = read;
    return read == read_size;
}
//////////////////////////////////////////////////////////////////////////
bool read_file_memory( const wchar_t * _path, uint8_t ** _buffer, size_t * const _size )
{
    FILE * file = open_wide_file( _path, L"rb" );

    if( file == nullptr )
    {
        return false;
    }

    ::fseek( file, 0, SEEK_END );
    const long file_size = ::ftell( file );
    ::rewind( file );

    uint8_t * buffer = static_cast<uint8_t *>( ::malloc( static_cast<size_t>( file_size ) ) );

    if( buffer == nullptr )
    {
        ::fclose( file );
        return false;
    }

    const size_t read = ::fread( buffer, 1, static_cast<size_t>( file_size ), file );
    ::fclose( file );

    if( read != static_cast<size_t>( file_size ) )
    {
        ::free( buffer );
        return false;
    }

    *_buffer = buffer;
    *_size = read;
    return true;
}
//////////////////////////////////////////////////////////////////////////
bool write_file_memory( const wchar_t * _path, const uint8_t * _buffer, size_t _size )
{
    FILE * file = open_wide_file( _path, L"wb" );

    if( file == nullptr )
    {
        return false;
    }

    const size_t write_size = ::fwrite( _buffer, 1, _size, file );
    ::fclose( file );
    return write_size == _size;
}
//////////////////////////////////////////////////////////////////////////
bool move_file_memory( const wchar_t * _path, const uint8_t * _buffer, size_t _size )
{
    const bool result = write_file_memory( _path, _buffer, _size );
    ::free( const_cast<uint8_t *>( _buffer ) );
    return result;
}
//////////////////////////////////////////////////////////////////////////
bool write_file_magic_memory( const wchar_t * _path, const char * _magic, const uint8_t * _buffer, size_t _size, size_t * const _write )
{
    FILE * file = open_wide_file( _path, L"wb" );

    if( file == nullptr )
    {
        return false;
    }

    const size_t magic_size = ::strlen( _magic );
    const size_t magic_write = ::fwrite( _magic, 1, magic_size, file );
    const size_t buffer_write = ::fwrite( _buffer, 1, _size, file );
    ::fclose( file );

    *_write = magic_write + buffer_write;
    return *_write == magic_size + _size;
}
//////////////////////////////////////////////////////////////////////////
bool move_file_magic_memory( const wchar_t * _path, const char * _magic, const uint8_t * _buffer, size_t _size, size_t * const _write )
{
    const bool result = write_file_magic_memory( _path, _magic, _buffer, _size, _write );
    ::free( const_cast<uint8_t *>( _buffer ) );
    return result;
}
