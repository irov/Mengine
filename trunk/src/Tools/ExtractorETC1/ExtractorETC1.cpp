#	ifdef _WIN32_WINNT	
#       undef _WIN32_WINNT
#       define _WIN32_WINNT 0x0500
#   endif

#   ifdef _WIN32_WINDOWS
#       undef _WIN32_WINDOWS
#       define _WIN32_WINDOWS 0x0500
#   endif

#	define WIN32_LEAN_AND_MEAN
#	include <Windows.h>

#	include <Shlwapi.h>
#	include <shellapi.h>

#	include <vector>
#	include <string>
#	include <sstream>

#	include <stdint.h>
//////////////////////////////////////////////////////////////////////////
#pragma pack( push, 1 )
struct PVRTextureHeader
{
	uint32_t    version;            
	uint32_t    flags;          
	uint64_t    pixelFormat;        
	uint32_t    colourSpace;        
	uint32_t    channelType;        
	uint32_t    height;         
	uint32_t    width;          
	uint32_t    depth;          
	uint32_t    numSurfaces;        
	uint32_t    numFaces;       
	uint32_t    numMipmaps;     
	uint32_t    metaDataSize; 
};
#pragma pack(pop)

const unsigned int PVRTEX_MIPMAP = (1<<8); // has mip map levels
const unsigned int PVRTEX_TWIDDLE = (1<<9); // is twiddled
const unsigned int PVRTEX_BUMPMAP = (1<<10); // has normals encoded for a bump map
const unsigned int PVRTEX_TILING = (1<<11); // is bordered for tiled pvr
const unsigned int PVRTEX_CUBEMAP = (1<<12); // is a cubemap/skybox
const unsigned int PVRTEX_FALSEMIPCOL  = (1<<13); //
const unsigned int PVRTEX_VOLUME = (1<<14);
const unsigned int PVRTEX_PIXELTYPE = 0xff; // pixel type is always in the last 16bits of the flags
const unsigned int PVRTEX_IDENTIFIER  = 0x21525650;  // the pvr identifier is the characters 'P','V','R'

const unsigned int PVRTEX_V1_HEADER_SIZE = 44; // old header size was 44 for identification purposes
//////////////////////////////////////////////////////////////////////////
static void ForcePathQuoteSpaces( WCHAR * _quotePath, const std::wstring & _path )
{
	if( _path.empty() == true )
	{
		wcscpy_s( _quotePath, 2, L"" );

		return;
	}

	std::wstring true_path = _path;

	if( _path[0] == L'/' )
	{
		true_path[0] = true_path[1];
		true_path[1] = L':';
	}

	const WCHAR * pathBuffer = true_path.c_str();
	size_t pathSize = true_path.size();

	PathCanonicalize( _quotePath, pathBuffer );
	if( PathQuoteSpaces( _quotePath ) == FALSE )
	{
		wmemmove( _quotePath + 1, _quotePath, pathSize );
		_quotePath[0] = '\"';
		_quotePath[pathSize + 1] = '\"';
		_quotePath[pathSize + 2] = 0;
	}
};
//////////////////////////////////////////////////////////////////////////
static void message_error( const char * _format, ... )
{
	va_list argList;

	va_start(argList, _format);

	char str[2048];

	vsnprintf( str, 2048 - 1, _format, argList );

	va_end(argList);

	MessageBoxA( NULL, str, "ExtractorETC1", MB_OK );
}
//////////////////////////////////////////////////////////////////////////
int APIENTRY wWinMain( HINSTANCE hInstance, HINSTANCE hPrevInstance, PWSTR lpCmdLine, int nShowCmd )
{
	(void)hInstance;
	(void)hPrevInstance;
	(void)nShowCmd;

	int cmd_num;
	LPWSTR * cmd_args = CommandLineToArgvW( lpCmdLine, &cmd_num );

	std::wstring in;
	std::wstring out;

	for( int i = 0; i < cmd_num; i += 2 )
	{
		LPWSTR arg = cmd_args[i + 0];
		LPWSTR value = cmd_args[i + 1];

		if( wcscmp( arg, L"-in" ) == 0 )
		{
			in = value;
		}
		else if( wcscmp( arg, L"-out" ) == 0 )
		{
			out = value;
		}
	}

	if( in.empty() == true )
	{
		message_error("not found 'in' param\n"
			);

		return 1;
	}

	if( out.empty() == true )
	{
		message_error("not found 'out' param\n"
			);

		return 1;
	}

	WCHAR inCanonicalizeQuote[MAX_PATH];
	ForcePathQuoteSpaces( inCanonicalizeQuote, in.c_str() );
	PathUnquoteSpaces( inCanonicalizeQuote );
	
	FILE * file_in = _wfopen( inCanonicalizeQuote, L"rb" );

	if( file_in == NULL )
	{
		message_error("in file not found %ls"
			, inCanonicalizeQuote
			);

		return 1;
	}

	PVRTextureHeader header;
	fread( &header, sizeof(header), 1, file_in );

	fseek( file_in, header.metaDataSize, SEEK_CUR );

	uint32_t w = (header.width + 3) / 4;
	uint32_t h = (header.height + 3) / 4;

	size_t size = w * h * 1 * 8;
			
	uint8_t * etc1_byte = new uint8_t [size];
	fread( etc1_byte, 1, size, file_in );
	fclose( file_in );

	WCHAR outCanonicalizeQuote[MAX_PATH];
	ForcePathQuoteSpaces( outCanonicalizeQuote, out.c_str() );
	PathUnquoteSpaces( outCanonicalizeQuote );

	FILE * file_out = _wfopen( outCanonicalizeQuote, L"wb" );

	if( file_out == NULL )
	{
		message_error("out file not fount %ls"
			, inCanonicalizeQuote
			);

		return 1;
	}

	fwrite( etc1_byte, 1, size, file_out );
	fclose( file_out );	
			
	return 0;
}