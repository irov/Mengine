#	include "HotspotImageConverterPNGToHIT.h"

#	include "Interface/FileSystemInterface.h"
#	include "Interface/UnicodeInterface.h"
#	include "Interface/StringizeInterface.h"

#	include "Interface/ImageCodecInterface.h"
#	include "Interface/PickCodecInterface.h"

#	include "Logger/Logger.h"

#   include <math.h>

namespace Menge
{
	///////////////////////////////////////////////////////////////////////////////////////////////
	HotspotImageConverterPNGToHIT::HotspotImageConverterPNGToHIT( ServiceProviderInterface * _serviceProvider )
		: HotspotImageConverter(_serviceProvider)
	{
	}
	///////////////////////////////////////////////////////////////////////////////////////////////
	HotspotImageConverterPNGToHIT::~HotspotImageConverterPNGToHIT()
	{
	}
	///////////////////////////////////////////////////////////////////////////////////////////////
	bool HotspotImageConverterPNGToHIT::initialize()
	{
        m_convertExt = ".hit";

        return true;
	}
    //////////////////////////////////////////////////////////////////////////
    const String & HotspotImageConverterPNGToHIT::getConvertExt() const
    {
        return m_convertExt;
    }
	///////////////////////////////////////////////////////////////////////////////////////////////
	bool HotspotImageConverterPNGToHIT::convert( bool & _skip )
	{
        _skip = false;

		if( m_options.inputFileName.empty() == true || m_options.outputFileName.empty() == true )
		{
			LOGGER_ERROR(m_serviceProvider)("HotspotImageConverterPNGToHIT::convert input(%s) or output(%s) file name is empty"
                , m_options.inputFileName.c_str()
                , m_options.outputFileName.c_str()
                );

			return false;
		}

 		const FilePath & inputFile = m_options.inputFileName;
		
		if( FILE_SERVICE(m_serviceProvider)
            ->existFile( m_options.pakName, inputFile, NULL ) == false )
		{
			LOGGER_ERROR(m_serviceProvider)( "HotspotImageConverterPNGToHIT::convert: Can't open  input file '%s'"
				, inputFile.c_str()
				);

			return false;
		}


        const FilePath & outputFile = m_options.outputFileName;
        
		if( FILE_SERVICE(m_serviceProvider)
            ->existFile( m_options.pakName, outputFile, NULL ) == false )
		{
			if( this->convert_( m_options.inputFileName, outputFile ) == false)
			{
				LOGGER_ERROR(m_serviceProvider)( "HotspotImageConverterPNGToHIT::convert Can't convert png file '%s'"
					, m_options.inputFileName.c_str()
					);
		
				return false;
			}
		}
		else
		{			
			InputStreamInterface * oldFile = FILE_SERVICE(m_serviceProvider)
                ->openInputFile( m_options.pakName, inputFile );

			if( oldFile == NULL )
			{
				return false;
			}

            time_t fileTimeInput;
			oldFile->time(fileTimeInput);
			
            oldFile->destroy();
			oldFile = NULL;

			
			InputStreamInterface * newFile = FILE_SERVICE(m_serviceProvider)
                ->openInputFile( m_options.pakName, outputFile );

			if( newFile == NULL )
			{
				return false;
			}

            time_t fileTimeOutput;
			newFile->time(fileTimeOutput);

			newFile->destroy();
			newFile = NULL;

			if( fileTimeInput > fileTimeOutput )
			{
				if( this->convert_( inputFile, outputFile ) == false)
				{
					LOGGER_ERROR(m_serviceProvider)( "HotspotImageConverterPNGToHIT::convert Can't convert png file '%s'"
						, inputFile.c_str()
						);
							
					return false;
				}
			}
            else
            {
                _skip = true;
            }
		}

		return true;
	}
	///////////////////////////////////////////////////////////////////////////////////////////////
	bool HotspotImageConverterPNGToHIT::convert_( const FilePath & _input, const FilePath & _output )
	{
        InputStreamInterface * input_stream = FILE_SERVICE(m_serviceProvider)
            ->openInputFile( m_options.pakName, _input );

        if( input_stream == 0 )
        {
            LOGGER_ERROR(m_serviceProvider)( "HotspotImageConverterPNGToHIT::convert_: Image file '%s' was not found"
                , _input.c_str() 
                );

            return NULL;
        }


        ImageDecoderInterface * imageDecoder = 
            CODEC_SERVICE(m_serviceProvider)->createDecoderT<ImageDecoderInterface>( Helper::stringizeString(m_serviceProvider, "pngImage"), input_stream );

        if( imageDecoder == 0 )
        {
            LOGGER_ERROR(m_serviceProvider)( "HotspotImageConverterPNGToHIT::convert_: Image decoder for file '%s' was not found"
                , _input.c_str() 
                );

            input_stream->destroy();

            return NULL;
        }

        const ImageCodecDataInfo* dataInfo = imageDecoder->getCodecDataInfo();

        ImageCodecOptions optionsAlpha;

        optionsAlpha.flags |= DF_READ_ALPHA_ONLY;
        //optionsAlpha.flags |= DF_CUSTOM_PITCH;
        optionsAlpha.pitch = dataInfo->width;
        optionsAlpha.channels = 1;

        imageDecoder->setOptions( &optionsAlpha );

        size_t width = dataInfo->width;
        size_t height = dataInfo->height;

        size_t mimmap_level;
        size_t mimmap_size = this->calcMimMapBufferLevelAndSize_( width, height, mimmap_level );

        size_t bufferSize = width * height;        
        unsigned char * buffer = new unsigned char [bufferSize + mimmap_size];

        if( imageDecoder->decode( buffer, bufferSize ) == 0 )
        {
            LOGGER_ERROR(m_serviceProvider)("HotspotImageConverterPNGToHIT::convert_ Invalid decode %s"
                , _input.c_str()                
                );
            
            imageDecoder->destroy();

            input_stream->destroy();

            return false;
        }

        input_stream->destroy();

        this->makeMipMapLevel_( buffer, width, height, mimmap_level );

        OutputStreamInterface * output_stream = FILE_SERVICE(m_serviceProvider)
            ->openOutputFile( m_options.pakName, _output );

        if( output_stream == NULL )
        {
            LOGGER_ERROR(m_serviceProvider)( "HotspotImageConverterPNGToHIT::convert_: HIT file '%s' not create (open file %s)"
                , _output.c_str() 
                , m_options.pakName.c_str()
                );

            return NULL;
        }

        PickEncoderInterface * encoder = CODEC_SERVICE(m_serviceProvider)
            ->createEncoderT<PickEncoderInterface>(Helper::stringizeString(m_serviceProvider, "hitPick"), output_stream);
        
        if( encoder == NULL )
        {
            LOGGER_ERROR(m_serviceProvider)( "HotspotImageConverterPNGToHIT::convert_: HIT file '%s' not create (createEncoder hitPick)"
                , _output.c_str() 
                );

            return NULL;
        }

        PickCodecDataInfo di;

        di.width = width;
        di.height = height;
        di.mipmaplevel = mimmap_level;
        di.mipmapsize = bufferSize + mimmap_size;

        encoder->encode( buffer, &di );

        delete [] buffer;

        encoder->destroy();
        output_stream->destroy();

        return true;
	}
    //////////////////////////////////////////////////////////////////////////
    size_t HotspotImageConverterPNGToHIT::calcMimMapBufferLevelAndSize_( size_t _width, size_t _height, size_t & _level )
    {
        size_t mipmap_pow_width = (size_t)(logf( (float)_width ) / logf( 2.f ));
        size_t mipmap_pow_height = (size_t)(logf( (float)_height ) / logf( 2.f ));

        size_t mipmap_level = (std::min)(mipmap_pow_width, mipmap_pow_height) + 1;

        size_t bufferOffset = 0;
        for( size_t i = 1; i != mipmap_level; ++i )
        {
            size_t offset = (_width >> i) * (_height >> i);
            bufferOffset += offset;
        }

        _level = mipmap_level;

        return bufferOffset;
    }
    //////////////////////////////////////////////////////////////////////////
    void HotspotImageConverterPNGToHIT::makeMipMapLevel_( unsigned char * _buffer, size_t _width, size_t _height, size_t _level )
    {
        unsigned char ** mipmap = new unsigned char * [_level];
        
        mipmap[0] = _buffer;

        size_t bufferOffset = _width * _height;
        for( size_t i = 1; i != _level; ++i )
        {
            mipmap[i] = _buffer + bufferOffset;

            size_t offset = (_width >> i) * (_height >> i);
            bufferOffset += offset;
        }

        size_t mipmap_width = _width;
        size_t mipmap_height = _height;

        for( size_t
            it = 1,
            it_end = _level;
        it != it_end;
        ++it )
        {
            size_t prev_width = mipmap_width >> (it - 1);
            //size_t prev_height = mipmap_height >> (it - 1);

            size_t cur_width = mipmap_width >> it;
            size_t cur_height = mipmap_height >> it;

            unsigned char * prev_buff = mipmap[it - 1];
            unsigned char * cur_buff = mipmap[it];

            for( size_t i = 0; i != cur_width; ++i )
            {
                for( size_t j = 0; j != cur_height; ++j )
                {
                    size_t i0 = (i * 2 + 0) + (j * 2 + 0) * prev_width;
                    size_t i1 = (i * 2 + 1) + (j * 2 + 0) * prev_width;
                    size_t i2 = (i * 2 + 0) + (j * 2 + 1) * prev_width;
                    size_t i3 = (i * 2 + 1) + (j * 2 + 1) * prev_width;

                    unsigned char v0 = prev_buff[i0];
                    unsigned char v1 = prev_buff[i1];
                    unsigned char v2 = prev_buff[i2];
                    unsigned char v3 = prev_buff[i3];

                    unsigned char b0 = (std::max)( v0, v1 );
                    unsigned char b1 = (std::max)( v2, v3 );

                    unsigned char b_max = (std::max)( b0, b1 );

                    cur_buff[ i + j * cur_width ] = b_max;
                }
            }
        }

        delete [] mipmap;
    }
}