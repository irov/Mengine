#	ifdef _WIN32_WINNT	
#       undef _WIN32_WINNT
#       define _WIN32_WINNT 0x0500
#   endif

#   ifdef _WIN32_WINDOWS
#       undef _WIN32_WINDOWS
#       define _WIN32_WINDOWS 0x0500
#   endif

#	define WIN32_LEAN_AND_MEAN

#	ifndef NOMINMAX
#	define NOMINMAX
#	endif

#pragma warning(push, 0) 
#	include <Windows.h>
#   include <WinUser.h>

#   include <shellapi.h>
#   include <shlwapi.h>
#   include <shlobj.h>
#	include <fcntl.h>
#pragma warning(pop) 

#pragma comment ( lib, "Ws2_32.lib" )
#pragma comment ( lib, "Crypt32.lib" )

#   include <iostream>

#	include "Interface/ServiceInterface.h"

#	include "XmlToBinDecoder.h"
#	include "AlphaSpreading.h"
#	include "XmlToAekConverter.h"
#   include "ParseAem.h"

#	include "Interface/StringizeInterface.h"
#	include "Interface/UnicodeInterface.h"
#	include "Interface/ArchiveInterface.h"
#	include "Interface/LoggerInterface.h"
#	include "Interface/CodecInterface.h"
#	include "Interface/DataInterface.h"
#	include "Interface/MemoryInterface.h"
#   include "Interface/ConverterInterface.h"
#   include "Interface/FileSystemInterface.h"
#   include "Interface/PluginInterface.h"
#   include "Interface/WindowsLayerInterface.h"
#   include "Interface/UnicodeInterface.h"
#   include "Interface/ImageCodecInterface.h"
#	include "Interface/LoaderInterface.h"
#	include "Interface/ThreadSystemInterface.h"
#	include "Interface/ParticleSystemInterface.h"
#	include "Interface/ConfigInterface.h"
#	include "Interface/OptionsInterface.h"

#   include "WindowsLayer/VistaWindowsLayer.h"

#	include "Config/String.h"

#	include "Kernel/ConverterFactory.h"

#	include "Image.h"

#   include "Kernel/Logger.h"

#	include "pybind/pybind.hpp"
#	include "pybind/stl_type_cast.hpp"

#	include "stdex/sha1.h"

#	include <io.h>

//////////////////////////////////////////////////////////////////////////
PLUGIN_EXPORT( Win32FileGroup );
PLUGIN_EXPORT( ImageCodec );
PLUGIN_EXPORT( Zip );
PLUGIN_EXPORT( LZ4 );
//////////////////////////////////////////////////////////////////////////
SERVICE_PROVIDER_EXTERN( ServiceProvider );

SERVICE_EXTERN( Platform );
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
SERVICE_EXTERN( PluginSystem );
SERVICE_EXTERN( PluginService );

SERVICE_EXTERN( WindowsLayer );
SERVICE_EXTERN( FileService );
SERVICE_EXTERN( LoaderService );
//////////////////////////////////////////////////////////////////////////
namespace Mengine
{		
	//////////////////////////////////////////////////////////////////////////
	static void createConsole()
	{
		typedef BOOL (WINAPI *PATTACHCONSOLE)(DWORD);

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

		freopen( "CONOUT$", "w", stdout );

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

		SERVICE_CREATE( FactoryService );
        SERVICE_CREATE( OptionsService );
		SERVICE_CREATE( UnicodeSystem );		
		SERVICE_CREATE( StringizeService );
		SERVICE_CREATE( ArchiveService );
		SERVICE_CREATE( ArchiveService );
		SERVICE_CREATE( LoggerService );
		
		class MyLoggerInterface
			: public LoggerInterface
		{
		public:
			bool initialize() override 
			{ 
				return true; 
			}
			
			void finalize() override 
			{
			};

			void setVerboseLevel( EMessageLevel _level ) override 
			{
				(void)_level;
			};

			void setVerboseFlag( size_t _flag ) override 
			{
				(void)_flag;
			};

		public:
			bool validMessage( EMessageLevel _level, size_t _flag ) const override 
			{ 
				(void)_level;
				(void)_flag;

				return true; 
			};

		public:
			void log( EMessageLevel _level, size_t _flag, const char * _data, size_t _count ) override
			{
				(void)_level;
				(void)_flag;
				(void)_count;

                printf( "%s"
                    , _data
                );
			}

			void flush() override 
			{
			}
		};

		LOGGER_SERVICE()
			->setVerboseLevel( LM_WARNING );

		LOGGER_SERVICE()
			->registerLogger( new MyLoggerInterface );

        LOGGER_WARNING( "Inititalizing Config Manager..." );
		
		SERVICE_CREATE( ConfigService );
		SERVICE_CREATE( ConverterService );
		SERVICE_CREATE( CodecService );
		SERVICE_CREATE( DataService );
		SERVICE_CREATE( ThreadSystem );
		SERVICE_CREATE( ThreadService );
		SERVICE_CREATE( MemoryService );
		SERVICE_CREATE( PluginSystem );
		SERVICE_CREATE( PluginService );

		SERVICE_CREATE( WindowsLayer );
        SERVICE_CREATE( Platform );
		SERVICE_CREATE( FileService );
				
		PLUGIN_CREATE( Win32FileGroup );
		PLUGIN_CREATE( Zip );
		PLUGIN_CREATE( LZ4 );
		PLUGIN_CREATE( ImageCodec );

		SERVICE_CREATE( LoaderService );
		
		PLUGIN_SERVICE()
			->loadPlugin( L"DevelopmentConverterPlugin.dll" );

		PLUGIN_SERVICE()
			->loadPlugin( L"XmlCodecPlugin.dll" );

		if( FILE_SERVICE()
			->mountFileGroup( ConstString::none(), nullptr, Helper::emptyPath(), STRINGIZE_STRING_LOCAL( "global" ), nullptr ) == false )
		{
			return false;
		}

		ConstString dev = Helper::stringizeString("dev");

		if( FILE_SERVICE()
			->mountFileGroup( dev, nullptr, Helper::emptyPath(), STRINGIZE_STRING_LOCAL("global"), nullptr ) == false )
		{
			return false;
		}

		return true;
	}
	//////////////////////////////////////////////////////////////////////////
	static bool s_convert( const WString & _fromPath, const WString & _toPath, const WString & _convertType, const WString & _params )
	{		
		String utf8_fromPath;
		Helper::unicodeToUtf8( _fromPath, utf8_fromPath );

		String utf8_toPath;
		Helper::unicodeToUtf8( _toPath, utf8_toPath );

		String utf8_convertType;
		Helper::unicodeToUtf8( _convertType, utf8_convertType );

		String utf8_params;
		Helper::unicodeToUtf8( _params, utf8_params );		

		ConverterOptions options;

        const FileGroupInterfacePtr & fileGroup = FILE_SERVICE()
            ->getDefaultFileGroup();

		options.fileGroup = fileGroup;
		options.inputFileName = Helper::stringizeFilePath(utf8_fromPath);
		options.outputFileName = Helper::stringizeFilePath(utf8_toPath);
		options.params = utf8_params;

		ConverterInterfacePtr converter = CONVERTER_SERVICE()
			->createConverter( Helper::stringizeString(utf8_convertType) );

		if( converter == nullptr )
		{
            LOGGER_ERROR( "convertPVRToHTF can't create convert '%s'\nfrom: %s\nto: %s\n"
				, utf8_convertType.c_str()
				, options.inputFileName.c_str()
				, options.outputFileName.c_str()
				);

			return false;
		}

		converter->setOptions( &options );

		if( converter->convert() == false )
		{
			LOGGER_ERROR( "convertPVRToHTF can't convert '%s'\nfrom: %s\nto: %s\n"
				, utf8_convertType.c_str()
				, options.inputFileName.c_str()
				, options.outputFileName.c_str()
				);

			return false;
		}

		return true;
	}
	//////////////////////////////////////////////////////////////////////////
	bool convert( const wchar_t * fromPath, const wchar_t * toPath, const wchar_t * convertType, const wchar_t * params )
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
	PyObject * isAlphaInImageFile( const wchar_t * _path )
	{
		String utf8_path;
		if( Helper::unicodeToUtf8( _path, utf8_path ) == false )
		{
			return NULL;
		}

		FilePath c_path = Helper::stringizeFilePath(utf8_path);

        const FileGroupInterfacePtr & fileGroup = FILE_SERVICE()
            ->getDefaultFileGroup();

		InputStreamInterfacePtr stream = FILE_SERVICE()
			->openInputFile( fileGroup, c_path, false );
		
		if( stream == nullptr )
		{
			return NULL;
		}

		const ConstString & codecType = CODEC_SERVICE()
				->findCodecType( c_path );

		ImageDecoderInterfacePtr imageDecoder = CODEC_SERVICE()
			->createDecoderT<ImageDecoderInterfacePtr>( codecType );

		if( imageDecoder == nullptr )
		{
			return NULL;
		}

		if( imageDecoder->prepareData( stream ) == false )
		{
			return NULL;
		}

		const ImageCodecDataInfo * dataInfo = imageDecoder->getCodecDataInfo();

		bool isAlpha = (dataInfo->channels == 4);

		return pybind::ret_bool( isAlpha );
	}
	//////////////////////////////////////////////////////////////////////////
	PyObject * isUselessAlphaInImageFile( const wchar_t * _path )
	{
		String utf8_path;
		if( Helper::unicodeToUtf8( _path, utf8_path ) == false )
		{
			LOGGER_ERROR("isUselessAlphaInImageFile %ls invalid unicodeToUtf8"
				, _path
				);

			return nullptr;
		}

		FilePath c_path = Helper::stringizeFilePath(utf8_path);

		Image * image = new Image();

		if( image->load( c_path ) == false )
		{
			LOGGER_ERROR("isUselessAlphaInImageFile %ls invalid load"
				, _path
				);
			
			return nullptr;
		}

		if( image->getChannels() != 4 )
		{
			return pybind::ret_true();
		}

		bool useless = image->uselessalpha();

		delete image;

		return pybind::ret_bool( useless );
	}
	//////////////////////////////////////////////////////////////////////////
	Image * loadImage( const wchar_t * _path )
	{
		String utf8_path;
		if( Helper::unicodeToUtf8( _path, utf8_path ) == false )
		{
			return nullptr;
		}

		FilePath c_path = Helper::stringizeFilePath(utf8_path);

		Image * image = new Image();

		if( image->load( c_path ) == false )
		{
			return nullptr;
		}
				
		return image;
	}
	//////////////////////////////////////////////////////////////////////////
	bool saveImage( Image * _image, const wchar_t * _path )
	{
		String utf8_path;
		if( Helper::unicodeToUtf8( _path, utf8_path ) == false )
		{
			return false;
		}

		FilePath c_path = Helper::stringizeFilePath(utf8_path);

		if( _image->save( c_path ) == false )
		{
			return false;
		}

		return true;
	}
	//////////////////////////////////////////////////////////////////////////
	static ColourValue color_convert( pybind::kernel_interface * _kernel, PyObject * _obj )
	{
		if( pybind::tuple_check( _obj ) == true )
		{
			if( pybind::tuple_size( _obj ) == 4 )
			{
				PyObject * i0 = pybind::tuple_getitem( _obj, 0 );
				PyObject * i1 = pybind::tuple_getitem( _obj, 1 );
				PyObject * i2 = pybind::tuple_getitem( _obj, 2 );
				PyObject * i3 = pybind::tuple_getitem( _obj, 3 );

				float r = pybind::extract<float>( _kernel, i0);
				float g = pybind::extract<float>( _kernel, i1);
				float b = pybind::extract<float>( _kernel, i2);
				float a = pybind::extract<float>( _kernel, i3);

				return ColourValue( r, g, b, a );
			}
			else if( pybind::tuple_size( _obj ) == 3 )
			{
				PyObject * i0 = pybind::tuple_getitem( _obj, 0 );
				PyObject * i1 = pybind::tuple_getitem( _obj, 1 );
				PyObject * i2 = pybind::tuple_getitem( _obj, 2 );

				float r = pybind::extract<float>( _kernel, i0);
				float g = pybind::extract<float>( _kernel, i1);
				float b = pybind::extract<float>( _kernel, i2);
				float a = 1.f;

				return ColourValue( r, g, b, a );
			}
		}
		else if( pybind::list_check( _obj ) == true )
		{
			if( pybind::list_size( _obj ) == 4 )
			{
				PyObject * i0 = pybind::list_getitem( _obj, 0 );
				PyObject * i1 = pybind::list_getitem( _obj, 1 );
				PyObject * i2 = pybind::list_getitem( _obj, 2 );
				PyObject * i3 = pybind::list_getitem( _obj, 3 );

				float r = pybind::extract<float>( _kernel, i0);
				float g = pybind::extract<float>( _kernel, i1);
				float b = pybind::extract<float>( _kernel, i2);
				float a = pybind::extract<float>( _kernel, i3);

				return ColourValue( r, g, b, a );
			}				
			else if( pybind::list_size( _obj ) == 3 )
			{
				PyObject * i0 = pybind::list_getitem( _obj, 0 );
				PyObject * i1 = pybind::list_getitem( _obj, 1 );
				PyObject * i2 = pybind::list_getitem( _obj, 2 );


				float r = pybind::extract<float>( _kernel, i0);
				float g = pybind::extract<float>( _kernel, i1);
				float b = pybind::extract<float>( _kernel, i2);
				float a = 1.f;

				return ColourValue( r, g, b, a );
			}
		}

		return ColourValue();
	}
	//////////////////////////////////////////////////////////////////////////
	Image * createImage( pybind::kernel_interface * _kernel, uint32_t _width, uint32_t _height, uint32_t _channel, PyObject * _colour )
	{
		Image * image = new Image();

		image->create( _width, _height, _channel );
		image->fill( color_convert( _kernel, _colour) );

		return image;
	}
	//////////////////////////////////////////////////////////////////////////
    WString pathSHA1( const wchar_t * _path )
	{
		String utf8_path;
		if( Helper::unicodeToUtf8( _path, utf8_path ) == false )
		{
			return NULL;
		}

		FilePath c_path = Helper::stringizeFilePath( utf8_path );

        const FileGroupInterfacePtr & fileGroup = FILE_SERVICE()
            ->getDefaultFileGroup();

		InputStreamInterfacePtr stream = FILE_SERVICE()
			->openInputFile( fileGroup, c_path, false );

		if( stream == nullptr )
		{
			LOGGER_ERROR( "pathSHA1 %ls invalid open '%s'"
				, _path
				, c_path.c_str()
				);

			return L"";
		}

		size_t size = stream->size();

		uint8_t * buf = new uint8_t[size];

		stream->read( buf, size );

		unsigned char hash[20];
		stdex::sha1_calc( buf, size, hash );

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
			: m_softspace(0)
		{
		}

		~PythonLogger()
		{
		}

	public:
		void write( const char * _msg, size_t _size )
		{
            LOGGER_SERVICE()
                ->logMessage( LM_ERROR, 0, _msg, _size );
		}

	public:
		PyObject * py_write( PyObject * _args, PyObject * _kwds )
		{
			(void)_kwds;

			if( pybind::tuple_check( _args ) == false )
			{
				return pybind::ret_none();
			}

			size_t tuple_size = pybind::tuple_size(_args);

			if( tuple_size == 0 )
			{
				return pybind::ret_none();
			}

			PyObject * arg = pybind::tuple_getitem(_args, 0);

			if( pybind::string_check( arg ) == true )
			{
				size_t size;
				const char * str = pybind::string_to_char_and_size( arg, size );

				this->write( str, size );
			}
			else if( pybind::unicode_check( arg ) == true )
			{
				size_t size;
				const char * utf8 = pybind::unicode_to_utf8_and_size( arg, size );

				this->write( utf8, size );
			}

			return pybind::ret_none();
		}

		PyObject * py_flush( PyObject * _args, PyObject * _kwds )
		{
			(void)_args;
			(void)_kwds;

			return pybind::ret_none();
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
			printf("setErrors %ls\n"
				, _errors
				);
		}

		const wchar_t * getEncoding() const
		{
			return L"ascii";
		}

		void setEncoding( const wchar_t * _encoding ) const
		{
			printf("setEncoding %ls\n"
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
static bool getCurrentUserRegValue( const WCHAR * _path, WCHAR * _value, DWORD _size )
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
	LONG nError = ::RegQueryValueEx( hKey, L"", 0, NULL, (LPBYTE)_value, &dwBufferSize );

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
struct extract_String_type
    : public pybind::type_cast_result<Mengine::String>
{
public:
    bool apply( pybind::kernel_interface * _kernel, PyObject * _obj, Mengine::String & _value, bool _nothrow ) override
    {
        (void)_kernel;
        (void)_nothrow;

        if( pybind::string_check( _obj ) == true )
        {
            uint32_t size;
            const Mengine::String::value_type * string_char = pybind::string_to_char_and_size( _obj, size );

            if( string_char == 0 )
            {
                return false;
            }

            _value.assign( string_char, size );
        }
        else
        {
            return false;
        }

        return true;
    }

public:
    PyObject * wrap( pybind::kernel_interface * _kernel, pybind::type_cast_result<Mengine::String>::TCastRef _value ) override
    {
        (void)_kernel;

        const Mengine::String::value_type * value_str = _value.c_str();
        Mengine::String::size_type value_size = _value.size();

        PyObject * py_value = pybind::string_from_char_size( value_str, (uint32_t)value_size );

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

        if( pybind::unicode_check( _obj ) == true )
        {
            uint32_t size = 0;
            const Mengine::WString::value_type * value_char = pybind::unicode_to_wchar_and_size( _obj, size );

            if( value_char == nullptr )
            {
                return false;
            }

            _value.assign( value_char, size );
        }
        else
        {
            return false;
        }

        return true;
    }

public:
    PyObject * wrap( pybind::kernel_interface * _kernel, pybind::type_cast_result<Mengine::WString>::TCastRef _value ) override
    {
        (void)_kernel;

        const Mengine::WString::value_type * value_str = _value.c_str();
        Mengine::WString::size_type value_size = _value.size();

        PyObject * py_value = pybind::unicode_from_wchar_size( value_str, (uint32_t)value_size );

        return py_value;
    }
};
//////////////////////////////////////////////////////////////////////////
bool run()
{
	try
	{
		if( Mengine::initialize() == false )
		{
			printf("PyInit_ToolsBuilderPlugin initialize failed\n"
				);

			return false;
		}
	}
	catch( const std::exception & se )
	{
		char MSG[2048];
		sprintf(MSG, "PyInit_ToolsBuilderPlugin exception %s"			
			, se.what()			
			);

		MessageBoxA(NULL, MSG, "Error", MB_OK);

		return false;
	}

    LOGGER_ERROR( "initialize complete" );

	const WCHAR * szRegPath = L"SOFTWARE\\Python\\PythonCore\\3.6-32\\PythonPath";

	{
		WCHAR szPythonPath[512];
		if( getCurrentUserRegValue( szRegPath, szPythonPath, 512 ) == false )
		{
			LOGGER_ERROR( "invalid get reg value '%ls'"
				, szRegPath
				);
		}

		pybind::set_path( szPythonPath );

        LOGGER_ERROR( "set python path '%ls'"
			, szPythonPath
			);
	}	

	pybind::kernel_interface * kernel = pybind::initialize( false, false, false );

	if( kernel == nullptr )
	{
		return false;
	}
	
    LOGGER_ERROR( "pybind::initialize complete" );

	PyObject * py_tools_module = pybind::module_init( "ToolsBuilderPlugin" );

    pybind::registration_type_cast<Mengine::String>(kernel, new extract_String_type);
    pybind::registration_type_cast<Mengine::WString>(kernel, new extract_WString_type);

    pybind::registration_stl_vector_type_cast<Mengine::String, stdex::vector<Mengine::String>>(kernel);
    pybind::registration_stl_vector_type_cast<Mengine::WString, stdex::vector<Mengine::WString>>(kernel);

	pybind::interface_<Mengine::PythonLogger>( kernel, "XlsScriptLogger", true, py_tools_module)
		.def_native("write", &Mengine::PythonLogger::py_write )
		.def_native("flush", &Mengine::PythonLogger::py_flush )
		.def_property("softspace", &Mengine::PythonLogger::getSoftspace, &Mengine::PythonLogger::setSoftspace )
		.def_property("errors", &Mengine::PythonLogger::getErrors, &Mengine::PythonLogger::setErrors )
		.def_property("encoding", &Mengine::PythonLogger::getEncoding, &Mengine::PythonLogger::setEncoding )
		;
    Mengine::PythonLogger * logger = new Mengine::PythonLogger();
	PyObject * py_logger = pybind::ptr( kernel, logger);

	pybind::setStdOutHandle( py_logger );
	pybind::setStdErrorHandle( py_logger );

	pybind::def_function( kernel, "spreadingPngAlpha", &Mengine::spreadingPngAlpha, py_tools_module );
	pybind::def_function( kernel, "writeBin", &Mengine::writeBin, py_tools_module );
	pybind::def_function( kernel, "writeAek", &Mengine::writeAek, py_tools_module );


	pybind::def_function( kernel, "convert", &Mengine::convert, py_tools_module );
	pybind::def_function( kernel, "isAlphaInImageFile", &Mengine::isAlphaInImageFile, py_tools_module );
	pybind::def_function( kernel, "isUselessAlphaInImageFile", &Mengine::isUselessAlphaInImageFile, py_tools_module );

    Mengine::Image::embedding( kernel, py_tools_module );

	pybind::def_function( kernel, "loadImage", &Mengine::loadImage, py_tools_module );
	pybind::def_function( kernel, "saveImage", &Mengine::saveImage, py_tools_module );
	pybind::def_function_kernel( kernel, "createImage", &Mengine::createImage, py_tools_module );

	pybind::def_function( kernel, "pathSHA1", &Mengine::pathSHA1, py_tools_module );

	PyObject * module_builtins = pybind::get_builtins();

	pybind::def_function( kernel, "Error", &s_error, module_builtins );

	pybind::incref( py_tools_module );
	pybind::module_addobject( module_builtins, "ToolsBuilderPlugin", py_tools_module );

    Mengine::WChar currentDirectory[MAX_PATH];

	if( ::GetCurrentDirectory( MAX_PATH, currentDirectory ) == 0 )
	{
		return false;
	}

	PyObject * py_syspath = pybind::list_new( 0 );

	{
		WCHAR szPythonPath[512];
		if( getCurrentUserRegValue( szRegPath, szPythonPath, 512 ) == false )
		{
			LOGGER_ERROR( "invalid get reg value '%ls'"
				, szRegPath
				);
		}

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
			pybind::list_appenditem( py_syspath, py_stdpath );
			pybind::decref( py_stdpath );

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
	pybind::list_appenditem( py_syspath, py_currentpath );
	pybind::decref( py_currentpath );

	pybind::set_syspath( py_syspath );

	pybind::decref( py_syspath );

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
	PyObject * py_run_module = py_run_module = pybind::module_import( utf8_ModuleName.c_str(), exist_run );

	if( py_run_module == nullptr )
	{
        LOGGER_ERROR( "Module '%s' Function '%s' error"
            , utf8_ModuleName.c_str()
            , utf8_FunctionName.c_str()
        );

		return false;
	}

	PyObject * py_args = pybind::tuple_new( nArgs - 3 );

	for( int i = 3; i != nArgs; ++i )
	{
		LPWSTR arg = szArglist[i];
		
		pybind::tuple_setitem_t( kernel, py_args, i - 3, arg );
	}

	pybind::call_method_native( py_run_module, utf8_FunctionName.c_str(), py_args );

	pybind::decref( py_args );

	return true;
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
	(void)hInstance;
    (void)hPrevInstance;
    (void)lpCmdLine;
    (void)nCmdShow;
#elif __MINGW32__
    (void)argc;
    (void)argv;
#else
#   error unsupport
#endif

    Mengine::createConsole();

	run();
	
	return 0;
}