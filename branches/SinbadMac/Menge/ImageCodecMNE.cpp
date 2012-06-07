#	include "ImageCodecMNE.h"

#	include "Codec.h"
#	include "LogEngine.h"

namespace Menge
{
	//////////////////////////////////////////////////////////////////////////
	ImageCodecMNE::ImageCodecMNE()
	{
	}
	//////////////////////////////////////////////////////////////////////////
	ImageCodecMNE::~ImageCodecMNE()
	{
	}
	//////////////////////////////////////////////////////////////////////////
	bool ImageCodecMNE::code( OutStreamInterface* _outStream, unsigned char* _buffer, CodecData* _data ) const
	{
		return true;
	}
	//////////////////////////////////////////////////////////////////////////
	bool ImageCodecMNE::getDataInfo( DataStreamInterface* _inputData, CodecData* _codecData ) const
	{
		ImageCodec::ImageData* imageData = static_cast<ImageCodec::ImageData*>( _codecData );

		unsigned long is_alpha = 0;
		_inputData->read(&is_alpha,sizeof(unsigned long));

		unsigned long jpg_size = 0;
		_inputData->read(&jpg_size,sizeof(unsigned long));

		if( is_alpha != 0 )
		{
			imageData->flags |= DF_COUNT_ALPHA;
		}

		CodecInterface* codecJPEG = CodecManager::getCodec( "jpg" );
		bool res = codecJPEG->getDataInfo( _inputData, imageData );
		if( res == false )
		{
			MENGE_LOG_ERROR( "MNE codec error: can't get file info" );
			return false;
		}

		return true;
	}
	//////////////////////////////////////////////////////////////////////////
	bool ImageCodecMNE::decode( DataStreamInterface* _input, unsigned char* _buffer, unsigned int _options ) const
	{
		unsigned long is_alpha = 0;
		_input->read(&is_alpha,sizeof(unsigned long));

		unsigned long jpg_size = 0;
		_input->read(&jpg_size,sizeof(unsigned long));

		CodecInterface* codecJPEG = CodecManager::getCodec( "jpg" );
		bool res = codecJPEG->decode( _input, _buffer, _options );
		if( res == false )
		{
			MENGE_LOG_ERROR( "MNE codec error: can't decode JPEG info" );
			return false;
		}

		if( is_alpha != 0 )
		{
			_input->seek( jpg_size+3*sizeof(unsigned long) );
			CodecInterface* codecPNG = CodecManager::getCodec( "png" );
			res = codecPNG->decode( _input, _buffer, DF_READ_ALPHA_ONLY );
			if( res == false )
			{
				MENGE_LOG_ERROR( "MNE codec error: can't decode PNG info" );
				return false;
			}
		}

		return true;
	}
	//////////////////////////////////////////////////////////////////////////
	bool ImageCodecMNE::start( DataStreamInterface* _inputData, CodecData* _codecData )
	{
		// streaming not implemented
		return false;
	}
	//////////////////////////////////////////////////////////////////////////
	std::streamsize ImageCodecMNE::read( void* _buf, std::streamsize _count )
	{
		// streaming not implemented
		return 0;
	}
	//////////////////////////////////////////////////////////////////////////
	bool ImageCodecMNE::eof() const
	{
		// streaming not implemented
		return false;
	}
	//////////////////////////////////////////////////////////////////////////
	int ImageCodecMNE::sync( float _timing )
	{
		return 0;
	}
	//////////////////////////////////////////////////////////////////////////
	void ImageCodecMNE::finish()
	{
	}
	//////////////////////////////////////////////////////////////////////////
};
