#include <iostream>

#include "Interface/ServiceInterface.h"

#include "XmlToBinDecoder.h"
#include "AlphaSpreading.h"
#include "XmlToAekConverter.h"

#include "Interface/StringizeServiceInterface.h"
#include "Interface/UnicodeSystemInterface.h"
#include "Interface/ArchivatorInterface.h"
#include "Interface/LoggerInterface.h"
#include "Interface/CodecInterface.h"
#include "Interface/DataInterface.h"
#include "Interface/MemoryInterface.h"
#include "Interface/EnumeratorServiceInterface.h"
#include "Interface/ConverterInterface.h"
#include "Interface/FileServiceInterface.h"
#include "Interface/PluginInterface.h"
#include "Interface/UnicodeSystemInterface.h"
#include "Interface/ImageCodecInterface.h"
#include "Interface/LoaderInterface.h"
#include "Interface/ThreadSystemInterface.h"
#include "Interface/ConfigServiceInterface.h"
#include "Interface/OptionsServiceInterface.h"
#include "Interface/CodecServiceInterface.h"
#include "Interface/VocabularyServiceInterface.h"
#include "Interface/PluginServiceInterface.h"
#include "Interface/LoggerServiceInterface.h"

#include "Environment/Windows/WindowsIncluder.h"

#include "Config/String.h"

#include "Kernel/ConverterFactory.h"
#include "Kernel/LoggerBase.h"

#include "Image.h"

#include "Kernel/Logger.h"
#include "Kernel/Document.h"
#include "Kernel/FilePathHelper.h"
#include "Kernel/FactoryDefault.h"
#include "Kernel/UnicodeHelper.h"
#include "Kernel/FileStreamHelper.h"

#include "Config/Vector.h"

#include "pybind/pybind.hpp"
#include "pybind/stl/stl_type_cast.hpp"

#include "stdex/sha1.h"

#include <algorithm>
#include <io.h>

//////////////////////////////////////////////////////////////////////////
PLUGIN_EXPORT( Win32FileGroup );
PLUGIN_EXPORT( ImageCodec );
PLUGIN_EXPORT( Movie1 );
PLUGIN_EXPORT( Movie );
PLUGIN_EXPORT( Zip );
PLUGIN_EXPORT( LZ4 );
PLUGIN_EXPORT( MetabufLoader );
PLUGIN_EXPORT( DevelopmentConverter );
//////////////////////////////////////////////////////////////////////////
SERVICE_PROVIDER_EXTERN( ServiceProvider );

SERVICE_EXTERN( Platform );
SERVICE_EXTERN( NotificationService );
SERVICE_EXTERN( OptionsService );
SERVICE_EXTERN( FactoryService );
SERVICE_EXTERN( OptionsService );
SERVICE_EXTERN( UnicodeSystem );
SERVICE_EXTERN( StringizeService );
SERVICE_EXTERN( ArchiveService );
SERVICE_EXTERN( ArchiveService );
SERVICE_EXTERN( LoggerService );
SERVICE_EXTERN( ConfigService );
SERVICE_EXTERN( ConverterService );
SERVICE_EXTERN( CodecService );
SERVICE_EXTERN( DataService );
SERVICE_EXTERN( ThreadSystem );
SERVICE_EXTERN( ThreadService );
SERVICE_EXTERN( MemoryService );
SERVICE_EXTERN( ModuleService );
SERVICE_EXTERN( EnumeratorService );
SERVICE_EXTERN( PluginService );
SERVICE_EXTERN( PrototypeService );
SERVICE_EXTERN( FileService );
SERVICE_EXTERN( VocabularyService );
SERVICE_EXTERN( LoaderService );
//////////////////////////////////////////////////////////////////////////
namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    static void createConsole()
    {
        typedef BOOL( WINAPI *PATTACHCONSOLE )(DWORD);

        PATTACHCONSOLE pAttachConsole = nullptr;

        HMODULE hKernel32 = ::LoadLibraryW( L"Kernel32.dll" );

        if( hKernel32 != nullptr )
        {
            FARPROC proc = GetProcAddress( hKernel32, "AttachConsole" );

            pAttachConsole = reinterpret_cast<PATTACHCONSOLE>(proc);

            if( pAttachConsole == nullptr )
            {
                FreeLibrary( hKernel32 );
                return;
            }
        }


        CONSOLE_SCREEN_BUFFER_INFO coninfo;

        // try to attach to calling console first
        if( pAttachConsole( (DWORD)-1 ) == FALSE )
        {
            // allocate a console for this app
            AllocConsole();

            // set the screen buffer to be big enough to let us scroll text
            HANDLE output_handle = ::GetStdHandle( STD_OUTPUT_HANDLE );
            GetConsoleScreenBufferInfo( output_handle, &coninfo );
            coninfo.dwSize.Y = 1000;
            SetConsoleScreenBufferSize( output_handle, coninfo.dwSize );
        }

        FILE * CONOUT = freopen( "CONOUT$", "w", stdout );
        MENGINE_UNUSED( CONOUT );

        //::MoveWindow( GetConsoleWindow(), 0, 650, 0, 0, TRUE );
        // make cout, wcout, cin, wcin, wcerr, cerr, wclog and clog
        // point to console as well
        std::ios::sync_with_stdio();

        if( hKernel32 != nullptr )
        {
            FreeLibrary( hKernel32 );
        }

        std::cout << "console ready.." << std::endl;
    }

    static bool initialize()
    {
        stdex_allocator_initialize();

        ServiceProviderInterface * serviceProvider;
        SERVICE_PROVIDER_CREATE( ServiceProvider, &serviceProvider );

        SERVICE_PROVIDER_SETUP( serviceProvider );

        SERVICE_CREATE( NotificationService );
        SERVICE_CREATE( OptionsService );
        SERVICE_CREATE( FactoryService );
        SERVICE_CREATE( UnicodeSystem );
        SERVICE_CREATE( StringizeService );
        SERVICE_CREATE( ArchiveService );
        SERVICE_CREATE( LoggerService );

        class MyLogger
            : public LoggerBase
        {
        public:
            void log( ELoggerLevel _level, uint32_t _flag, const char * _data, uint32_t _count ) override
            {
                (void)_level;
                (void)_flag;
                (void)_count;

                printf( "%s"
                    , _data
                );
            }
        };

        LOGGER_SERVICE()
            ->setVerboseLevel( LM_WARNING );

        LOGGER_SERVICE()
            ->registerLogger( Helper::makeFactorableUnique<MyLogger>() );

        LOGGER_WARNING( "Inititalizing Config Manager..." );

        SERVICE_CREATE( ConfigService );
        SERVICE_CREATE( ConverterService );
        SERVICE_CREATE( CodecService );
        SERVICE_CREATE( DataService );
        SERVICE_CREATE( ThreadSystem );
        SERVICE_CREATE( ThreadService );
        SERVICE_CREATE( MemoryService );
        SERVICE_CREATE( ModuleService );
        SERVICE_CREATE( EnumeratorService );
        SERVICE_CREATE( PluginService );
        SERVICE_CREATE( PrototypeService );
        SERVICE_CREATE( VocabularyService );

        SERVICE_CREATE( Platform );
        SERVICE_CREATE( FileService );

        PLUGIN_CREATE( Win32FileGroup );
        PLUGIN_CREATE( Zip );
        PLUGIN_CREATE( LZ4 );
        PLUGIN_CREATE( ImageCodec );
        PLUGIN_CREATE( Movie1 );
        PLUGIN_CREATE( Movie );

        SERVICE_CREATE( LoaderService );
        PLUGIN_CREATE( MetabufLoader );

        PLUGIN_CREATE( DevelopmentConverter );

        //PLUGIN_SERVICE()
        //    ->loadPlugin( "DevelopmentConverterPlugin.dll" );

        PLUGIN_SERVICE()
            ->loadPlugin( "XmlToBinPlugin.dll" );

        if( FILE_SERVICE()
            ->mountFileGroup( ConstString::none(), nullptr, nullptr, FilePath::none(), STRINGIZE_STRING_LOCAL( "global" ), nullptr, false, MENGINE_DOCUMENT_FUNCTION ) == false )
        {
            return false;
        }

        ConstString dev = STRINGIZE_STRING_LOCAL( "dev" );

        if( FILE_SERVICE()
            ->mountFileGroup( dev, nullptr, nullptr, FilePath::none(), STRINGIZE_STRING_LOCAL( "global" ), nullptr, false, MENGINE_DOCUMENT_FUNCTION ) == false )
        {
            return false;
        }

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    static bool s_convert( const WString & _fromPath, const WString & _toPath, const WString & _convertType, const WString & _params )
    {
        MENGINE_UNUSED( _params );

        String utf8_fromPath;
        Helper::unicodeToUtf8( _fromPath, utf8_fromPath );

        String utf8_toPath;
        Helper::unicodeToUtf8( _toPath, utf8_toPath );

        String utf8_convertType;
        Helper::unicodeToUtf8( _convertType, utf8_convertType );

        ConverterOptions options;

        const FileGroupInterfacePtr & fileGroup = FILE_SERVICE()
            ->getDefaultFileGroup();

        options.fileGroup = fileGroup;
        options.inputFilePath = Helper::stringizeFilePath( utf8_fromPath );
        options.outputFilePath = Helper::stringizeFilePath( utf8_toPath );

        ConverterInterfacePtr converter = CONVERTER_SERVICE()
            ->createConverter( Helper::stringizeString( utf8_convertType ), MENGINE_DOCUMENT_FUNCTION );

        if( converter == nullptr )
        {
            LOGGER_ERROR( "can't create convert '%s'\nfrom: %s\nto: %s\n"
                , utf8_convertType.c_str()
                , options.inputFilePath.c_str()
                , options.outputFilePath.c_str()
            );

            return false;
        }

        converter->setOptions( &options );

        if( converter->convert() == false )
        {
            LOGGER_ERROR( "can't convert '%s'\nfrom: %s\nto: %s\n"
                , utf8_convertType.c_str()
                , options.inputFilePath.c_str()
                , options.outputFilePath.c_str()
            );

            return false;
        }

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    static bool convert( const wchar_t * fromPath, const wchar_t * toPath, const wchar_t * convertType, const wchar_t * params )
    {
        if( s_convert( fromPath, toPath, convertType, params ) == false )
        {
            LOGGER_ERROR( "convert: error process %ls to %ls convert %ls"
                , fromPath
                , toPath
                , convertType
            );

            return false;
        }

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    static PyObject * isAlphaInImageFile( pybind::kernel_interface * _kernel, const wchar_t * _path )
    {
        String utf8_path;
        if( Helper::unicodeToUtf8( _path, utf8_path ) == false )
        {
            _kernel->error_message( "invalid get utf8 path from '%ls'"
                , _path
            );

            return nullptr;
        }

        FilePath c_path = Helper::stringizeFilePath( utf8_path );

        const FileGroupInterfacePtr & fileGroup = FILE_SERVICE()
            ->getDefaultFileGroup();

        InputStreamInterfacePtr stream = Helper::openInputStreamFile( fileGroup, c_path, false, MENGINE_DOCUMENT_FUNCTION );

        if( stream == nullptr )
        {
            _kernel->error_message( "invalid open file '%s'"
                , c_path.c_str()
            );

            return nullptr;
        }

        const ConstString & codecType = CODEC_SERVICE()
            ->findCodecType( c_path );

        ImageDecoderInterfacePtr imageDecoder = CODEC_SERVICE()
            ->createDecoderT<ImageDecoderInterfacePtr>( codecType, MENGINE_DOCUMENT_FUNCTION );

        if( imageDecoder == nullptr )
        {
            _kernel->error_message( "file '%s' invalid create decoder '%s'"
                , c_path.c_str()
                , codecType.c_str()
            );

            return nullptr;
        }

        if( imageDecoder->prepareData( stream ) == false )
        {
            _kernel->error_message( "file '%s' invalid prepare data"
                , c_path.c_str()
            );

            return nullptr;
        }

        const ImageCodecDataInfo * dataInfo = imageDecoder->getCodecDataInfo();

        bool isAlpha = (dataInfo->channels == 4);

        return _kernel->ret_bool( isAlpha );
    }
    //////////////////////////////////////////////////////////////////////////
    static PyObject * isUselessAlphaInImageFile( pybind::kernel_interface * _kernel, const wchar_t * _path )
    {
        String utf8_path;
        if( Helper::unicodeToUtf8( _path, utf8_path ) == false )
        {
            LOGGER_ERROR( "isUselessAlphaInImageFile %ls invalid unicodeToUtf8"
                , _path
            );

            return nullptr;
        }

        FilePath c_path = Helper::stringizeFilePath( utf8_path );

        ImagePtr image = newImage();

        if( image->load( c_path ) == false )
        {
            LOGGER_ERROR( "invalid load '%ls'"
                , _path
            );

            return nullptr;
        }

        if( image->getChannels() != 4 )
        {
            return _kernel->ret_true();
        }

        bool useless = image->uselessalpha();

        return _kernel->ret_bool( useless );
    }
    //////////////////////////////////////////////////////////////////////////
    static ImagePtr loadImage( const wchar_t * _path )
    {
        String utf8_path;
        if( Helper::unicodeToUtf8( _path, utf8_path ) == false )
        {
            return nullptr;
        }

        FilePath c_path = Helper::stringizeFilePath( utf8_path );

        ImagePtr image = newImage();

        if( image->load( c_path ) == false )
        {
            return nullptr;
        }

        return image;
    }
    //////////////////////////////////////////////////////////////////////////
    static bool saveImage( const ImagePtr & _image, const wchar_t * _path )
    {
        String utf8_path;
        if( Helper::unicodeToUtf8( _path, utf8_path ) == false )
        {
            return false;
        }

        FilePath c_path = Helper::stringizeFilePath( utf8_path );

        if( _image->save( c_path ) == false )
        {
            return false;
        }

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    static Color color_convert( pybind::kernel_interface * _kernel, PyObject * _obj )
    {
        if( _kernel->tuple_check( _obj ) == true )
        {
            if( _kernel->tuple_size( _obj ) == 4 )
            {
                float r = pybind::tuple_getitem_t( _kernel, _obj, 0 );
                float g = pybind::tuple_getitem_t( _kernel, _obj, 1 );
                float b = pybind::tuple_getitem_t( _kernel, _obj, 2 );
                float a = pybind::tuple_getitem_t( _kernel, _obj, 3 );

                return Color( r, g, b, a );
            }
            else if( _kernel->tuple_size( _obj ) == 3 )
            {
                float r = pybind::tuple_getitem_t( _kernel, _obj, 0 );
                float g = pybind::tuple_getitem_t( _kernel, _obj, 1 );
                float b = pybind::tuple_getitem_t( _kernel, _obj, 2 );
                float a = 1.f;

                return Color( r, g, b, a );
            }
        }
        else if( _kernel->list_check( _obj ) == true )
        {
            if( _kernel->list_size( _obj ) == 4 )
            {
                float r = pybind::list_getitem_t( _kernel, _obj, 0 );
                float g = pybind::list_getitem_t( _kernel, _obj, 1 );
                float b = pybind::list_getitem_t( _kernel, _obj, 2 );
                float a = pybind::list_getitem_t( _kernel, _obj, 3 );

                return Color( r, g, b, a );
            }
            else if( _kernel->list_size( _obj ) == 3 )
            {
                float r = pybind::list_getitem_t( _kernel, _obj, 0 );
                float g = pybind::list_getitem_t( _kernel, _obj, 1 );
                float b = pybind::list_getitem_t( _kernel, _obj, 2 );
                float a = 1.f;

                return Color( r, g, b, a );
            }
        }

        return Color();
    }
    //////////////////////////////////////////////////////////////////////////
    ImagePtr createImage( pybind::kernel_interface * _kernel, uint32_t _width, uint32_t _height, uint32_t _channel, PyObject * _colour )
    {
        ImagePtr image = newImage();

        image->create( _width, _height, _channel );
        image->fill( color_convert( _kernel, _colour ) );

        return image;
    }
    //////////////////////////////////////////////////////////////////////////
    WString pathSHA1( const WChar * _path )
    {
        String utf8_path;
        if( Helper::unicodeToUtf8( _path, utf8_path ) == false )
        {
            return WString();
        }

        FilePath c_path = Helper::stringizeFilePath( utf8_path );

        const FileGroupInterfacePtr & fileGroup = FILE_SERVICE()
            ->getDefaultFileGroup();

        InputStreamInterfacePtr stream = Helper::openInputStreamFile( fileGroup, c_path, false, MENGINE_DOCUMENT_FUNCTION );

        if( stream == nullptr )
        {
            LOGGER_ERROR( "'%ls' invalid open '%s'"
                , _path
                , c_path.c_str()
            );

            return WString();
        }

        size_t size = stream->size();

        uint8_t * buf = new uint8_t[size];

        stream->read( buf, size );

        unsigned char hash[20];
        stdex::sha1_calc( buf, (int)size, hash );

        delete[] buf;

        char hex[41];
        stdex::sha1_hex( hash, hex );

        return WString( hex, hex + 40 );
    }
    //////////////////////////////////////////////////////////////////////////
    class PythonLogger
    {
    public:
        PythonLogger()
            : m_softspace( 0 )
        {
        }

        ~PythonLogger()
        {
        }

    public:
        void write( const char * _msg, uint32_t _size )
        {
            LOGGER_SERVICE()
                ->logMessage( LM_ERROR, 0, _msg, _size );
        }

    public:
        PyObject * py_write( pybind::kernel_interface * _kernel, PyObject * _args, PyObject * _kwds )
        {
            (void)_kwds;

            if( _kernel->tuple_check( _args ) == false )
            {
                return _kernel->ret_none();
            }

            uint32_t tuple_size = _kernel->tuple_size( _args );

            if( tuple_size == 0 )
            {
                return _kernel->ret_none();
            }

            PyObject * arg = _kernel->tuple_getitem( _args, 0 );

            if( _kernel->string_check( arg ) == true )
            {
                uint32_t size;
                const char * str = _kernel->string_to_char_and_size( arg, size );

                this->write( str, size );
            }
            else if( _kernel->unicode_check( arg ) == true )
            {
                uint32_t size;
                const char * utf8 = _kernel->unicode_to_utf8_and_size( arg, size );

                this->write( utf8, size );
            }

            return _kernel->ret_none();
        }

        PyObject * py_flush( pybind::kernel_interface * _kernel, PyObject * _args, PyObject * _kwds )
        {
            (void)_args;
            (void)_kwds;

            return _kernel->ret_none();
        }

    public:
        void setSoftspace( int _softspace )
        {
            m_softspace = _softspace;
        }

        int getSoftspace() const
        {
            return m_softspace;
        }

        const wchar_t * getErrors() const
        {
            return L"strict";
        }

        void setErrors( const wchar_t * _errors )
        {
            printf( "setErrors %ls\n"
                , _errors
            );
        }

        const wchar_t * getEncoding() const
        {
            return L"ascii";
        }

        void setEncoding( const wchar_t * _encoding ) const
        {
            printf( "setEncoding %ls\n"
                , _encoding
            );
        }

    protected:
        int m_softspace;
    };
}
//////////////////////////////////////////////////////////////////////////
static void s_error( const wchar_t * _msg )
{
    LOGGER_ERROR( "%ls"
        , _msg
    );
}
//////////////////////////////////////////////////////////////////////////
static bool getCurrentUserRegValue( const WCHAR * _path, const WCHAR * _key, WCHAR * _value, DWORD _size )
{
    HKEY hKey;
    LONG lRes = RegOpenKeyEx( HKEY_CURRENT_USER, _path, 0, KEY_READ, &hKey );  // Check Python x32
    if( lRes == ERROR_FILE_NOT_FOUND )
    {
#ifdef _MSC_VER
        lRes = RegOpenKeyEx( HKEY_CURRENT_USER, _path, 0, KEY_READ | KEY_WOW64_64KEY, &hKey );  // Check Python x64
#endif
    }

    if( lRes != ERROR_SUCCESS )
    {
        LOGGER_ERROR( "getCurrentUserRegValue %ls RegOpenKeyEx get Error %d"
            , _path
            , lRes
        );

        return false;
    }

    DWORD dwBufferSize = _size;
    LONG nError = ::RegQueryValueEx( hKey, _key, 0, NULL, (LPBYTE)_value, &dwBufferSize );

    RegCloseKey( hKey );

    if( nError != ERROR_SUCCESS )
    {
        LOGGER_ERROR( "getCurrentUserRegValue %ls RegQueryValueEx get Error %d"
            , _path
            , nError
        );

        return false;
    }

    return true;
}
//////////////////////////////////////////////////////////////////////////
static bool filterCurrentUserRegValue( const WCHAR * _path, const std::function<void( const WCHAR *, DWORD )> & _filter )
{
    HKEY hKey;
    LONG lOpenRes = RegOpenKeyEx( HKEY_CURRENT_USER, _path, 0, KEY_READ, &hKey );  // Check Python x32
    if( lOpenRes == ERROR_FILE_NOT_FOUND )
    {
#ifdef _MSC_VER
        lOpenRes = RegOpenKeyEx( HKEY_CURRENT_USER, _path, 0, KEY_READ | KEY_WOW64_64KEY, &hKey );  // Check Python x64
#endif
    }

    if( lOpenRes != ERROR_SUCCESS )
    {
        LOGGER_ERROR( "getCurrentUserRegValue %ls RegOpenKeyEx get Error %d"
            , _path
            , lOpenRes
        );

        return false;
    }

    for( DWORD index = 0;; ++index )
    {
        WCHAR wcKeyValue[256];
        DWORD wcKeyValueSize = 256;
        LONG lEnumRes = ::RegEnumKeyEx( hKey, index, wcKeyValue, &wcKeyValueSize, 0, NULL, NULL, NULL );

        if( lEnumRes != ERROR_SUCCESS )
        {
            break;
        }

        _filter( wcKeyValue, wcKeyValueSize );
    }

    RegCloseKey( hKey );

    return true;
}
//////////////////////////////////////////////////////////////////////////
struct extract_String_type
    : public pybind::type_cast_result<Mengine::String>
{
public:
    bool apply( pybind::kernel_interface * _kernel, PyObject * _obj, Mengine::String & _value, bool _nothrow ) override
    {
        (void)_kernel;
        (void)_nothrow;

        if( _kernel->string_check( _obj ) == false )
        {
            return false;
        }

        uint32_t size;
        const Mengine::String::value_type * string_char = _kernel->string_to_char_and_size( _obj, size );

        if( string_char == nullptr )
        {
            return false;
        }

        _value.assign( string_char, size );

        return true;
    }

public:
    PyObject * wrap( pybind::kernel_interface * _kernel, pybind::type_cast_result<Mengine::String>::TCastRef _value ) override
    {
        (void)_kernel;

        const Mengine::String::value_type * value_str = _value.c_str();
        Mengine::String::size_type value_size = _value.size();

        PyObject * py_value = _kernel->string_from_char_size( value_str, (uint32_t)value_size );

        return py_value;
    }
};
//////////////////////////////////////////////////////////////////////////
struct extract_WString_type
    : public pybind::type_cast_result<Mengine::WString>
{
public:
    bool apply( pybind::kernel_interface * _kernel, PyObject * _obj, Mengine::WString & _value, bool _nothrow ) override
    {
        (void)_kernel;
        (void)_nothrow;

        if( _kernel->unicode_check( _obj ) == false )
        {
            return false;
        }

        uint32_t size = 0;
        const Mengine::WString::value_type * value_char = _kernel->unicode_to_wchar_and_size( _obj, size );

        if( value_char == nullptr )
        {
            return false;
        }

        _value.assign( value_char, size );

        return true;
    }

public:
    PyObject * wrap( pybind::kernel_interface * _kernel, pybind::type_cast_result<Mengine::WString>::TCastRef _value ) override
    {
        const Mengine::WString::value_type * value_str = _value.c_str();
        Mengine::WString::size_type value_size = _value.size();

        PyObject * py_value = _kernel->unicode_from_wchar_size( value_str, (uint32_t)value_size );

        return py_value;
    }
};
//////////////////////////////////////////////////////////////////////////
static void* my_stdex_malloc( void *ctx, size_t size )
{
    (void)ctx;
    if( size == 0 )
        size = 1;

    return stdex_malloc( size, "python3" );
}
//////////////////////////////////////////////////////////////////////////
static void* my_stdex_calloc( void *ctx, size_t nelem, size_t elsize )
{
    (void)ctx;
    if( nelem == 0 || elsize == 0 )
    {
        nelem = 1;
        elsize = 1;
    }

    return stdex_calloc( nelem, elsize, "python3" );
}
//////////////////////////////////////////////////////////////////////////
static void* my_stdex_realloc( void *ctx, void *ptr, size_t new_size )
{
    (void)ctx;
    if( new_size == 0 )
        new_size = 1;

    void* ptr2 = stdex_realloc( ptr, new_size, "python3" );

    if( ptr2 == nullptr )
    {
        return nullptr;
    }

    return ptr2;
}
//////////////////////////////////////////////////////////////////////////
static void my_stdex_free( void *ctx, void *ptr )
{
    (void)ctx;

    stdex_free( ptr, "python3" );
}
//////////////////////////////////////////////////////////////////////////
bool run()
{
    try
    {
        if( Mengine::initialize() == false )
        {
            printf( "PyInit_ToolsBuilderPlugin initialize failed\n"
            );

            return false;
        }
    }
    catch( const std::exception & se )
    {
        char MSG[2048];
        sprintf( MSG, "PyInit_ToolsBuilderPlugin exception %s"
            , se.what()
        );

        MessageBoxA( NULL, MSG, "Error", MB_OK );

        return false;
    }

    LOGGER_ERROR( "initialize complete" );

    const WCHAR * szPythonVersionRegPath = L"SOFTWARE\\Python\\PythonCore";

    std::vector<std::wstring> vPythonVersions;
    if( filterCurrentUserRegValue( szPythonVersionRegPath, [&vPythonVersions]( const WCHAR * _value, DWORD _size )
    {
        vPythonVersions.emplace_back( std::wstring( _value, _size ) );
    } ) == false )
    {
        LOGGER_ERROR( "invalid enum reg value '%ls'"
            , szPythonVersionRegPath
        );

        return false;
    }

    struct PythonDesc
    {
        uint32_t major_version;
        uint32_t minor_version;

        WCHAR szPythonPath[512];
    };

    std::vector<PythonDesc> pythonDescs;
    for( const std::wstring & version : vPythonVersions )
    {
        PythonDesc desc;

        WCHAR szRegPath[512];
        wsprintf( szRegPath, L"SOFTWARE\\Python\\PythonCore\\%ls\\PythonPath"
            , version.c_str()
        );        

        if( getCurrentUserRegValue( szRegPath, L"", desc.szPythonPath, 512 ) == false )
        {
            continue;
        }

        WCHAR szRegSysVersion[512];
        wsprintf( szRegSysVersion, L"SOFTWARE\\Python\\PythonCore\\%ls"
            , version.c_str()
        );

        WCHAR szSysVersion[512];
        if( getCurrentUserRegValue( szRegSysVersion, L"SysVersion", szSysVersion, 512 ) == false )
        {
            continue;
        }

        ::swscanf( szSysVersion, L"%u.%u"
            , &desc.major_version
            , &desc.minor_version
        );

        pythonDescs.emplace_back( desc );
    }

    std::sort( pythonDescs.begin(), pythonDescs.end(), []( const PythonDesc & _l, const PythonDesc & _r )
    {
        uint32_t lk = _l.major_version * 100 + _l.minor_version;
        uint32_t rk = _r.major_version * 100 + _r.minor_version;

        return lk < rk;
    } );

    PythonDesc & desc = pythonDescs.front();

    WCHAR * szPythonPath = desc.szPythonPath;

    if( ::wcslen( szPythonPath ) == 0 )
    {
        LOGGER_ERROR( "invalid found python path" );

        return false;
    }

    LOGGER_ERROR( "python path: %ls"
        , szPythonPath
    );

    pybind::kernel_allocator_t stdex_alloc;
    stdex_alloc.ctx = nullptr;
    stdex_alloc.malloc = &my_stdex_malloc;
    stdex_alloc.calloc = &my_stdex_calloc;
    stdex_alloc.realloc = &my_stdex_realloc;
    stdex_alloc.free = &my_stdex_free;

    pybind::kernel_domain_allocator_t da;
    da.raw = &stdex_alloc;
    da.mem = &stdex_alloc;
    da.obj = &stdex_alloc;

    pybind::kernel_interface * kernel = pybind::initialize( &da, szPythonPath, false, false, false );

    if( kernel == nullptr )
    {
        return false;
    }

    LOGGER_ERROR( "pybind::initialize complete" );

    PyObject * py_tools_module = kernel->module_init( "ToolsBuilderPlugin" );

    pybind::registration_type_cast<Mengine::String>(kernel, pybind::make_type_cast<extract_String_type>());
    pybind::registration_type_cast<Mengine::WString>(kernel, pybind::make_type_cast<extract_WString_type>());

    pybind::registration_stl_vector_type_cast<Mengine::String, Mengine::Vector<Mengine::String>>(kernel);
    pybind::registration_stl_vector_type_cast<Mengine::WString, Mengine::Vector<Mengine::WString>>(kernel);

    pybind::interface_<Mengine::PythonLogger>( kernel, "XlsScriptLogger", true, py_tools_module )
        .def_native_kernel( "write", &Mengine::PythonLogger::py_write )
        .def_native_kernel( "flush", &Mengine::PythonLogger::py_flush )
        .def_property( "softspace", &Mengine::PythonLogger::getSoftspace, &Mengine::PythonLogger::setSoftspace )
        .def_property( "errors", &Mengine::PythonLogger::getErrors, &Mengine::PythonLogger::setErrors )
        .def_property( "encoding", &Mengine::PythonLogger::getEncoding, &Mengine::PythonLogger::setEncoding )
        ;
    Mengine::PythonLogger * logger = new Mengine::PythonLogger();
    PyObject * py_logger = pybind::ptr( kernel, logger );

    kernel->setStdOutHandle( py_logger );
    kernel->setStdErrorHandle( py_logger );

    pybind::def_function_kernel( kernel, "spreadingPngAlpha", &Mengine::spreadingPngAlpha, py_tools_module );
    pybind::def_function_kernel( kernel, "writeBin", &Mengine::writeBin, py_tools_module );
    pybind::def_function_kernel( kernel, "writeAek", &Mengine::writeAek, py_tools_module );


    pybind::def_function( kernel, "convert", &Mengine::convert, py_tools_module );
    pybind::def_function_kernel( kernel, "isAlphaInImageFile", &Mengine::isAlphaInImageFile, py_tools_module );
    pybind::def_function_kernel( kernel, "isUselessAlphaInImageFile", &Mengine::isUselessAlphaInImageFile, py_tools_module );

    Mengine::Image::embedding( kernel, py_tools_module );

    pybind::def_function( kernel, "loadImage", &Mengine::loadImage, py_tools_module );
    pybind::def_function( kernel, "saveImage", &Mengine::saveImage, py_tools_module );
    pybind::def_function_kernel( kernel, "createImage", &Mengine::createImage, py_tools_module );

    pybind::def_function( kernel, "pathSHA1", &Mengine::pathSHA1, py_tools_module );

    PyObject * module_builtins = kernel->get_builtins();

    pybind::def_function( kernel, "Error", &s_error, module_builtins );

    kernel->incref( py_tools_module );
    kernel->module_addobject( module_builtins, "ToolsBuilderPlugin", py_tools_module );

    Mengine::WChar currentDirectory[MAX_PATH];

    if( ::GetCurrentDirectory( MAX_PATH, currentDirectory ) == 0 )
    {
        return false;
    }

    PyObject * py_syspath = kernel->list_new( 0 );

    {
#ifdef _MSC_VER
        WCHAR * ch_buffer;
        WCHAR * ch = wcstok( szPythonPath, L";", &ch_buffer );
#elif __MINGW32__
        WCHAR * ch = wcstok( szPythonPath, L";" );
#else
#   error unssuport
#endif

        while( ch != NULL )
        {
            PyObject * py_stdpath = pybind::ptr( kernel, ch );
            kernel->list_appenditem( py_syspath, py_stdpath );
            kernel->decref( py_stdpath );

#ifdef _MSC_VER
            ch = wcstok( NULL, L";", &ch_buffer );
#elif __MINGW32__
            ch = wcstok( NULL, L";" );
#else
#   error unssuport
#endif
        }
    }

    PyObject * py_currentpath = pybind::ptr( kernel, currentDirectory );
    kernel->list_appenditem( py_syspath, py_currentpath );
    kernel->decref( py_currentpath );

    kernel->set_syspath( py_syspath );

    kernel->decref( py_syspath );

    LPWSTR lpwCmdLine = GetCommandLineW();


    int nArgs = 0;
    LPWSTR * szArglist = CommandLineToArgvW( lpwCmdLine, &nArgs );

    if( nArgs < 3 )
    {
        return false;
    }

    PWSTR szModuleName = szArglist[1];

    Mengine::String utf8_ModuleName;
    Mengine::Helper::unicodeToUtf8( szModuleName, utf8_ModuleName );

    PWSTR szFunctionName = szArglist[2];

    Mengine::String utf8_FunctionName;
    Mengine::Helper::unicodeToUtf8( szFunctionName, utf8_FunctionName );

    LOGGER_ERROR( "Module '%s' Function '%s'"
        , utf8_ModuleName.c_str()
        , utf8_FunctionName.c_str()
    );

    bool exist_run;
    PyObject * py_run_module = kernel->module_import( utf8_ModuleName.c_str(), exist_run );

    if( py_run_module == nullptr )
    {
        LOGGER_ERROR( "Module '%s' Function '%s' error"
            , utf8_ModuleName.c_str()
            , utf8_FunctionName.c_str()
        );

        return false;
    }

    PyObject * py_args = kernel->tuple_new( nArgs - 3 );

    for( int i = 3; i != nArgs; ++i )
    {
        LPWSTR arg = szArglist[i];

        pybind::tuple_setitem_t( kernel, py_args, i - 3, arg );
    }

    PyObject * py_result = kernel->ask_method_native( py_run_module, utf8_FunctionName.c_str(), py_args );

    kernel->decref( py_args );

    bool result;
    if( kernel->extract_bool( py_result, result ) == false )
    {
        return false;
    }

    return result;
}
///////////////////////////////////////////////////////////////////////////////////
#ifdef _MSC_VER
int CALLBACK WinMain( _In_  HINSTANCE hInstance, _In_  HINSTANCE hPrevInstance, _In_  LPSTR lpCmdLine, _In_  int nCmdShow )
#elif __MINGW32__
int main( int argc, char *argv[] )
#else
#   error unsupport
#endif
{
#ifdef _MSC_VER
    ( void )hInstance;
    (void)hPrevInstance;
    (void)lpCmdLine;
    (void)nCmdShow;
#elif __MINGW32__
    ( void )argc;
    (void)argv;
#else
#   error unsupport
#endif

    Mengine::createConsole();

    try
    {
        if( run() == false )
        {
            return 1;
        }
    }
    catch( const std::exception & se )
    {
        const char * se_what = se.what();

        MessageBoxA( NULL, se_what, "Mengine exception", MB_OK );

        return 1;
    }

    return 0;
}