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
#include "Interface/PrototypeServiceInterface.h"

#include "Environment/Windows/WindowsIncluder.h"
#include "Environment/Windows/Win32Helper.h"

#include "Plugins/Win32FindPython3Plugin/Win32FindPython3Interface.h"

#include "XmlToBinDecoder.h"
#include "XmlToAekConverter.h"
#include "Image.h"
#include "Convert.h"

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
#include "Kernel/TextureHelper.h"

#include "ToolUtils/ToolLogger.h"

#include "Config/StdIO.h"
#include "Config/StdString.h"

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
PLUGIN_EXPORT( PVRTC );
PLUGIN_EXPORT( WebP );
PLUGIN_EXPORT( Theora );
PLUGIN_EXPORT( OggVorbis );
PLUGIN_EXPORT( Metabuf );
PLUGIN_EXPORT( Win32FindPython3 );
PLUGIN_EXPORT( Win32FileGroup );
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
extern "C"
{
    struct _object;
    typedef _object PyObject;
}
//////////////////////////////////////////////////////////////////////////
#define METH_VARARGS 0x0001
//////////////////////////////////////////////////////////////////////////
typedef PyObject * (*PyCFunction)(PyObject *, PyObject *);
//////////////////////////////////////////////////////////////////////////
struct PyMethodDef
{
    const char * ml_name;   /* The name of the built-in function/method */
    PyCFunction ml_meth;    /* The C function that implements it */
    int         ml_flags;   /* Combination of METH_xxx flags, which mostly
                               describe the args expected by the C func */
    const char * ml_doc;    /* The __doc__ attribute, or NULL */
};
typedef int64_t Py_ssize_t;
//////////////////////////////////////////////////////////////////////////
typedef void (*PY_INITIALIZEEX)(int);
typedef wchar_t * (*PY_GETPATH)(void);
typedef void (*PY_SETPATH)(const wchar_t *);
typedef PyObject * (*PYIMPORT_ADDMODULE)(const char *);
typedef PyObject * (*PYCFUNCTION_NEW)(PyMethodDef *, PyObject *);
typedef int (*PYMODULE_ADDOBJECT)(PyObject *, const char *, PyObject *);
typedef PyObject * (*PYIMPROT_IMPORTMODULE)(const char *);
typedef int (*PYARG_PARSETUPLE)(PyObject *, const char *, ...);
typedef int (*PYDICT_NEXT)(PyObject *, Py_ssize_t *, PyObject **, PyObject **);
typedef const char * (*PYUNICODE_ASUTF8ANDSIZE)(PyObject *, Py_ssize_t *);
typedef Py_ssize_t( *PYUNICODE_ASWIDECHAR )(PyObject *, wchar_t *, Py_ssize_t);
typedef PyObject * (*PYUNICODE_FROMWIDECHAR)(const wchar_t *, Py_ssize_t);
typedef PyObject * (*PYUNICODE_JOIN)(PyObject *, PyObject *);
typedef PyObject * (*PYBOOL_FROMLONG)(long);
typedef PyObject * (*PYCAPSULE_NEW)(void *, const char *, void (*)(PyObject *));
typedef void * (*PYCAPSULE_GETPOINTER)(PyObject *, const char *);
typedef long long (*PYLONG_ASUNSIGNEDLONGLONG)(PyObject *);
typedef PyObject * (*PYLONG_FROMUNSIGNEDLONGLONG)(long long);
typedef PyObject * (*PYLIST_NEW)(Py_ssize_t);
typedef int (*PYLIST_SETITEM)(PyObject *, Py_ssize_t, PyObject *);
typedef PyObject * (*PYLIST_GETITEM)(PyObject *, Py_ssize_t);
typedef PyObject * (*PYTUPLE_PACK)(Py_ssize_t, ...);
typedef PyObject * (*PYTUPLE_NEW)(Py_ssize_t);
typedef int (*PYTUPLE_SETITEM)(PyObject *, Py_ssize_t, PyObject *);
typedef PyObject * (*PYERR_OCCURRED)(void);
typedef void (*PYERR_PRINT)(void);
typedef void (*PYERR_FETCH)(PyObject **, PyObject **, PyObject **);
typedef void (*PYERR_NORMALIZEEXCEPTION)(PyObject **, PyObject **, PyObject **);
typedef PyObject * (*PYOBJECT_STR)(PyObject *);
typedef PyObject * (*PYOBJECT_CALLOBJECT)(PyObject *, PyObject *);
typedef PyObject * (*PYOBJECT_GETATTRSTRING)(PyObject *, const char *);
typedef int (*PYOBJECT_SETATTRSTRING)(PyObject *, const char *, PyObject *);
typedef PyObject * (*PYOBJECT_CALLFUNCTIONOBJARGS)(PyObject * callable, ...);
typedef int (*PYOBJECT_ISTRUE)(PyObject *);
typedef int (*PYSYS_SETOBJECT)(const char *, PyObject *);
typedef PyObject * (*PYDICT_NEW)(void);
typedef int (*PYDICT_SETITEM)(PyObject *, PyObject *, PyObject *);
//////////////////////////////////////////////////////////////////////////
PY_INITIALIZEEX Py_InitializeEx;
PY_GETPATH Py_GetPath;
PY_SETPATH Py_SetPath;
PYIMPORT_ADDMODULE PyImport_AddModule;
PYCFUNCTION_NEW PyCFunction_New;
PYMODULE_ADDOBJECT PyModule_AddObject;
PYIMPROT_IMPORTMODULE PyImport_ImportModule;
PYARG_PARSETUPLE PyArg_ParseTuple;
PYDICT_NEXT PyDict_Next;
PYUNICODE_ASUTF8ANDSIZE PyUnicode_AsUTF8AndSize;
PYUNICODE_ASWIDECHAR PyUnicode_AsWideChar;
PYUNICODE_FROMWIDECHAR PyUnicode_FromWideChar;
PYUNICODE_JOIN PyUnicode_Join;
PYBOOL_FROMLONG PyBool_FromLong;
PYCAPSULE_NEW PyCapsule_New;
PYCAPSULE_GETPOINTER PyCapsule_GetPointer;
PYLONG_ASUNSIGNEDLONGLONG PyLong_AsUnsignedLongLong;
PYLONG_FROMUNSIGNEDLONGLONG PyLong_FromUnsignedLongLong;
PYLIST_NEW PyList_New;
PYLIST_SETITEM PyList_SetItem;
PYLIST_GETITEM PyList_GetItem;
PYTUPLE_PACK PyTuple_Pack;
PYTUPLE_NEW PyTuple_New;
PYTUPLE_SETITEM PyTuple_SetItem;
PYERR_OCCURRED PyErr_Occurred;
PYERR_PRINT PyErr_Print;
PYERR_FETCH PyErr_Fetch;
PYERR_NORMALIZEEXCEPTION PyErr_NormalizeException;
PYOBJECT_STR PyObject_Str;
PYOBJECT_CALLOBJECT PyObject_CallObject;
PYOBJECT_GETATTRSTRING PyObject_GetAttrString;
PYOBJECT_SETATTRSTRING PyObject_SetAttrString;
PYOBJECT_CALLFUNCTIONOBJARGS PyObject_CallFunctionObjArgs;
PYOBJECT_ISTRUE PyObject_IsTrue;
PYSYS_SETOBJECT PySys_SetObject;
PYDICT_NEW PyDict_New;
PYDICT_SETITEM PyDict_SetItem;
PyObject * Py_None;
PyObject * Py_True;
PyObject * Py_False;
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
            FARPROC proc = ::GetProcAddress( hKernel32, "AttachConsole" );

            pAttachConsole = reinterpret_cast<PATTACHCONSOLE>(proc);

            if( pAttachConsole == nullptr )
            {
                ::FreeLibrary( hKernel32 );

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
            ::AllocConsole();

            // set the screen buffer to be big enough to let us scroll text
            HANDLE output_handle = ::GetStdHandle( STD_OUTPUT_HANDLE );
            ::GetConsoleScreenBufferInfo( output_handle, &coninfo );
            coninfo.dwSize.Y = 1000;
            ::SetConsoleScreenBufferSize( output_handle, coninfo.dwSize );
        }

        FILE * CONOUT = ::freopen( "CONOUT$", "w", stdout );
        MENGINE_UNUSED( CONOUT );

        //::MoveWindow( GetConsoleWindow(), 0, 650, 0, 0, TRUE );
        // make cout, wcout, cin, wcin, wcerr, cerr, wclog and clog
        // point to console as well
        std::ios::sync_with_stdio();

        if( hKernel32 != nullptr )
        {
            ::FreeLibrary( hKernel32 );
        }

        std::cout << "console ready.." << std::endl;
    }
    //////////////////////////////////////////////////////////////////////////
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
            ->setVerboseLevel( LM_MESSAGE );

        LOGGER_SERVICE()
            ->setThreadMode( LMODE_THREAD );

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
        PLUGIN_CREATE( PVRTC, MENGINE_DOCUMENT_FUNCTION );
        PLUGIN_CREATE( Theora, MENGINE_DOCUMENT_FUNCTION );
        PLUGIN_CREATE( OggVorbis, MENGINE_DOCUMENT_FUNCTION );
        PLUGIN_CREATE( ImageCodec, MENGINE_DOCUMENT_FUNCTION );
        PLUGIN_CREATE( Movie, MENGINE_DOCUMENT_FUNCTION );
        PLUGIN_CREATE( Metabuf, MENGINE_DOCUMENT_FUNCTION );
        PLUGIN_CREATE( Win32FindPython3, MENGINE_DOCUMENT_FUNCTION );
        PLUGIN_CREATE( Win32FileGroup, MENGINE_DOCUMENT_FUNCTION );

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

        FileGroupInterfacePtr globalFileGroup = nullptr;

        if( FILE_SERVICE()
            ->mountFileGroup( STRINGIZE_STRING_LOCAL( "dev" ), nullptr, nullptr, FilePath::none(), STRINGIZE_STRING_LOCAL( "global" ), &globalFileGroup, false, MENGINE_DOCUMENT_FUNCTION ) == false )
        {
            return false;
        }

        VOCABULARY_SET( FileGroupInterface, STRINGIZE_STRING_LOCAL( "FileGroup" ), STRINGIZE_STRING_LOCAL( "dev" ), globalFileGroup, MENGINE_DOCUMENT_FUNCTION );

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    static void finalize()
    {
        SERVICE_PROVIDER_STOP();

        PLUGIN_SERVICE()
            ->unloadPlugins();

        VOCABULARY_REMOVE( STRINGIZE_STRING_LOCAL( "FileGroup" ), STRINGIZE_STRING_LOCAL( "dev" ) );

        FILE_SERVICE()
            ->unmountFileGroup( STRINGIZE_STRING_LOCAL( "dev" ) );

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
}
//////////////////////////////////////////////////////////////////////////
static PyObject * py_writeBin( PyObject * _self, PyObject * _args )
{
    (void)_self;

    PyObject * py_protocolPath;
    PyObject * py_xmlPath;
    PyObject * py_binPath;

    if( !PyArg_ParseTuple( _args, "UUU", &py_protocolPath, &py_xmlPath, &py_binPath ) )
    {
        return nullptr;
    }

    wchar_t protocolPath[MENGINE_MAX_PATH + 1] = {L'\0'};
    PyUnicode_AsWideChar( py_protocolPath, protocolPath, MENGINE_MAX_PATH );

    wchar_t xmlPath[MENGINE_MAX_PATH + 1] = {L'\0'};
    PyUnicode_AsWideChar( py_xmlPath, xmlPath, MENGINE_MAX_PATH );

    wchar_t binPath[MENGINE_MAX_PATH + 1] = {L'\0'};
    PyUnicode_AsWideChar( py_binPath, binPath, MENGINE_MAX_PATH );

    if( Mengine::writeBin( protocolPath, xmlPath, binPath ) == false )
    {
        return Py_False;
    }

    return Py_True;
}
//////////////////////////////////////////////////////////////////////////
static PyObject * py_writeAek( PyObject * _self, PyObject * _args )
{
    (void)_self;

    PyObject * py_protocolPath;
    PyObject * py_xmlPath;
    PyObject * py_aekPath;

    if( !PyArg_ParseTuple( _args, "UUU", &py_protocolPath, &py_xmlPath, &py_aekPath ) )
    {
        return nullptr;
    }

    wchar_t protocolPath[MENGINE_MAX_PATH + 1] = {L'\0'};
    PyUnicode_AsWideChar( py_protocolPath, protocolPath, MENGINE_MAX_PATH );

    wchar_t xmlPath[MENGINE_MAX_PATH + 1] = {L'\0'};
    PyUnicode_AsWideChar( py_xmlPath, xmlPath, MENGINE_MAX_PATH );

    wchar_t aekPath[MENGINE_MAX_PATH + 1] = {L'\0'};
    PyUnicode_AsWideChar( py_aekPath, aekPath, MENGINE_MAX_PATH );

    if( Mengine::writeAek( protocolPath, xmlPath, aekPath ) == false )
    {
        return Py_False;
    }

    return Py_True;
}
//////////////////////////////////////////////////////////////////////////
static PyObject * py_convert( PyObject * _self, PyObject * _args )
{
    (void)_self;

    PyObject * py_fromPath;
    PyObject * py_toPath;
    PyObject * py_convertType;
    PyObject * py_params;

    if( !PyArg_ParseTuple( _args, "UUUO", &py_fromPath, &py_toPath, &py_convertType, &py_params ) )
    {
        return nullptr;
    }

    wchar_t fromPath[MENGINE_MAX_PATH];
    PyUnicode_AsWideChar( py_fromPath, fromPath, MENGINE_MAX_PATH );

    wchar_t toPath[MENGINE_MAX_PATH];
    PyUnicode_AsWideChar( py_toPath, toPath, MENGINE_MAX_PATH );

    wchar_t convertType[256];
    PyUnicode_AsWideChar( py_convertType, convertType, 256 );

    Mengine::Params params;

    if( py_params != Py_None )
    {
        PyObject * py_key;
        PyObject * py_value;
        Py_ssize_t pos = 0;

        while( PyDict_Next( py_params, &pos, &py_key, &py_value ) )
        {
            const char * key = PyUnicode_AsUTF8AndSize( py_key, nullptr );

            PyObject * py_value_str = PyObject_Str( py_value );

            const char * utf8_value_str = PyUnicode_AsUTF8AndSize( py_value_str, nullptr );

            params[Mengine::Helper::stringizeString( key )] = Mengine::ParamString( utf8_value_str );
        }
    }

    if( Mengine::convert( fromPath, toPath, convertType, params ) == false )
    {
        return nullptr;
    }

    return Py_None;
}
//////////////////////////////////////////////////////////////////////////
static PyObject * py_isAlphaInImageFile( PyObject * _self, PyObject * _args )
{
    (void)_self;

    PyObject * py_path;

    if( !PyArg_ParseTuple( _args, "U", &py_path ) )
    {
        return nullptr;
    }

    const char * utf8_path = PyUnicode_AsUTF8AndSize( py_path, nullptr );

    Mengine::FilePath c_path = Mengine::Helper::stringizeFilePath( utf8_path );

    Mengine::FileGroupInterfacePtr globalFileGroup = VOCABULARY_GET( STRINGIZE_STRING_LOCAL( "FileGroup" ), STRINGIZE_STRING_LOCAL( "dev" ) );

    Mengine::InputStreamInterfacePtr stream = Mengine::Helper::openInputStreamFile( globalFileGroup, c_path, false, false, MENGINE_DOCUMENT_FUNCTION );

    if( stream == nullptr )
    {
        return nullptr;
    }

    const Mengine::ConstString & codecType = CODEC_SERVICE()
        ->findCodecType( c_path );

    Mengine::ImageDecoderInterfacePtr imageDecoder = CODEC_SERVICE()
        ->createDecoder( codecType, MENGINE_DOCUMENT_FUNCTION );

    if( imageDecoder == nullptr )
    {
        return nullptr;
    }

    if( imageDecoder->prepareData( stream ) == false )
    {
        return nullptr;
    }

    const Mengine::ImageCodecDataInfo * dataInfo = imageDecoder->getCodecDataInfo();

    uint32_t channels = Mengine::Helper::getPixelFormatChannels( dataInfo->format );

    if( channels != 4 )
    {
        return Py_False;
    }

    return Py_True;
}
//////////////////////////////////////////////////////////////////////////
static PyObject * py_isUselessAlphaInImageFile( PyObject * _self, PyObject * _args )
{
    (void)_self;

    PyObject * py_path;

    if( !PyArg_ParseTuple( _args, "U", &py_path ) )
    {
        return nullptr;
    }

    const char * utf8_path = PyUnicode_AsUTF8AndSize( py_path, nullptr );

    Mengine::FilePath c_path = Mengine::Helper::stringizeFilePath( utf8_path );

    Mengine::Image image;

    if( image.load( c_path ) == false )
    {
        return nullptr;
    }

    if( image.getChannels() != 4 )
    {
        return Py_True;
    }

    if( image.uselessalpha() == false )
    {
        return Py_False;
    }

    return Py_True;
}
//////////////////////////////////////////////////////////////////////////
static PyObject * py_isPow2SquadImageFile( PyObject * _self, PyObject * _args )
{
    (void)_self;

    PyObject * py_path;

    if( !PyArg_ParseTuple( _args, "U", &py_path ) )
    {
        return nullptr;
    }

    const char * utf8_path = PyUnicode_AsUTF8AndSize( py_path, nullptr );

    Mengine::FilePath c_path = Mengine::Helper::stringizeFilePath( utf8_path );

    Mengine::FileGroupInterfacePtr globalFileGroup = VOCABULARY_GET( STRINGIZE_STRING_LOCAL( "FileGroup" ), STRINGIZE_STRING_LOCAL( "dev" ) );

    Mengine::InputStreamInterfacePtr stream = Mengine::Helper::openInputStreamFile( globalFileGroup, c_path, false, false, MENGINE_DOCUMENT_FUNCTION );

    if( stream == nullptr )
    {
        return nullptr;
    }

    const Mengine::ConstString & codecType = CODEC_SERVICE()
        ->findCodecType( c_path );

    Mengine::ImageDecoderInterfacePtr imageDecoder = CODEC_SERVICE()
        ->createDecoder( codecType, MENGINE_DOCUMENT_FUNCTION );

    if( imageDecoder == nullptr )
    {
        return nullptr;
    }

    if( imageDecoder->prepareData( stream ) == false )
    {
        return nullptr;
    }

    const Mengine::ImageCodecDataInfo * dataInfo = imageDecoder->getCodecDataInfo();

    uint32_t width = dataInfo->width;
    uint32_t height = dataInfo->height;

    if( Mengine::Helper::isTexturePow2( width ) == false )
    {
        return Py_False;
    }

    if( Mengine::Helper::isTexturePow2( height ) == false )
    {
        return Py_False;
    }

    if( width != height )
    {
        return Py_False;
    }

    return Py_True;
}
//////////////////////////////////////////////////////////////////////////
static void py_deleteImage( PyObject * _capsule )
{
    Mengine::Image * image = (Mengine::Image *)PyCapsule_GetPointer( _capsule, "Image" );

    delete image;
}
//////////////////////////////////////////////////////////////////////////
static PyObject * py_loadImage( PyObject * _self, PyObject * _args )
{
    (void)_self;

    PyObject * py_path;

    if( !PyArg_ParseTuple( _args, "U", &py_path ) )
    {
        return nullptr;
    }

    const char * utf8_path = PyUnicode_AsUTF8AndSize( py_path, nullptr );

    Mengine::FilePath c_path = Mengine::Helper::stringizeFilePath( utf8_path );

    Mengine::Image * image = new Mengine::Image();

    if( image->load( c_path ) == false )
    {
        return nullptr;
    }

    return PyCapsule_New( image, "Image", &py_deleteImage );
}
//////////////////////////////////////////////////////////////////////////
static PyObject * py_saveImage( PyObject * _self, PyObject * _args )
{
    (void)_self;

    PyObject * py_image;
    PyObject * py_path;

    if( !PyArg_ParseTuple( _args, "OU", &py_image, &py_path ) )
    {
        return nullptr;
    }

    Mengine::Image * image = (Mengine::Image *)PyCapsule_GetPointer( py_image, "Image" );

    const char * utf8_path = PyUnicode_AsUTF8AndSize( py_path, nullptr );

    Mengine::FilePath c_path = Mengine::Helper::stringizeFilePath( utf8_path );

    if( image->save( c_path ) == false )
    {
        return Py_False;
    }

    return Py_True;
}
//////////////////////////////////////////////////////////////////////////
static PyObject * py_createImage( PyObject * _self, PyObject * _args )
{
    (void)_self;

    uint32_t width;
    uint32_t height;
    uint32_t channel;
    PyObject * py_color;

    if( !PyArg_ParseTuple( _args, "IIIO", &width, &height, &channel, &py_color ) )
    {
        return nullptr;
    }

    float r, g, b, a;

    if( !PyArg_ParseTuple( py_color, "ffff", &r, &g, &b, &a ) )
    {
        return nullptr;
    }

    Mengine::Image * image = new Mengine::Image();

    image->create( width, height, channel );
    image->fill( {r, g, b, a} );

    return PyCapsule_New( image, "Image", &py_deleteImage );
}
//////////////////////////////////////////////////////////////////////////
static PyObject * py_getImageWidth( PyObject * _self, PyObject * _args )
{
    (void)_self;

    PyObject * py_image;

    if( !PyArg_ParseTuple( _args, "O", &py_image ) )
    {
        return nullptr;
    }

    Mengine::Image * image = (Mengine::Image *)PyCapsule_GetPointer( py_image, "Image" );

    uint32_t width = image->getWidth();

    return PyLong_FromUnsignedLongLong( width );
}
//////////////////////////////////////////////////////////////////////////
static PyObject * py_getImageHeight( PyObject * _self, PyObject * _args )
{
    (void)_self;

    PyObject * py_image;

    if( !PyArg_ParseTuple( _args, "O", &py_image ) )
    {
        return nullptr;
    }

    Mengine::Image * image = (Mengine::Image *)PyCapsule_GetPointer( py_image, "Image" );

    uint32_t height = image->getHeight();

    return PyLong_FromUnsignedLongLong( height );
}
//////////////////////////////////////////////////////////////////////////
static PyObject * py_getImageChannels( PyObject * _self, PyObject * _args )
{
    (void)_self;

    PyObject * py_image;

    if( !PyArg_ParseTuple( _args, "O", &py_image ) )
    {
        return nullptr;
    }

    Mengine::Image * image = (Mengine::Image *)PyCapsule_GetPointer( py_image, "Image" );

    uint32_t channels = image->getChannels();

    return PyLong_FromUnsignedLongLong( channels );
}
//////////////////////////////////////////////////////////////////////////
static PyObject * py_pasteImage( PyObject * _self, PyObject * _args )
{
    (void)_self;

    PyObject * py_image;
    PyObject * py_image_paste;
    uint32_t x;
    uint32_t y;

    if( !PyArg_ParseTuple( _args, "OOII", &py_image, &py_image_paste, &x, &y ) )
    {
        return nullptr;
    }

    Mengine::Image * image = (Mengine::Image *)PyCapsule_GetPointer( py_image, "Image" );

    Mengine::Image * image_paste = (Mengine::Image *)PyCapsule_GetPointer( py_image_paste, "Image" );

    bool result = image->paste( image_paste, x, y );

    return PyBool_FromLong( result );
}
//////////////////////////////////////////////////////////////////////////
static PyObject * py_putImageData( PyObject * _self, PyObject * _args )
{
    (void)_self;

    PyObject * py_image;
    PyObject * py_data;

    if( !PyArg_ParseTuple( _args, "OO", &py_image, &py_data ) )
    {
        return nullptr;
    }

    Mengine::Image * image = (Mengine::Image *)PyCapsule_GetPointer( py_image, "Image" );

    uint32_t width = image->getWidth();
    uint32_t height = image->getHeight();
    uint32_t channels = image->getChannels();
    uint8_t * memory = image->getMemory();

    for( uint32_t j = 0; j != height; ++j )
    {
        for( uint32_t i = 0; i != width; ++i )
        {
            uint32_t pixel_index = i + (j * width);
            uint32_t index = pixel_index * channels;

            PyObject * py_pixel = PyList_GetItem( py_data, pixel_index );

            for( uint32_t k = 0; k != channels; ++k )
            {
                PyObject * py_color = PyList_GetItem( py_pixel, k );

                uint8_t color = (uint8_t)PyLong_AsUnsignedLongLong( py_color );

                memory[index + k] = color;
            }
        }
    }

    return Py_True;
}
//////////////////////////////////////////////////////////////////////////
static PyObject * py_rotateImage( PyObject * _self, PyObject * _args )
{
    (void)_self;

    PyObject * py_image;
    float angle;

    if( !PyArg_ParseTuple( _args, "Of", &py_image, &angle ) )
    {
        return nullptr;
    }

    Mengine::Image * image = (Mengine::Image *)PyCapsule_GetPointer( py_image, "Image" );

    Mengine::Image * rotate_image = image->rotate( angle );

    return PyCapsule_New( rotate_image, "Image", &py_deleteImage );
}
//////////////////////////////////////////////////////////////////////////
static PyObject * py_getImageExtremColor( PyObject * _self, PyObject * _args )
{
    (void)_self;

    PyObject * py_image;

    if( !PyArg_ParseTuple( _args, "O", &py_image ) )
    {
        return nullptr;
    }

    Mengine::Image * image = (Mengine::Image *)PyCapsule_GetPointer( py_image, "Image" );

    uint8_t min_color[4];
    uint8_t max_color[4];
    image->getExtremColor( min_color, max_color );

    uint32_t channels = image->getChannels();

    PyObject * py_extrem = PyList_New( channels );

    for( uint32_t k = 0; k != channels; ++k )
    {
        PyObject * py_color = PyTuple_Pack( 2, PyLong_FromUnsignedLongLong( min_color[k] ), PyLong_FromUnsignedLongLong( max_color[k] ) );

        PyList_SetItem( py_extrem, k, py_color );
    }

    return py_extrem;
}
//////////////////////////////////////////////////////////////////////////
static PyObject * py_uselessalphaImage( PyObject * _self, PyObject * _args )
{
    (void)_self;

    PyObject * py_image;

    if( !PyArg_ParseTuple( _args, "O", &py_image ) )
    {
        return nullptr;
    }

    Mengine::Image * image = (Mengine::Image *)PyCapsule_GetPointer( py_image, "Image" );

    bool result = image->uselessalpha();

    return PyBool_FromLong( result );
}
//////////////////////////////////////////////////////////////////////////
static PyObject * py_splitImage( PyObject * _self, PyObject * _args )
{
    (void)_self;

    PyObject * py_image;

    if( !PyArg_ParseTuple( _args, "O", &py_image ) )
    {
        return nullptr;
    }

    Mengine::Image * image = (Mengine::Image *)PyCapsule_GetPointer( py_image, "Image" );

    Mengine::Image * imageRGB;
    Mengine::Image * imageAlpha;
    image->split( &imageRGB, &imageAlpha );

    PyObject * py_imageRGB = PyCapsule_New( imageRGB, "Image", &py_deleteImage );
    PyObject * py_imageAlpha = PyCapsule_New( imageAlpha, "Image", &py_deleteImage );

    return PyTuple_Pack( 2, py_imageRGB, py_imageAlpha );
}
//////////////////////////////////////////////////////////////////////////
static PyObject * py_releaseImage( PyObject * _self, PyObject * _args )
{
    (void)_self;

    PyObject * py_image;

    if( !PyArg_ParseTuple( _args, "O", &py_image ) )
    {
        return nullptr;
    }

    Mengine::Image * image = (Mengine::Image *)PyCapsule_GetPointer( py_image, "Image" );

    image->release();

    return Py_None;
}
//////////////////////////////////////////////////////////////////////////
static PyObject * py_getImageData( PyObject * _self, PyObject * _args )
{
    (void)_self;

    PyObject * py_image;
    if( !PyArg_ParseTuple( _args, "O", &py_image ) )
    {
        return nullptr;
    }

    Mengine::Image * image = (Mengine::Image *)PyCapsule_GetPointer( py_image, "Image" );

    uint32_t width = image->getWidth();
    uint32_t height = image->getHeight();
    uint32_t channels = image->getChannels();

    uint8_t * memory = image->getMemory();

    PyObject * py_pixels = PyList_New( width * height );

    for( uint32_t j = 0; j != height; ++j )
    {
        for( uint32_t i = 0; i != width; ++i )
        {
            uint32_t pixel_index = i + (j * width);
            uint32_t index = pixel_index * channels;

            PyObject * py_pixel = PyList_New( channels );

            for( uint32_t k = 0; k != channels; ++k )
            {
                uint8_t color = memory[index + k];

                PyObject * py_color = PyLong_FromUnsignedLongLong( color );

                PyList_SetItem( py_pixel, k, py_color );
            }

            PyList_SetItem( py_pixels, pixel_index, py_pixel );
        }
    }

    return py_pixels;
}
//////////////////////////////////////////////////////////////////////////
static PyObject * py_pathSHA1( PyObject * _self, PyObject * _args )
{
    (void)_self;

    PyObject * py_path;

    if( !PyArg_ParseTuple( _args, "U", &py_path ) )
    {
        return nullptr;
    }

    const char * utf8_path = PyUnicode_AsUTF8AndSize( py_path, nullptr );

    Mengine::FilePath c_path = Mengine::Helper::stringizeFilePath( utf8_path );

    Mengine::FileGroupInterfacePtr globalFileGroup = VOCABULARY_GET( STRINGIZE_STRING_LOCAL( "FileGroup" ), STRINGIZE_STRING_LOCAL( "dev" ) );

    Mengine::InputStreamInterfacePtr stream = Mengine::Helper::openInputStreamFile( globalFileGroup, c_path, false, false, MENGINE_DOCUMENT_FUNCTION );

    if( stream == nullptr )
    {
        return nullptr;
    }

    size_t size = stream->size();

    uint8_t * buf = new uint8_t[size];

    stream->read( buf, size );

    unsigned char hash[20];
    stdex::sha1_calc( buf, (int)size, hash );

    delete[] buf;

    wchar_t hex[41];
    stdex::sha1_hexw( hash, hex );

    return PyUnicode_FromWideChar( hex, 40 );
}
//////////////////////////////////////////////////////////////////////////
static Mengine::Char g_message[MENGINE_LOGGER_MAX_MESSAGE + 1] = {'\0'};
//////////////////////////////////////////////////////////////////////////
static PyObject * py_log( PyObject * _self, PyObject * _args )
{
    (void)_self;

    PyObject * py_message;

    if( !PyArg_ParseTuple( _args, "U", &py_message ) )
    {
        return nullptr;
    }

    const char * message = PyUnicode_AsUTF8AndSize( py_message, nullptr );

    if( Mengine::StdString::strcmp( message, "\n" ) != 0 )
    {
        Mengine::StdString::strcpy( g_message, message );

        return Py_None;
    }

    LOGGER_MESSAGE( "%s"
        , g_message
    );

    Mengine::StdString::strcpy( g_message, "" );

    return Py_None;
}
//////////////////////////////////////////////////////////////////////////
static bool CheckPythonException()
{
    if( PyErr_Occurred() )
    {
        PyObject * ptype;
        PyObject * pvalue;
        PyObject * ptraceback;
        PyErr_Fetch( &ptype, &pvalue, &ptraceback );
        PyErr_NormalizeException( &ptype, &pvalue, &ptraceback );

        PyObject * str_value = PyObject_Str( pvalue );
        const char * error_message = PyUnicode_AsUTF8AndSize( str_value, nullptr );

        LOGGER_ERROR( "%s"
            , error_message
        );

        if( ptraceback != nullptr )
        {
            PyObject * pmodule_traceback = PyImport_ImportModule( "traceback" );
            PyObject * pfunc_format_exception = PyObject_GetAttrString( pmodule_traceback, "format_exception" );

            PyObject * pformatted_list = PyObject_CallFunctionObjArgs( pfunc_format_exception, ptype, pvalue, ptraceback, nullptr );

            PyObject * pformatted_string = PyUnicode_Join( PyUnicode_FromWideChar( L"", -1 ), pformatted_list );

            const char * formatted_string = PyUnicode_AsUTF8AndSize( pformatted_string, nullptr );

            LOGGER_ERROR( "%s"
                , formatted_string
            );
        }

        return false;
    }

    return true;
}
//////////////////////////////////////////////////////////////////////////
static PyObject * CreateCFunction( const char * _name, PyCFunction _func )
{
    PyMethodDef * method = new PyMethodDef();
    method->ml_name = _name;
    method->ml_meth = _func;
    method->ml_flags = METH_VARARGS;
    method->ml_doc = nullptr;

    PyObject * py_method = PyCFunction_New( method, nullptr );

    return py_method;
}
//////////////////////////////////////////////////////////////////////////
static void AddCFunctionToModule( PyObject * _module, const char * _name, PyCFunction _func )
{
    PyObject * py_method = CreateCFunction( _name, _func );

    PyModule_AddObject( _module, _name, py_method );
}
//////////////////////////////////////////////////////////////////////////
static bool run()
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
        Mengine::Char MSG[MENGINE_LOGGER_MAX_MESSAGE + 1] = {'\0'};

        MENGINE_SNPRINTF( MSG, MENGINE_LOGGER_MAX_MESSAGE, "PyInit_ToolsBuilderPlugin exception: %s"
            , se.what()
        );

        MessageBoxA( NULL, MSG, "Error", MB_OK );

        return false;
    }

    LOGGER_MESSAGE( "initialize complete" );

    WCHAR currentPath[MENGINE_MAX_PATH + 1] = {L'\0'};
    DWORD len = ::GetCurrentDirectory( MENGINE_MAX_PATH, currentPath );

    if( len == 0 )
    {
        LOGGER_MESSAGE( "invalid get current directory" );

        return false;
    }

    LOGGER_MESSAGE( "current directory: %ls"
        , currentPath
    );

    currentPath[len + 0] = MENGINE_PATH_WDELIM;
    currentPath[len + 1] = L'\0';

    WCHAR python3LibPath[MENGINE_MAX_PATH + 1] = {L'\0'};
    WIN32_FINDPYTHON3_SERVICE()
        ->getPython3LibraryPathW( python3LibPath );

    LOGGER_MESSAGE( "python lib path: %ls"
        , python3LibPath
    );

    WCHAR python3DllPath[MENGINE_MAX_PATH + 1] = {L'\0'};
    WIN32_FINDPYTHON3_SERVICE()
        ->getPython3DllPathW( python3DllPath );

    LOGGER_MESSAGE( "python dll path: %ls"
        , python3DllPath
    );

    HMODULE hPythonDll = ::LoadLibrary( python3DllPath );

    if( hPythonDll == NULL )
    {
        LOGGER_ERROR( "invalid load python dll '%ls' error: %u"
            , python3DllPath
            , ::GetLastError()
        );

        return false;
    }

    LOGGER_MESSAGE( "load python dll '%ls'"
        , python3DllPath
    );

    Py_InitializeEx = (PY_INITIALIZEEX)::GetProcAddress( hPythonDll, "Py_InitializeEx" );
    Py_GetPath = (PY_GETPATH)::GetProcAddress( hPythonDll, "Py_GetPath" );
    Py_SetPath = (PY_SETPATH)::GetProcAddress( hPythonDll, "Py_SetPath" );
    PyImport_AddModule = (PYIMPORT_ADDMODULE)::GetProcAddress( hPythonDll, "PyImport_AddModule" );
    PyCFunction_New = (PYCFUNCTION_NEW)::GetProcAddress( hPythonDll, "PyCFunction_New" );
    PyModule_AddObject = (PYMODULE_ADDOBJECT)::GetProcAddress( hPythonDll, "PyModule_AddObject" );
    PyImport_ImportModule = (PYIMPROT_IMPORTMODULE)::GetProcAddress( hPythonDll, "PyImport_ImportModule" );
    PyArg_ParseTuple = (PYARG_PARSETUPLE)::GetProcAddress( hPythonDll, "PyArg_ParseTuple" );
    PyDict_Next = (PYDICT_NEXT)::GetProcAddress( hPythonDll, "PyDict_Next" );
    Py_None = (PyObject *)::GetProcAddress( hPythonDll, "_Py_NoneStruct" );
    PyUnicode_AsUTF8AndSize = (PYUNICODE_ASUTF8ANDSIZE)::GetProcAddress( hPythonDll, "PyUnicode_AsUTF8AndSize" );
    PyUnicode_AsWideChar = (PYUNICODE_ASWIDECHAR)::GetProcAddress( hPythonDll, "PyUnicode_AsWideChar" );
    PyUnicode_FromWideChar = (PYUNICODE_FROMWIDECHAR)::GetProcAddress( hPythonDll, "PyUnicode_FromWideChar" );
    PyUnicode_Join = (PYUNICODE_JOIN)::GetProcAddress( hPythonDll, "PyUnicode_Join" );
    PyBool_FromLong = (PYBOOL_FROMLONG)::GetProcAddress( hPythonDll, "PyBool_FromLong" );
    PyCapsule_New = (PYCAPSULE_NEW)::GetProcAddress( hPythonDll, "PyCapsule_New" );
    PyCapsule_GetPointer = (PYCAPSULE_GETPOINTER)::GetProcAddress( hPythonDll, "PyCapsule_GetPointer" );
    PyLong_AsUnsignedLongLong = (PYLONG_ASUNSIGNEDLONGLONG)::GetProcAddress( hPythonDll, "PyLong_AsUnsignedLongLong" );
    PyLong_FromUnsignedLongLong = (PYLONG_FROMUNSIGNEDLONGLONG)::GetProcAddress( hPythonDll, "PyLong_FromUnsignedLongLong" );
    PyList_New = (PYLIST_NEW)::GetProcAddress( hPythonDll, "PyList_New" );
    PyList_SetItem = (PYLIST_SETITEM)::GetProcAddress( hPythonDll, "PyList_SetItem" );
    PyList_GetItem = (PYLIST_GETITEM)::GetProcAddress( hPythonDll, "PyList_GetItem" );
    PyTuple_Pack = (PYTUPLE_PACK)::GetProcAddress( hPythonDll, "PyTuple_Pack" );
    PyTuple_New = (PYTUPLE_NEW)::GetProcAddress( hPythonDll, "PyTuple_New" );
    PyTuple_SetItem = (PYTUPLE_SETITEM)::GetProcAddress( hPythonDll, "PyTuple_SetItem" );
    PyErr_Occurred = (PYERR_OCCURRED)::GetProcAddress( hPythonDll, "PyErr_Occurred" );
    PyErr_Print = (PYERR_PRINT)::GetProcAddress( hPythonDll, "PyErr_Print" );
    PyErr_Fetch = (PYERR_FETCH)::GetProcAddress( hPythonDll, "PyErr_Fetch" );
    PyErr_NormalizeException = (PYERR_NORMALIZEEXCEPTION)::GetProcAddress( hPythonDll, "PyErr_NormalizeException" );
    PyObject_Str = (PYOBJECT_STR)::GetProcAddress( hPythonDll, "PyObject_Str" );
    PyObject_GetAttrString = (PYOBJECT_GETATTRSTRING)::GetProcAddress( hPythonDll, "PyObject_GetAttrString" );
    PyObject_SetAttrString = (PYOBJECT_SETATTRSTRING)::GetProcAddress( hPythonDll, "PyObject_SetAttrString" );
    PyObject_CallFunctionObjArgs = (PYOBJECT_CALLFUNCTIONOBJARGS)::GetProcAddress( hPythonDll, "PyObject_CallFunctionObjArgs" );
    PyObject_CallObject = (PYOBJECT_CALLOBJECT)::GetProcAddress( hPythonDll, "PyObject_CallObject" );
    PyObject_IsTrue = (PYOBJECT_ISTRUE)::GetProcAddress( hPythonDll, "PyObject_IsTrue" );
    PySys_SetObject = (PYSYS_SETOBJECT)::GetProcAddress( hPythonDll, "PySys_SetObject" );
    PyDict_New = (PYDICT_NEW)::GetProcAddress( hPythonDll, "PyDict_New" );
    PyDict_SetItem = (PYDICT_SETITEM)::GetProcAddress( hPythonDll, "PyDict_SetItem" );
    Py_False = PyBool_FromLong( 0 );
    Py_True = PyBool_FromLong( 1 );

    Py_SetPath( python3LibPath );

    LOGGER_MESSAGE( "Python initialize" );

    Py_InitializeEx( 0 );

    if( CheckPythonException() == false )
    {
        LOGGER_ERROR( "Invalid python initialize" );

        return false;
    }

    LOGGER_MESSAGE( "Python successful" );

    PyObject * py_tools_module = PyImport_AddModule( "ToolsBuilderPlugin" );

    AddCFunctionToModule( py_tools_module, "writeBin", &py_writeBin );
    AddCFunctionToModule( py_tools_module, "writeAek", &py_writeAek );
    AddCFunctionToModule( py_tools_module, "convert", &py_convert );

    AddCFunctionToModule( py_tools_module, "isAlphaInImageFile", &py_isAlphaInImageFile );
    AddCFunctionToModule( py_tools_module, "isUselessAlphaInImageFile", &py_isUselessAlphaInImageFile );
    AddCFunctionToModule( py_tools_module, "isPow2SquadImageFile", &py_isPow2SquadImageFile );

    AddCFunctionToModule( py_tools_module, "loadImage", &py_loadImage );
    AddCFunctionToModule( py_tools_module, "saveImage", &py_saveImage );
    AddCFunctionToModule( py_tools_module, "createImage", &py_createImage );
    AddCFunctionToModule( py_tools_module, "getImageWidth", &py_getImageWidth );
    AddCFunctionToModule( py_tools_module, "getImageHeight", &py_getImageHeight );
    AddCFunctionToModule( py_tools_module, "getImageChannels", &py_getImageChannels );
    AddCFunctionToModule( py_tools_module, "pasteImage", &py_pasteImage );
    AddCFunctionToModule( py_tools_module, "putImageData", &py_putImageData );
    AddCFunctionToModule( py_tools_module, "rotateImage", &py_rotateImage );
    AddCFunctionToModule( py_tools_module, "getImageExtremColor", &py_getImageExtremColor );
    AddCFunctionToModule( py_tools_module, "uselessalphaImage", &py_uselessalphaImage );
    AddCFunctionToModule( py_tools_module, "splitImage", &py_splitImage );
    AddCFunctionToModule( py_tools_module, "releaseImage", &py_releaseImage );
    AddCFunctionToModule( py_tools_module, "getImageData", &py_getImageData );

    AddCFunctionToModule( py_tools_module, "pathSHA1", &py_pathSHA1 );

    AddCFunctionToModule( py_tools_module, "log", &py_log );

    LOGGER_MESSAGE( "ToolsBuilderPlugin python module successful" );

    WCHAR python3LibPathCount[MENGINE_MAX_PATH + 1] = {L'\0'};
    Mengine::StdString::wcscpy( python3LibPathCount, python3LibPath );

    WCHAR python3LibPathItem[MENGINE_MAX_PATH + 1] = {L'\0'};
    Mengine::StdString::wcscpy( python3LibPathItem, python3LibPath );

    wchar_t * token;
    wchar_t * context;

    token = ::wcstok( python3LibPathCount, L";", &context );

    Py_ssize_t pathCount = 0;
    while( token != NULL )
    {
        ++pathCount;

        token = ::wcstok( nullptr, L";", &context );
    }

    PyObject * py_syspath = PyList_New( pathCount * 2 + 1 );

    Py_ssize_t pathIndex = 0;

    PyList_SetItem( py_syspath, pathIndex, PyUnicode_FromWideChar( currentPath, -1 ) );

    ++pathIndex;

    token = ::wcstok( python3LibPathItem, L";", &context );
    
    while( token != nullptr )
    {
        WCHAR token_site[MENGINE_MAX_PATH + 1] = {L'\0'};
        ::PathCombineW( token_site, token, L"site-packages\\" );

        LOGGER_MESSAGE( "ToolsBuilderPlugin python add sys path: %ls"
            , token
        );

        LOGGER_MESSAGE( "ToolsBuilderPlugin python add sys path: %ls"
            , token_site
        );

        PyList_SetItem( py_syspath, pathIndex + 0, PyUnicode_FromWideChar( token, -1 ) );
        PyList_SetItem( py_syspath, pathIndex + 1, PyUnicode_FromWideChar( token_site, -1 ) );

        ++pathIndex;
        ++pathIndex;

        token = ::wcstok( NULL, L";", &context );
    }

    PySys_SetObject( "path", py_syspath );

    LOGGER_MESSAGE( "ToolsBuilderPlugin python setup sys path successful" );

    if( CheckPythonException() == false )
    {
        LOGGER_ERROR( "invalid set python path" );

        return false;
    }

    LPWSTR lpwCmdLine = ::GetCommandLineW();

    LOGGER_MESSAGE( "ToolsBuilderPlugin command line: %ls"
        , lpwCmdLine
    );

    int nArgs = 0;
    LPWSTR * szArglist = ::CommandLineToArgvW( lpwCmdLine, &nArgs );

    if( nArgs < 3 )
    {
        LOGGER_ERROR( "invalid command line" );

        return false;
    }

    PWSTR szModuleName = szArglist[1];

    Mengine::Char utf8_ModuleName[256 + 1] = {'\0'};
    Mengine::Helper::unicodeToUtf8( szModuleName, utf8_ModuleName, 256 );

    PWSTR szFunctionName = szArglist[2];

    Mengine::Char utf8_FunctionName[256 + 1] = {'\0'};
    Mengine::Helper::unicodeToUtf8( szFunctionName, utf8_FunctionName, 256 );

    LOGGER_MESSAGE( "ToolsBuilderPlugin run module '%s' function '%s'"
        , utf8_ModuleName
        , utf8_FunctionName
    );

    PyObject * py_run_module = PyImport_ImportModule( utf8_ModuleName );

    if( CheckPythonException() == false )
    {
        LOGGER_ERROR( "invalid import module '%s'"
            , utf8_ModuleName
        );

        return false;
    }

    PyObject * py_args = PyTuple_New( nArgs - 3 );

    for( int i = 3; i != nArgs; ++i )
    {
        LPWSTR arg = szArglist[i];

        PyTuple_SetItem( py_args, i - 3, PyUnicode_FromWideChar( arg, -1 ) );
    }

    PyObject * pfunc_name = PyObject_GetAttrString( py_run_module, utf8_FunctionName );

    PyObject * presult = PyObject_CallObject( pfunc_name, py_args );

    if( CheckPythonException() == false )
    {
        LOGGER_ERROR( "invalid call function '%s'"
            , utf8_FunctionName
        );

        return false;
    }

    if( PyObject_IsTrue( presult ) == 0 )
    {
        LOGGER_ERROR( "invalid call function '%s' result"
            , utf8_FunctionName
        );

        return false;
    }

    LOGGER_MESSAGE( "build successful" );

    Mengine::finalize();

    return true;
}
///////////////////////////////////////////////////////////////////////////////////
int main( int argc, char * argv[] )
{
    MENGINE_UNUSED( argc );
    MENGINE_UNUSED( argv );

    if( ::IsDebuggerPresent() == TRUE )
    {
        Mengine::createConsole();
    }

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