#	include "ImageCodecMNE.h"

#	include "ImageCodecJPEG.h"
#	include "ImageCodecPNG.h"

#	include "LogEngine.h"

namespace Menge
{
	//////////////////////////////////////////////////////////////////////////
	ImageCodecMNE::ImageCodecMNE(const String & _type)
		: m_type(_type)
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

		Codec* codecJPEG = Codec::getCodec( "jpg" );
		bool res = codecJPEG->getDataInfo( _inputData, imageData );
		if( res == false )
		{
			MENGE_LOG( "MNE codec error: can't get file info" );
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

		Codec* codecJPEG = Codec::getCodec( "jpg" );
		bool res = codecJPEG->decode( _input, _buffer, _options );
		if( res == false )
		{
			MENGE_LOG( "MNE codec error: can't decode JPEG info" );
			return false;
		}

		if( is_alpha != 0 )
		{
			_input->seek( jpg_size+3*sizeof(unsigned long) );
			Codec* codecPNG = Codec::getCodec( "png" );
			res = codecPNG->decode( _input, _buffer, DF_READ_ALPHA_ONLY );
			if( res == false )
			{
				MENGE_LOG( "MNE codec error: can't decode PNG info" );
				return false;
			}
		}

		return true;
	}
	//////////////////////////////////////////////////////////////////////////
	const String & ImageCodecMNE::getType() const
	{
		return m_type;
	}
	//////////////////////////////////////////////////////////////////////////
};
