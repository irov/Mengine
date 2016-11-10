#	include "ToolUtils/ToolUtils.h"

#	include "pugixml.hpp"

#	include <vector>
#	include <string>
#	include <sstream>

//////////////////////////////////////////////////////////////////////////
int APIENTRY wWinMain( HINSTANCE hInstance, HINSTANCE hPrevInstance, PWSTR lpCmdLine, int nShowCmd )
{
	(void)hInstance;
	(void)hPrevInstance;
	(void)nShowCmd;

	std::wstring texturepacker_path = parse_kwds( lpCmdLine, L"--texturepacker", std::wstring() );
	std::wstring image_path = parse_kwds( lpCmdLine, L"--image", std::wstring() );
	uint32_t offset_x = parse_kwds( lpCmdLine, L"--offset_x", 0U );
	uint32_t offset_y = parse_kwds( lpCmdLine, L"--offset_y", 0U );
	float width = parse_kwds( lpCmdLine, L"--width", -1.f );
	float height = parse_kwds( lpCmdLine, L"--height", -1.f );

	if( texturepacker_path.empty() == true )
	{
		message_error( "not found 'texturepacker' param\n"
			);

		return 0;
	}

	if( image_path.empty() == true )
	{
		message_error( "not found 'image' param\n"
			);

		return 0;
	}

	std::wstring system_cmd;

	system_cmd += L" --shape-padding 0 ";
	system_cmd += L" --border-padding 0 ";
	system_cmd += L" --padding 0 ";
	system_cmd += L" --disable-rotation ";	
	system_cmd += L" --extrude 0 ";
	system_cmd += L" --trim-mode Polygon ";
	system_cmd += L" --trim-threshold 0 ";	

	WCHAR ImagePathCanonicalizeQuote[MAX_PATH];
	ForcePathQuoteSpaces( ImagePathCanonicalizeQuote, image_path );
	
	system_cmd += ImagePathCanonicalizeQuote;

	WCHAR tempPath[MAX_PATH];
	GetTempPath( MAX_PATH, tempPath );

	WCHAR outCanonicalize[MAX_PATH];
	PathCombine( outCanonicalize, tempPath, L"aemovie_temp_texturepacker_sheet.xml" );
	//PathCanonicalize( outCanonicalize, L"%TEMP%/temp_texturepacker_sheet.xml" );
	//PathUnquoteSpaces( outCanonicalize );

	system_cmd += L" --data ";
	system_cmd += outCanonicalize;
	system_cmd += L" --format xml ";
	system_cmd += L" --quiet ";

	STARTUPINFO lpStartupInfo;
	ZeroMemory( &lpStartupInfo, sizeof( STARTUPINFOW ) );
	lpStartupInfo.cb = sizeof( lpStartupInfo );

	PROCESS_INFORMATION lpProcessInformation;
	ZeroMemory( &lpProcessInformation, sizeof( PROCESS_INFORMATION ) );

	WCHAR lpCommandLine[32768];
	wcscpy_s( lpCommandLine, system_cmd.c_str() );

	WCHAR TexturePathCanonicalizeQuote[MAX_PATH];
	ForcePathQuoteSpaces( TexturePathCanonicalizeQuote, texturepacker_path );
	PathUnquoteSpaces( TexturePathCanonicalizeQuote );

	if( CreateProcess( TexturePathCanonicalizeQuote
		, lpCommandLine
		, NULL
		, NULL
		, FALSE
		, CREATE_NO_WINDOW
		, NULL
		, NULL
		, &lpStartupInfo
		, &lpProcessInformation ) == FALSE )
	{
		message_error( "invalid CreateProcess %ls %ls\n"
			, TexturePathCanonicalizeQuote
			, lpCommandLine
			);

		return 0;
	}

	CloseHandle( lpProcessInformation.hThread );

	WaitForSingleObject( lpProcessInformation.hProcess, INFINITE );

	DWORD exit_code;//Код завершения процесса
	GetExitCodeProcess( lpProcessInformation.hProcess, &exit_code );

	CloseHandle( lpProcessInformation.hProcess );

	if( exit_code != 0 )
	{
		message_error( "invalid Process %ls exit_code %d\n"
			, TexturePathCanonicalizeQuote
			, exit_code
			);

		return 0;
	}

	FILE * f = _wfopen( outCanonicalize, L"rb" );

	if( f == NULL )
	{
		message_error( "invalid _wfopen %ls\n"
			, outCanonicalize
			);

		return 0;
	}

	fseek( f, 0, SEEK_END );
	int f_size = ftell( f );
	rewind( f );

	std::vector<char> v_buffer( f_size );
	char * mf_buffer = &v_buffer[0];

	fread( mf_buffer, 1, f_size, f );
	fclose( f );

	pugi::xml_document doc;

	pugi::xml_parse_result result = doc.load_buffer( mf_buffer, f_size );

	pugi::xml_node xml_sprite = doc.first_element_by_path( "TextureAtlas/sprite" );
	pugi::xml_attribute xml_sprite_x = xml_sprite.attribute( "x" );
	pugi::xml_attribute xml_sprite_y = xml_sprite.attribute( "y" );
	pugi::xml_attribute xml_sprite_w = xml_sprite.attribute( "w" );
	pugi::xml_attribute xml_sprite_h = xml_sprite.attribute( "h" );
	pugi::xml_attribute xml_sprite_oX = xml_sprite.attribute( "oX" );
	pugi::xml_attribute xml_sprite_oY = xml_sprite.attribute( "oY" );
	pugi::xml_attribute xml_sprite_oW = xml_sprite.attribute( "oW" );
	pugi::xml_attribute xml_sprite_oH = xml_sprite.attribute( "oH" );

	uint32_t x = xml_sprite_x.as_uint();
	uint32_t y = xml_sprite_y.as_uint();
	uint32_t w = xml_sprite_w.as_uint();
	uint32_t h = xml_sprite_h.as_uint();
	uint32_t oX = xml_sprite_oX.as_uint();
	uint32_t oY = xml_sprite_oY.as_uint();
	uint32_t oW = xml_sprite_oW.as_uint();
	uint32_t oH = xml_sprite_oH.as_uint();

	pugi::xml_node xml_vertices = doc.first_element_by_path( "TextureAtlas/sprite/vertices" );

	const char * vertices = xml_vertices.child_value();

	std::stringstream ss_vertices( vertices );

	//Sleep( 10000 );

	std::vector<float> positions;

	for( ;; )
	{
		int32_t pos_x;
		int32_t pos_y;
		if( ss_vertices >> pos_x && ss_vertices >> pos_y )
		{
			pos_x -= x;
			pos_y -= y;

			pos_x += offset_x;
			pos_y += offset_y;

			float xf = (float)pos_x;
			float yf = (float)pos_y;

			positions.push_back( xf );
			positions.push_back( yf );
		}
		else
		{
			break;
		}
	}

	pugi::xml_node xml_verticesUV = doc.first_element_by_path( "TextureAtlas/sprite/verticesUV" );

	const char * verticesUV = xml_verticesUV.child_value();

	if( width < 0.f )
	{
		width = (float)w;
	}

	if( height < 0.f )
	{
		height = (float)h;
	}

	std::stringstream ss_verticesUV( verticesUV );

	std::vector<float> uvs;

	for( ;; )
	{
		uint32_t uv_x;
		uint32_t uv_y;
		if( ss_verticesUV >> uv_x && ss_verticesUV >> uv_y )
		{
			uv_x -= x;
			uv_y -= y;
			uv_x += oX;
			uv_y += oY;

			float xf = (float)uv_x / (float)width;
			float yf = (float)uv_y / (float)height;

			uvs.push_back( xf );
			uvs.push_back( yf );
		}
		else
		{
			break;
		}
	}

	pugi::xml_node xml_triangles = doc.first_element_by_path( "TextureAtlas/sprite/triangles" );

	const char * triangles = xml_triangles.child_value();

	std::stringstream ss_triangles( triangles );

	std::vector<uint16_t> indices;

	for( ;; )
	{
		uint16_t index;
		if( ss_triangles >> index )
		{
			indices.push_back( index );
		}
		else
		{
			break;
		}
	}

	uint32_t vertex_count = positions.size() / 2;
	uint32_t indices_count = indices.size();

	printf( "vertex_count=%u\n", vertex_count );
	printf( "indices_count=%u\n", indices_count );

	printf( "positions=" );

	for( std::vector<float>::const_iterator
		it = positions.begin(),
		it_end = positions.end();
	it != it_end;
	++it )
	{
		const float v = *it;

		printf( " %12f", v );
	}

	printf( "\n" );

	printf( "uvs=" );

	for( std::vector<float>::const_iterator
		it = uvs.begin(),
		it_end = uvs.end();
	it != it_end;
	++it )
	{
		const float v = *it;

		printf( " %12f", v );
	}

	printf( "\n" );

	printf( "indices=" );

	for( std::vector<uint16_t>::const_iterator
		it = indices.begin(),
		it_end = indices.end();
	it != it_end;
	++it )
	{
		const uint16_t v = *it;

		printf( " %u", v );
	}

	printf( "\n" );

	return 0;
}