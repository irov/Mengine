#	include "VideoConverterFFMPEGToGVF.h"

#	include "Interface/FileSystemInterface.h"
#	include "Interface/UnicodeInterface.h"
#	include "Interface/StringizeInterface.h"
#	include "Interface/ImageCodecInterface.h"
#	include "Interface/RenderSystemInterface.h"
#	include "Interface/WindowsLayerInterface.h"

#	include "Logger/Logger.h"

#	include "Core/OutputStreamWriter.h"
#	include "Core/PixelFormat.h"

#   include "WindowsLayer/WindowsIncluder.h"

#   include "lz4.h"
#   include "lz4hc.h"

#	include "gvf_encoder.h"

#	include "stdex/ini.h"

namespace Menge
{
	static const uint32_t DDS_MAGIC = 0x20534444;

	static const uint32_t DDSD_CAPS = 0x00000001;
	static const uint32_t DDSD_HEIGHT = 0x00000002;
	static const uint32_t DDSD_WIDTH = 0x00000004;
	static const uint32_t DDSD_PITCH = 0x00000008;
	static const uint32_t DDSD_PIXELFORMAT = 0x00001000;
	static const uint32_t DDSD_MIPMAPCOUNT = 0x00020000;
	static const uint32_t DDSD_LINEARSIZE = 0x00080000;
	static const uint32_t DDSD_DEPTH = 0x00800000;

	//
	// DDPIXELFORMAT flags
	//
	static const uint32_t DDPF_ALPHAPIXELS = 0x00000001l;
	static const uint32_t DDPF_FOURCC = 0x00000004l;        // Compressed formats
	static const uint32_t DDPF_RGB = 0x00000040l;        // Uncompressed formats
	static const uint32_t DDPF_ALPHA = 0x00000002l;
	static const uint32_t DDPF_COMPRESSED = 0x00000080l;
	static const uint32_t DDPF_LUMINANCE = 0x00020000l;
	static const uint32_t DDPF_BUMPLUMINANCE = 0x00040000l;        // L,U,V
	static const uint32_t DDPF_BUMPDUDV = 0x00080000l;        // U,V

#   define FOURCC(c0, c1, c2, c3) (c0 | (c1 << 8) | (c2 << 16) | (c3 << 24))
	//////////////////////////////////////////////////////////////////////////
	static PixelFormat s_convertFourCCFormat(uint32_t fourcc)
	{
		// convert dxt pixel format
		switch(fourcc)
		{
		case FOURCC('D','X','T','1'):
			return PF_DXT1;
		case FOURCC('D','X','T','2'):
			return PF_DXT2;
		case FOURCC('D','X','T','3'):
			return PF_DXT3;
		case FOURCC('D','X','T','4'):
			return PF_DXT4;
		case FOURCC('D','X','T','5'):
			return PF_DXT5;
		}

		return PF_UNKNOWN;
	};

#pragma pack( push, 1 )
	struct DDS_PIXELFORMAT {
		uint32_t dwSize;
		uint32_t dwFlags;
		uint32_t dwFourCC;
		uint32_t dwRGBBitCount;
		uint32_t dwRBitMask;
		uint32_t dwGBitMask;
		uint32_t dwBBitMask;
		uint32_t dwABitMask;
	};

	typedef struct {
		uint32_t           dwSize;
		uint32_t           dwFlags;
		uint32_t           dwHeight;
		uint32_t           dwWidth;
		uint32_t           dwPitchOrLinearSize;
		uint32_t           dwDepth;
		uint32_t           dwMipMapCount;
		uint32_t           dwReserved1[11];
		DDS_PIXELFORMAT	 ddspf;
		uint32_t           dwSurfaceFlags;
		uint32_t           dwCubemapFlags;
		uint32_t           dwReserved2[3];
	} DDS_HEADER;
#pragma pack(pop)

	//////////////////////////////////////////////////////////////////////////
	static uint32_t s_firstPOW2From( uint32_t n )
	{
		--n;            
		n |= n >> 16;
		n |= n >> 8;
		n |= n >> 4;
		n |= n >> 2;
		n |= n >> 1;
		++n;
		return n;
	}
	//////////////////////////////////////////////////////////////////////////
	static int s_gvf_write( void * _user, const void * _buffer, uint32_t _size )
	{
		OutputStreamWriter * stream = (OutputStreamWriter *)_user;

		if( stream->write( _buffer, _size ) == false )
		{
			return GVF_WRITE_FAILED;
		}

		return GVF_WRITE_SUCCESSFUL;		
	}
	///////////////////////////////////////////////////////////////////////////////////////////////
	VideoConverterFFMPEGToGVF::VideoConverterFFMPEGToGVF()
	{
	}
	///////////////////////////////////////////////////////////////////////////////////////////////
	VideoConverterFFMPEGToGVF::~VideoConverterFFMPEGToGVF()
	{
	}
	///////////////////////////////////////////////////////////////////////////////////////////////
	bool VideoConverterFFMPEGToGVF::initialize()
	{
        m_convertExt = ".gvf";

        return true;
	}
	///////////////////////////////////////////////////////////////////////////////////////////////
	bool VideoConverterFFMPEGToGVF::convert()
	{
        FileGroupInterfacePtr fileGroup;
        if( FILE_SERVICE(m_serviceProvider)->hasFileGroup( m_options.pakName, &fileGroup ) == false )
        {
            LOGGER_ERROR(m_serviceProvider)("VideoConverterFFMPEGToGVF::convert_: not found file group '%s'"
                , m_options.pakName.c_str()
                );

            return false;
        }

        const ConstString & pakPath = fileGroup->getPath();

        String full_input = pakPath.c_str();
        full_input += m_options.inputFileName.c_str();

        String full_output = pakPath.c_str();
        full_output += m_options.outputFileName.c_str();

        WString unicode_input;
        if( Helper::utf8ToUnicode( m_serviceProvider, full_input, unicode_input ) == false )
        {
            LOGGER_ERROR(m_serviceProvider)("VideoConverterFFMPEGToGVF::convert_: invalid convert input utf8 to unicode %s"
                , full_input.c_str()
                );

            return false;        
		}

		WString unicode_output;
		if( Helper::utf8ToUnicode( m_serviceProvider, full_output, unicode_output ) == false )
		{
			LOGGER_ERROR(m_serviceProvider)("VideoConverterFFMPEGToGVF::convert_: invalid convert input utf8 to unicode %s"
				, full_input.c_str()
				);

			return false;        
		}

		WCHAR TempPath[MAX_PATH];
		GetTempPathW(MAX_PATH, TempPath);

		//remove temp dir
		{
			WString buffer = L"rmdir /S /Q \"%TEMP%/gvf_pngs\"";

			WINDOWSLAYER_SERVICE(m_serviceProvider)
				->cmd( buffer );
		}

		//create temp dir
		{
			WString buffer = L"mkdir \"%TEMP%/gvf_pngs\"";

			if( WINDOWSLAYER_SERVICE(m_serviceProvider)
				->cmd( buffer ) == false )
			{
				LOGGER_ERROR(m_serviceProvider)("VideoConverterFFMPEGToGVF::convert_: invalid cmd:\n%ls"
					, buffer.c_str()
					);

				return false;
			}
		}

		LOGGER_WARNING(m_serviceProvider)("gvf try get fps");

		//get fps 
		{
			WString buffer = L"ffprobe -v quiet -print_format ini -show_streams -i \"" + unicode_input + L"\" > \"%TEMP%/gvf_pngs/info.ini\"";
			//WString buffer = L"ffmpeg -loglevel error -i \"" + unicode_input + L"\" -r 24 -f image2 \"%TEMP%/gvf_pngs/frame_%3d.png\"";
			
			if( WINDOWSLAYER_SERVICE(m_serviceProvider)
				->cmd( buffer ) == false )
			{
				LOGGER_ERROR(m_serviceProvider)("VideoConverterFFMPEGToWEBM::convert_: invalid cmd:\n%ls"
					, buffer.c_str()
					);

				return false;
			}
		}

		uint32_t fps = 0;
		uint32_t alpha = 0;

		{
			WCHAR INI_Path[MAX_PATH];
			swprintf_s( INI_Path, MAX_PATH, L"%sgvf_pngs\\info.ini", TempPath );

			String utf8_INI_Path;
			Helper::unicodeToUtf8( m_serviceProvider, INI_Path, utf8_INI_Path );

			InputStreamInterfacePtr stream = FILE_SERVICE(m_serviceProvider)
				->openInputFile( CONST_STRING_LOCAL( m_serviceProvider, "dev"), Helper::stringizeString(m_serviceProvider, utf8_INI_Path) );
			
			size_t size = stream->size();

			char * buffer = new char[size];
			stream->read( buffer, size );
			stream = nullptr;

			stdex::ini ini;

			if( ini.load( buffer ) == false )
			{
				return false;
			}

			const char * r_frame_rate = ini.getSettingValue( "streams.stream.0", "r_frame_rate" );

			uint32_t frame_count;

			if( sscanf( r_frame_rate, "%d/%d"
				, &fps
				, &frame_count ) != 2 )
			{
				return false;
			}

			fps /= frame_count;

			const char * codec_name = ini.getSettingValue( "streams.stream.0", "codec_name" );

			if( strcmp( codec_name, "vp6a" ) == 0 )
			{
				alpha = 1;
			}
		}

		LOGGER_WARNING(m_serviceProvider)("fps %d"
			, fps
			);		        

		LOGGER_WARNING(m_serviceProvider)("gvf create pngs");

		//create pngs
		{
			WString ws_fps;
			Utils::unsignedToWString( fps, ws_fps );
			WString buffer = L"ffmpeg -loglevel error -i \"" + unicode_input + L"\" -r " + ws_fps + L" -f image2 \"%TEMP%/gvf_pngs/frame_%3d.png\"";

			if( WINDOWSLAYER_SERVICE(m_serviceProvider)
				->cmd( buffer ) == false )
			{
				LOGGER_ERROR(m_serviceProvider)("VideoConverterFFMPEGToWEBM::convert_: invalid cmd:\n%ls"
					, buffer.c_str()
					);

				return false;
			}
		}

		size_t pngs_index = 1;
						
		{
			while( true, true )
			{			
				WCHAR PNG_Path[MAX_PATH];
				swprintf_s( PNG_Path, MAX_PATH, L"%sgvf_pngs\\frame_%03d.png", TempPath, pngs_index );

				DWORD dwAttrib = GetFileAttributesW( PNG_Path );

				if( dwAttrib == INVALID_FILE_ATTRIBUTES || dwAttrib & FILE_ATTRIBUTE_DIRECTORY )
				{
					break;
				}

				++pngs_index;
			}
		}

		uint32_t frame_count = pngs_index - 1;

		LOGGER_WARNING(m_serviceProvider)("gvf frames %d"
			, frame_count
			);

		LOGGER_WARNING(m_serviceProvider)("gvf create dds");

		//create dds
		{
			WCHAR PNGS_Path[MAX_PATH];
			swprintf_s( PNGS_Path, MAX_PATH, L"%sgvf_pngs\\frames.txt", TempPath );

			String utf8_pngs_path;
			Helper::unicodeToUtf8(m_serviceProvider, PNGS_Path, utf8_pngs_path);

			OutputStreamInterfacePtr pngs_stream = FILE_SERVICE(m_serviceProvider)
				->openOutputFile( CONST_STRING_LOCAL( m_serviceProvider, "dev"), Helper::stringizeString(m_serviceProvider, utf8_pngs_path) );

			for( size_t index = 1; index != pngs_index; ++index )
			{	
				WCHAR PNG_Path[MAX_PATH];
				swprintf_s( PNG_Path, MAX_PATH, L"%sgvf_pngs\\frame_%03d.png\n", TempPath, index );

				String utf8_png_path;
				Helper::unicodeToUtf8(m_serviceProvider, PNG_Path, utf8_png_path);				

				pngs_stream->write( utf8_png_path.c_str(), utf8_png_path.size() );
				//WCHAR DDS_Path[MAX_PATH];
				//swprintf_s( DDS_Path, MAX_PATH, L"%sgvf_pngs\\frame_%03d.dds", TempPath, index );
			}

			pngs_stream->flush();
			pngs_stream = nullptr;

			WString buffer = L"crunch.exe -quiet -mipMode None -fileformat dds ";

			if( alpha == 1 )
			{
				buffer += L" -DXT5 ";				
			}
			else
			{
				buffer += L" -DXT1 ";
			}
			
			buffer += L" -dxtQuality superfast -rescalemode hi -outsamedir ";

			buffer += L" -file @";
			buffer += PNGS_Path;

			if( WINDOWSLAYER_SERVICE(m_serviceProvider)
					->cmd( buffer ) == false )
			{
				LOGGER_ERROR(m_serviceProvider)("VideoConverterFFMPEGToGVF::convert_: invalid cmd:\n%ls"
					, buffer.c_str()
					);

				return false;
			}
			//crunch.exe -mipMode None -fileformat dds -DXT5 -dxtQuality superfast
		}
				
		size_t width = 0;
		size_t height = 0;

		{
			WCHAR PNG_Path[MAX_PATH];
			swprintf_s( PNG_Path, MAX_PATH, L"%sgvf_pngs\\frame_%03d.png", TempPath, 1 );

			String utf8_PNG_Path;
			Helper::unicodeToUtf8( m_serviceProvider, PNG_Path, utf8_PNG_Path );

			InputStreamInterfacePtr stream = FILE_SERVICE(m_serviceProvider)
				->openInputFile( CONST_STRING_LOCAL( m_serviceProvider, "dev"), Helper::stringizeString(m_serviceProvider, utf8_PNG_Path) );

			ImageDecoderInterfacePtr imageDecoder = CODEC_SERVICE(m_serviceProvider)
				->createDecoderT<ImageDecoderInterfacePtr>( CONST_STRING_LOCAL(m_serviceProvider, "pngImage") );

			if( imageDecoder == nullptr )
			{
				LOGGER_ERROR(m_serviceProvider)("VideoConverterFFMPEGToGVF::convert_: Image decoder for file '%s' was not found"
					, m_options.inputFileName.c_str() 
					);

				return nullptr;
			}

			if( imageDecoder->initialize( stream ) == false )
			{
				LOGGER_ERROR(m_serviceProvider)("VideoConverterFFMPEGToGVF::convert_: Image initialize for file '%s' was not found"
					, m_options.inputFileName.c_str() 
					);

				return nullptr;
			}

			const ImageCodecDataInfo* dataInfo = imageDecoder->getCodecDataInfo();

			width = dataInfo->width;
			height = dataInfo->height;
		}

		LOGGER_WARNING(m_serviceProvider)("gvf save file");
		
		OutputStreamInterfacePtr gvf_stream = FILE_SERVICE(m_serviceProvider)
			->openOutputFile( CONST_STRING_LOCAL( m_serviceProvider, "dev"), Helper::stringizeString(m_serviceProvider, full_output) );

		OutputStreamWriter gvf_wr(gvf_stream);

		gvf_encoder * gvf;
		gvf_error_t err_code;

		err_code = gvf_encoder_create( &gvf, &gvf_wr, &s_gvf_write );

		if( err_code != GVF_ERROR_SUCCESSFUL )
		{
			LOGGER_ERROR(m_serviceProvider)("VideoConverterFFMPEGToGVF::convert_: '%ls' gvf_encoder_create err code %d"
				, m_options.inputFileName.c_str() 
				, err_code
				);

			return false;
		}

		uint16_t gvf_format = alpha == 1 ? GVF_FORMAT_DXT5 : GVF_FORMAT_DXT1;

		err_code = gvf_encoder_header( gvf, gvf_format, width, height, fps, frame_count );

		if( err_code != GVF_ERROR_SUCCESSFUL )
		{
			LOGGER_ERROR(m_serviceProvider)("VideoConverterFFMPEGToGVF::convert_: '%ls' gvf_encoder_header err code %d"
				, m_options.inputFileName.c_str() 
				, err_code
				);

			return false;
		}

		size_t pow2_width = s_firstPOW2From( width );
		size_t pow2_height = s_firstPOW2From( height );

		PixelFormat pf = alpha == 1 ? PF_DXT5 : PF_DXT1;

		size_t frame_size = Helper::getTextureMemorySize( pow2_width, pow2_height, 1, pf );

		
		LOGGER_WARNING(m_serviceProvider)("gvf save frames");

		{
			for( size_t index = 0; index != frame_count; ++index )
			{
				WCHAR DDS_Path[MAX_PATH];
				swprintf_s( DDS_Path, MAX_PATH, L"%sgvf_pngs\\frame_%03d.dds", TempPath, index + 1 );

				String utf8_DDS_Path;
				Helper::unicodeToUtf8( m_serviceProvider, DDS_Path, utf8_DDS_Path );
				
				InputStreamInterfacePtr stream = FILE_SERVICE(m_serviceProvider)
					->openInputFile( CONST_STRING_LOCAL( m_serviceProvider, "dev"), Helper::stringizeString(m_serviceProvider, utf8_DDS_Path) );

				uint32_t magic;
				stream->read( &magic, sizeof(magic) );

				if( magic != DDS_MAGIC )
				{
					LOGGER_ERROR(m_serviceProvider)("VideoConverterFFMPEGToGVF::convert_: invalid dds file magic number %ls" 
						, DDS_Path
						);

					return false;
				}

				DDS_HEADER header;
				stream->read( &header, sizeof(header) );

				//Check valid structure sizes
				if( header.dwSize != 124 && header.ddspf.dwSize != 32)
				{
					LOGGER_ERROR(m_serviceProvider)("VideoConverterFFMPEGToGVF::convert_: invalid dds file header %ls" 
						, DDS_Path
						);

					return false;
				}

				if( (header.dwFlags & DDSD_MIPMAPCOUNT) == DDSD_MIPMAPCOUNT && header.dwMipMapCount > 0 )
				{
					LOGGER_WARNING(m_serviceProvider)("VideoConverterFFMPEGToGVF::convert_: dds file has mipmaps %ls" 
						, DDS_Path
						);                        

					return false;				 
				}

				unsigned char * buffer = new unsigned char [frame_size];
				stream->read( buffer, frame_size );
				stream = nullptr;

				err_code = gvf_encoder_frame( gvf, index, buffer, frame_size );

				if( err_code != GVF_ERROR_SUCCESSFUL )
				{
					LOGGER_ERROR(m_serviceProvider)("VideoConverterFFMPEGToGVF::convert_: '%ls' gvf_encoder_frame %d err code %d"
						, m_options.inputFileName.c_str() 
						, index
						, err_code
						);

					return false;
				}

				delete [] buffer;
			}
		}

		err_code = gvf_encoder_flush( gvf );

		if( err_code != GVF_ERROR_SUCCESSFUL )
		{
			LOGGER_ERROR(m_serviceProvider)("VideoConverterFFMPEGToGVF::convert_: '%ls' gvf_encoder_flush err code %d"
				, m_options.inputFileName.c_str() 
				, err_code
				);

			return false;
		}

		gvf_encoder_destroy( gvf );

		LOGGER_WARNING(m_serviceProvider)("gvf finish");
		
		//remove temp dir
		{
			WString buffer = L"rmdir /S /Q \"%TEMP%/gvf_pngs\"";

			if( WINDOWSLAYER_SERVICE(m_serviceProvider)
				->cmd( buffer ) == false )
			{
				LOGGER_ERROR(m_serviceProvider)("VideoConverterFFMPEGToGVF::convert_: invalid cmd:\n%ls"
					, buffer.c_str()
					);

				return false;
			}
		}

        return true;
	}
}