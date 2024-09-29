#include "Interface/ServiceInterface.h"

#include "Interface/StringizeServiceInterface.h"
#include "Interface/UnicodeSystemInterface.h"
#include "Interface/ArchivatorInterface.h"
#include "Interface/LoggerInterface.h"
#include "Interface/DataInterface.h"
#include "Interface/MemoryInterface.h"
#include "Interface/EnumeratorServiceInterface.h"
#include "Interface/ConverterInterface.h"
#include "Interface/FileServiceInterface.h"
#include "Interface/PluginInterface.h"
#include "Interface/UnicodeSystemInterface.h"
#include "Interface/ImageCodecInterface.h"
#include "Interface/ThreadSystemInterface.h"
#include "Interface/ConfigServiceInterface.h"
#include "Interface/OptionsServiceInterface.h"
#include "Interface/CodecServiceInterface.h"
#include "Interface/VocabularyServiceInterface.h"
#include "Interface/PluginServiceInterface.h"
#include "Interface/LoggerServiceInterface.h"
#include "Interface/ConverterServiceInterface.h"
#include "Interface/RenderMaterialServiceInterface.h"

#include "Environment/Windows/WindowsIncluder.h"
#include "Environment/Windows/Win32Helper.h"

#include "XmlToBinDecoder.h"
#include "XmlToAekConverter.h"
#include "Image.h"

#include "Kernel/String.h"
#include "Kernel/ConverterFactory.h"
#include "Kernel/LoggerBase.h"
#include "Kernel/Logger.h"
#include "Kernel/FilePathHelper.h"
#include "Kernel/FactoryDefault.h"
#include "Kernel/UnicodeHelper.h"
#include "Kernel/FileStreamHelper.h"
#include "Kernel/Vector.h"
#include "Kernel/MemoryAllocator.h"
#include "Kernel/AllocatorHelper.h"
#include "Kernel/PixelFormatHelper.h"
#include "Kernel/ContentHelper.h"
#include "Kernel/TimestampHelper.h"
#include "Kernel/ThreadHelper.h"

#include "ToolUtils/ToolLogger.h"

#include "Config/StdIO.h"

#include "pybind/pybind.hpp"
#include "pybind/stl/stl_type_cast.hpp"

#include "stdex/sha1.h"

#include <iostream>
#include <algorithm>

#include <io.h>

//////////////////////////////////////////////////////////////////////////
PLUGIN_EXPORT( ImageCodec );
PLUGIN_EXPORT( Movie );
PLUGIN_EXPORT( Zip );
PLUGIN_EXPORT( LZ4 );
PLUGIN_EXPORT( JPEG );
PLUGIN_EXPORT( PNG );
PLUGIN_EXPORT( WebP );
PLUGIN_EXPORT( Theora );
PLUGIN_EXPORT( OggVorbis );
PLUGIN_EXPORT( Metabuf );
//////////////////////////////////////////////////////////////////////////
SERVICE_PROVIDER_EXTERN( ServiceProvider );
//////////////////////////////////////////////////////////////////////////
SERVICE_EXTERN( AllocatorSystem );
SERVICE_EXTERN( DocumentService );
SERVICE_EXTERN( PlatformSystem );
SERVICE_EXTERN( PlatformService );
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
SERVICE_EXTERN( TimeSystem );
SERVICE_EXTERN( ThreadSystem );
SERVICE_EXTERN( DateTimeSystem );
SERVICE_EXTERN( ThreadService );
SERVICE_EXTERN( MemoryService );
SERVICE_EXTERN( TimelineService );
SERVICE_EXTERN( TimepipeService );
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
        typedef BOOL( WINAPI * PATTACHCONSOLE )(DWORD);

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
        else
        {
            return;
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
        ServiceProviderInterface * serviceProvider;
        SERVICE_PROVIDER_CREATE( ServiceProvider, &serviceProvider );

        SERVICE_PROVIDER_SETUP( serviceProvider );

        SERVICE_CREATE( AllocatorSystem, nullptr );
        SERVICE_CREATE( StringizeService, nullptr );
        SERVICE_CREATE( DocumentService, nullptr );
        
        SERVICE_CREATE( PlatformSystem, MENGINE_DOCUMENT_FUNCTION );
        SERVICE_CREATE( TimeSystem, MENGINE_DOCUMENT_FUNCTION );
        SERVICE_CREATE( ThreadSystem, MENGINE_DOCUMENT_FUNCTION );
        SERVICE_CREATE( UnicodeSystem, MENGINE_DOCUMENT_FUNCTION );
        SERVICE_CREATE( DateTimeSystem, MENGINE_DOCUMENT_FUNCTION );
        
        SERVICE_CREATE( EnumeratorService, MENGINE_DOCUMENT_FUNCTION );
        SERVICE_CREATE( NotificationService, MENGINE_DOCUMENT_FUNCTION );
        SERVICE_CREATE( OptionsService, MENGINE_DOCUMENT_FUNCTION );
        SERVICE_CREATE( FactoryService, MENGINE_DOCUMENT_FUNCTION );
        
        SERVICE_CREATE( ArchiveService, MENGINE_DOCUMENT_FUNCTION );
        SERVICE_CREATE( LoggerService, MENGINE_DOCUMENT_FUNCTION );

        LOGGER_SERVICE()
            ->setVerboseLevel( LM_WARNING );

        LOGGER_SERVICE()
            ->registerLogger( Helper::makeFactorableUnique<ToolLogger>( MENGINE_DOCUMENT_FUNCTION ) );

        LOGGER_WARNING( "Inititalizing Config Manager..." );

        SERVICE_CREATE( ConverterService, MENGINE_DOCUMENT_FUNCTION );
        SERVICE_CREATE( CodecService, MENGINE_DOCUMENT_FUNCTION );
        SERVICE_CREATE( DataService, MENGINE_DOCUMENT_FUNCTION );
        SERVICE_CREATE( ThreadService, MENGINE_DOCUMENT_FUNCTION );
        SERVICE_CREATE( MemoryService, MENGINE_DOCUMENT_FUNCTION );
        SERVICE_CREATE( EnumeratorService, MENGINE_DOCUMENT_FUNCTION );
        SERVICE_CREATE( TimelineService, MENGINE_DOCUMENT_FUNCTION );
        SERVICE_CREATE( TimepipeService, MENGINE_DOCUMENT_FUNCTION );
        SERVICE_CREATE( ModuleService, MENGINE_DOCUMENT_FUNCTION );
        SERVICE_CREATE( PluginService, MENGINE_DOCUMENT_FUNCTION );
        SERVICE_CREATE( PrototypeService, MENGINE_DOCUMENT_FUNCTION );
        SERVICE_CREATE( VocabularyService, MENGINE_DOCUMENT_FUNCTION );

        SERVICE_CREATE( PlatformService, MENGINE_DOCUMENT_FUNCTION );

        SERVICE_CREATE( FileService, MENGINE_DOCUMENT_FUNCTION );
        SERVICE_CREATE( ConfigService, MENGINE_DOCUMENT_FUNCTION );
        SERVICE_CREATE( LoaderService, MENGINE_DOCUMENT_FUNCTION );

        PLUGIN_CREATE( Zip, MENGINE_DOCUMENT_FUNCTION );
        PLUGIN_CREATE( LZ4, MENGINE_DOCUMENT_FUNCTION );
        PLUGIN_CREATE( JPEG, MENGINE_DOCUMENT_FUNCTION );
        PLUGIN_CREATE( PNG, MENGINE_DOCUMENT_FUNCTION );
        PLUGIN_CREATE( WebP, MENGINE_DOCUMENT_FUNCTION );
        PLUGIN_CREATE( Theora, MENGINE_DOCUMENT_FUNCTION );
        PLUGIN_CREATE( OggVorbis, MENGINE_DOCUMENT_FUNCTION );
        PLUGIN_CREATE( ImageCodec, MENGINE_DOCUMENT_FUNCTION );
        PLUGIN_CREATE( Movie, MENGINE_DOCUMENT_FUNCTION );
        PLUGIN_CREATE( Metabuf, MENGINE_DOCUMENT_FUNCTION );

        if( PLUGIN_SERVICE()
            ->loadPlugin( "DevelopmentConverterPlugin.dll", MENGINE_DOCUMENT_FUNCTION ) == false )
        {
            return false;
        }

        if( PLUGIN_SERVICE()
            ->loadPlugin( "AstralaxPlugin.dll", MENGINE_DOCUMENT_FUNCTION ) == false )
        {
            return false;
        }

        if( PLUGIN_SERVICE()
            ->loadPlugin( "XmlToBinPlugin.dll", MENGINE_DOCUMENT_FUNCTION ) == false )
        {
            return false;
        }

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    static void finalize()
    {
        SERVICE_PROVIDER_STOP();

        PLUGIN_SERVICE()
            ->unloadPlugins();

        SERVICE_FINALIZE( ConfigService );
        SERVICE_FINALIZE( FileService );
        SERVICE_FINALIZE( PlatformService );
        SERVICE_FINALIZE( PluginService );
        SERVICE_FINALIZE( ModuleService );
        SERVICE_FINALIZE( TimepipeService );
        SERVICE_FINALIZE( TimelineService );
        SERVICE_FINALIZE( EnumeratorService );
        SERVICE_FINALIZE( MemoryService );
        SERVICE_FINALIZE( ThreadService );
        SERVICE_FINALIZE( DataService );
        SERVICE_FINALIZE( CodecService );
        SERVICE_FINALIZE( ConverterService );
        SERVICE_FINALIZE( LoggerService );
        SERVICE_FINALIZE( ArchiveService );
        SERVICE_FINALIZE( FactoryService );
        SERVICE_FINALIZE( OptionsService );
        SERVICE_FINALIZE( PrototypeService );
        SERVICE_FINALIZE( VocabularyService );
        SERVICE_FINALIZE( NotificationService );
        SERVICE_FINALIZE( EnumeratorService );
        SERVICE_FINALIZE( DateTimeSystem );
        SERVICE_FINALIZE( UnicodeSystem );
        SERVICE_FINALIZE( ThreadSystem );
        SERVICE_FINALIZE( TimeSystem );
        SERVICE_FINALIZE( PlatformSystem );
    }
    //////////////////////////////////////////////////////////////////////////
    static bool s_convert( const WString & _fromPath, const WString & _toPath, const WString & _convertType, const Params & _params )
    {
        MENGINE_UNUSED( _params );

        String utf8_fromPath;
        Helper::unicodeToUtf8( _fromPath, &utf8_fromPath );

        String utf8_toPath;
        Helper::unicodeToUtf8( _toPath, &utf8_toPath );

        String utf8_convertType;
        Helper::unicodeToUtf8( _convertType, &utf8_convertType );

        ConverterOptions options;

        FileGroupInterfacePtr globalFileGroup = VOCABULARY_GET( STRINGIZE_STRING_LOCAL( "FileGroup" ), STRINGIZE_STRING_LOCAL( "dev" ) );

        ContentInterfacePtr inputContent = Helper::makeFileContent( globalFileGroup, Helper::stringizeFilePath( utf8_fromPath ), MENGINE_DOCUMENT_FUNCTION );
        ContentInterfacePtr outputContent = Helper::makeFileContent( globalFileGroup, Helper::stringizeFilePath( utf8_toPath ), MENGINE_DOCUMENT_FUNCTION );

        options.inputContent = inputContent;
        options.outputContent = outputContent;
        options.params = _params;

        ConverterInterfacePtr converter = CONVERTER_SERVICE()
            ->createConverter( Helper::stringizeString( utf8_convertType ), MENGINE_DOCUMENT_FUNCTION );

        if( converter == nullptr )
        {
            LOGGER_ERROR( "can't create convert '%s'\nfrom: %s\nto: %s\n"
                , utf8_convertType.c_str()
                , Helper::getContentFullPath( options.inputContent )
                , Helper::getContentFullPath( options.outputContent )
            );

            return false;
        }

        converter->setOptions( &options );

        if( converter->convert() == false )
        {
            LOGGER_ERROR( "can't convert '%s'\nfrom: %s\nto: %s\n"
                , utf8_convertType.c_str()
                , Helper::getContentFullPath( options.inputContent )
                , Helper::getContentFullPath( options.outputContent )
            );

            return false;
        }

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    static bool convert( const WString & fromPath, const WString & toPath, const WString & convertType, const Params & params )
    {
        if( s_convert( fromPath, toPath, convertType, params ) == false )
        {
            LOGGER_ERROR( "convert: error process %ls to %ls convert %ls"
                , fromPath.c_str()
                , toPath.c_str()
                , convertType.c_str()
            );

            return false;
        }

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    static PyObject * isAlphaInImageFile( pybind::kernel_interface * _kernel, const wchar_t * _path )
    {
        String utf8_path;
        if( Helper::unicodeToUtf8( _path, &utf8_path ) == false )
        {
            _kernel->error_message( "invalid get utf8 path from '%ls'"
                , _path
            );

            return nullptr;
        }

        FilePath c_path = Helper::stringizeFilePath( utf8_path );

        FileGroupInterfacePtr globalFileGroup = VOCABULARY_GET( STRINGIZE_STRING_LOCAL( "FileGroup" ), STRINGIZE_STRING_LOCAL( "dev" ) );

        InputStreamInterfacePtr stream = Helper::openInputStreamFile( globalFileGroup, c_path, false, false, MENGINE_DOCUMENT_FUNCTION );

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
            ->createDecoder( codecType, MENGINE_DOCUMENT_FUNCTION );

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

        uint32_t channels = Helper::getPixelFormatChannels( dataInfo->format );

        if( channels != 4 )
        {
            return _kernel->ret_false();
        }

        return _kernel->ret_true();
    }
    //////////////////////////////////////////////////////////////////////////
    static PyObject * isUselessAlphaInImageFile( pybind::kernel_interface * _kernel, const wchar_t * _path )
    {
        String utf8_path;
        if( Helper::unicodeToUtf8( _path, &utf8_path ) == false )
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
        if( Helper::unicodeToUtf8( _path, &utf8_path ) == false )
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
        if( Helper::unicodeToUtf8( _path, &utf8_path ) == false )
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
        if( Helper::unicodeToUtf8( _path, &utf8_path ) == false )
        {
            return WString();
        }

        FilePath c_path = Helper::stringizeFilePath( utf8_path );

        FileGroupInterfacePtr globalFileGroup = VOCABULARY_GET( STRINGIZE_STRING_LOCAL( "FileGroup" ), STRINGIZE_STRING_LOCAL( "dev" ) );

        InputStreamInterfacePtr stream = Helper::openInputStreamFile( globalFileGroup, c_path, false, false, MENGINE_DOCUMENT_FUNCTION );

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
        void write( const char * _data, size_t _size )
        {
            LoggerMessage msg;
            msg.timestamp = Helper::getLocalTimestamp();
            msg.category = "python";
            msg.threadName = Helper::getCurrentThreadName();
            msg.level = LM_ERROR;
            msg.flag = 0;
            msg.filter = 0;
            msg.color = LCOLOR_NONE;
            msg.function = nullptr;
            msg.line = 0;
            msg.data = _data;
            msg.size = _size;

            LOGGER_SERVICE()
                ->logMessage( msg );
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
                size_t size;
                const char * str = _kernel->string_to_char_and_size( arg, &size );

                this->write( str, size );
            }
            else if( _kernel->unicode_check( arg ) == true )
            {
                size_t size;
                const char * utf8 = _kernel->unicode_to_utf8_and_size( arg, &size );

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

        size_t size = 0;
        const Mengine::String::value_type * string_char = _kernel->string_to_char_and_size( _obj, &size );

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

        size_t size = 0;
        const Mengine::WString::value_type * value_char = _kernel->unicode_to_wchar_and_size( _obj, &size );

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
static bool s_ConstString_compare( pybind::kernel_interface * _kernel, PyObject * _obj, Mengine::ConstString * _self, PyObject * _compare, pybind::PybindOperatorCompare _op, bool & _result )
{
    MENGINE_UNUSED( _obj );

    Mengine::ConstString cs_compare;
    if( pybind::extract_value( _kernel, _compare, cs_compare, false ) == false )
    {
        return false;
    }

    switch( _op )
    {
    case pybind::POC_Less:
        {
            _result = *_self < cs_compare;
        }break;
    case pybind::POC_Lessequal:
        {
            _result = *_self <= cs_compare;
        }break;
    case pybind::POC_Equal:
        {
            _result = *_self == cs_compare;
        }break;
    case pybind::POC_Notequal:
        {
            _result = *_self != cs_compare;
        }break;
    case pybind::POC_Great:
        {
            _result = *_self > cs_compare;
        }break;
    case pybind::POC_Greatequal:
        {
            _result = *_self >= cs_compare;
        }break;
    }

    return true;
}
//////////////////////////////////////////////////////////////////////////
static const Mengine::Char * s_ConstString_repr( Mengine::ConstString * _cs )
{
    const Mengine::Char * str_repr = _cs->c_str();

    return str_repr;
}
//////////////////////////////////////////////////////////////////////////
static Mengine::ConstString::hash_type s_ConstString_hash( Mengine::ConstString * _cs )
{
    Mengine::ConstString::hash_type hash = _cs->hash();

    return hash;
}
//////////////////////////////////////////////////////////////////////////
static bool ConstString_convert( pybind::kernel_interface * _kernel, PyObject * _obj, void * _place, void * _user )
{
    MENGINE_UNUSED( _kernel );
    MENGINE_UNUSED( _user );

    Mengine::ConstString * cstr = (Mengine::ConstString *)_place;

    if( _kernel->string_check( _obj ) == true )
    {
        size_t size;
        const char * value = _kernel->string_to_char_and_size( _obj, &size );

        *cstr = Mengine::Helper::stringizeStringSize( value, size );

        return true;
    }
    else if( _kernel->is_none( _obj ) == true )
    {
        *cstr = Mengine::ConstString::none();

        return true;
    }
    else if( _kernel->unicode_check( _obj ) == true )
    {
        size_t size;
        const wchar_t * value = _kernel->unicode_to_wchar_and_size( _obj, &size );

        Mengine::String utf8_value;
        Mengine::Helper::unicodeToUtf8Size( value, size, &utf8_value );

        *cstr = Mengine::Helper::stringizeStringSize( utf8_value.c_str(), utf8_value.size() );

        return true;
    }

    return false;
}
//////////////////////////////////////////////////////////////////////////
namespace Detail
{
    class MyAllocator
        : public pybind::allocator_interface
    {
    protected:
        void * malloc( size_t _size ) override
        {
            void * p = Mengine::Helper::allocateMemory( _size, "python" );

            return p;
        }

        void * calloc( size_t _num, size_t _size ) override
        {
            void * p = Mengine::Helper::callocateMemory( _num, _size, "python" );

            return p;
        }

        void * realloc( void * _ptr, size_t _size ) override
        {
            void * p = Mengine::Helper::reallocateMemory( _ptr, _size, "python" );

            return p;
        }

        void free( void * _ptr ) override
        {
            Mengine::Helper::deallocateMemory( _ptr, "python" );
        }
    };
}
//////////////////////////////////////////////////////////////////////////
bool run()
{
    try
    {
        if( Mengine::initialize() == false )
        {
            printf( "PyInit_ToolsBuilderPlugin initialize failed\n" );

            return false;
        }
    }
    catch( const std::exception & se )
    {
        char MSG[MENGINE_LOGGER_MAX_MESSAGE] = {'\0'};

        MENGINE_SNPRINTF( MSG, MENGINE_LOGGER_MAX_MESSAGE, "PyInit_ToolsBuilderPlugin exception: %s"
            , se.what()
        );

        MessageBoxA( NULL, MSG, "Error", MB_OK );

        return false;
    }

    LOGGER_ERROR( "initialize complete" );

    /*
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

        WCHAR szRegPath[512] = {L'\0'};
        MENGINE_WNSPRINTF( szRegPath, 512, L"SOFTWARE\\Python\\PythonCore\\%ls\\PythonPath"
            , version.c_str()
        );

        if( getCurrentUserRegValue( szRegPath, L"", desc.szPythonPath, 512 ) == false )
        {
            continue;
        }

        WCHAR szRegSysVersion[512] = {L'\0'};
        MENGINE_WNSPRINTF( szRegSysVersion, 512, L"SOFTWARE\\Python\\PythonCore\\%ls"
            , version.c_str()
        );

        WCHAR szSysVersion[512] = {L'\0'};
        if( getCurrentUserRegValue( szRegSysVersion, L"SysVersion", szSysVersion, 512 ) == false )
        {
            continue;
        }

        MENGINE_SWSCANF( szSysVersion, L"%u.%u"
            , &desc.major_version
            , &desc.minor_version
        );

        pythonDescs.emplace_back( desc );
    }

    if( pythonDescs.empty() == true )
    {
        LOGGER_ERROR( "invalid found any python" );

        return false;
    }

    std::vector<PythonDesc>::iterator it_found = std::find_if( pythonDescs.begin(), pythonDescs.end(), []( const PythonDesc & _desc )
    {
        if( _desc.major_version == 3 && _desc.minor_version == 8 )
        {
            return true;
        }

        return false;
    } );

    if( it_found == pythonDescs.end() )
    {
        LOGGER_ERROR( "invalid found python 3.8" );

        return false;
    }    

    PythonDesc & desc = *it_found;

    WCHAR * szPythonPath = desc.szPythonPath;

    if( ::wcslen( szPythonPath ) == 0 )
    {
        LOGGER_ERROR( "invalid found python path" );

        return false;
    }
    */

    WCHAR currentPath[MENGINE_MAX_PATH] = {L'\0'};
    DWORD len = ::GetCurrentDirectory( MENGINE_MAX_PATH, currentPath );

    if( len == 0 )
    {
        return false;
    }

    currentPath[len + 0] = MENGINE_PATH_WDELIM;
    currentPath[len + 1] = L'\0';

    WCHAR exportPath[MENGINE_MAX_PATH] = {L'\0'};
    ::wcscpy( exportPath, currentPath );
    ::wcscat( exportPath, L"Python3Lib/" );

    WCHAR shortpath_stdPath[MENGINE_MAX_PATH] = {L'\0'};
    DWORD ShortPathNameLen = ::GetShortPathName( exportPath, shortpath_stdPath, MENGINE_MAX_PATH );

    if( ShortPathNameLen == 0 )
    {
        LOGGER_ERROR( "invalid GetShortPathName '%ls' get error %ls"
            , exportPath
            , Mengine::Helper::Win32GetLastErrorMessageW()
        );

        return false;
    }

    LOGGER_ERROR( "python path: %ls"
        , shortpath_stdPath
    );

    pybind::allocator_interface * allocator = Mengine::Helper::newT<Detail::MyAllocator>();

    pybind::kernel_interface * kernel = pybind::initialize( allocator, shortpath_stdPath, MENGINE_DEBUG_VALUE( true, false ), true, false );

    if( kernel == nullptr )
    {
        return false;
    }

    pybind::list py_syspath( kernel );
    py_syspath.append( currentPath );
    py_syspath.append( shortpath_stdPath );

    kernel->set_syspath( py_syspath.ptr() );

    LOGGER_ERROR( "pybind::initialize complete" );

    PyObject * py_tools_module = kernel->module_init( "ToolsBuilderPlugin" );

    pybind::registration_type_cast<Mengine::String>(kernel, pybind::make_type_cast<extract_String_type>(kernel));
    pybind::registration_type_cast<Mengine::WString>(kernel, pybind::make_type_cast<extract_WString_type>(kernel));

    pybind::registration_stl_vector_type_cast<Mengine::Vector<Mengine::String>>(kernel);
    pybind::registration_stl_vector_type_cast<Mengine::Vector<Mengine::WString>>(kernel);

    pybind::structhash_<Mengine::ConstString>( kernel, "ConstString", true, py_tools_module )
        .def_compare( &s_ConstString_compare )
        .def_convert( &ConstString_convert, nullptr )
        .def_repr( &s_ConstString_repr )
        .def_hash( &s_ConstString_hash )
        ;

    pybind::registration_stl_map_type_cast<Mengine::Params>(kernel);

    pybind::interface_<Mengine::PythonLogger>( kernel, "XlsScriptLogger", true, py_tools_module )
        .def_native_silent_kernel( "write", &Mengine::PythonLogger::py_write )
        .def_native_kernel( "flush", &Mengine::PythonLogger::py_flush )
        .def_property( "softspace", &Mengine::PythonLogger::getSoftspace, &Mengine::PythonLogger::setSoftspace )
        .def_property( "errors", &Mengine::PythonLogger::getErrors, &Mengine::PythonLogger::setErrors )
        .def_property( "encoding", &Mengine::PythonLogger::getEncoding, &Mengine::PythonLogger::setEncoding )
        ;

    Mengine::PythonLogger * logger = new Mengine::PythonLogger();
    PyObject * py_logger = pybind::ptr( kernel, logger );

    kernel->setStdOutHandle( py_logger );
    kernel->setStdErrorHandle( py_logger );

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

    LPWSTR lpwCmdLine = ::GetCommandLineW();

    int nArgs = 0;
    LPWSTR * szArglist = ::CommandLineToArgvW( lpwCmdLine, &nArgs );

    if( nArgs < 3 )
    {
        return false;
    }

    PWSTR szModuleName = szArglist[1];

    Mengine::String utf8_ModuleName;
    Mengine::Helper::unicodeToUtf8( szModuleName, &utf8_ModuleName );

    PWSTR szFunctionName = szArglist[2];

    Mengine::String utf8_FunctionName;
    Mengine::Helper::unicodeToUtf8( szFunctionName, &utf8_FunctionName );

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

    Mengine::Helper::deleteT( static_cast<Detail::MyAllocator *>(allocator) );

    Mengine::finalize();

    return result;
}
///////////////////////////////////////////////////////////////////////////////////
#ifdef _MSC_VER
int CALLBACK WinMain( _In_  HINSTANCE hInstance, _In_  HINSTANCE hPrevInstance, _In_  LPSTR lpCmdLine, _In_  int nCmdShow )
#elif __MINGW32__
int main( int argc, char * argv[] )
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
            return EXIT_FAILURE;
        }
    }
    catch( const std::exception & se )
    {
        const char * se_what = se.what();

        MessageBoxA( NULL, se_what, "Mengine exception", MB_OK );

        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}