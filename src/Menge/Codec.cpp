#	include "Codec.h"

#	include "Interface/ImageCodecInterface.h"

#	include "FileEngine.h"

#	define MENGE_DEFAULT_DECODER_POOL_SIZE 8
#	define MENGE_DEFAULT_ENCODER_POOL_SIZE 8

namespace Menge 
{
	// Decoder
	DecoderManager::TDecoderFactory DecoderManager::ms_decoderFactory;
	DecoderManager::TDecoderPool DecoderManager::ms_decoderPool;
	// Encoder
	EncoderManager::TEncoderFactory EncoderManager::ms_encoderFactory;
	EncoderManager::TEncoderPool EncoderManager::ms_encoderPool;

	void DecoderManager::cleanup()
	{
		// Decoders
		for( TDecoderPool::iterator 
			it = ms_decoderPool.begin(), 
			it_end = ms_decoderPool.end();
		it != it_end;
		it++ )
		{
			TDecoderBuff & pool = it->second;

			for( TDecoderBuff::iterator 
				it_buff = pool.begin(),
				it_buff_end = pool.end(); 
			it_buff != it_buff_end;
			++it_buff )
			{
				DecoderInterface* decoder = *it_buff;
				decoder = ms_decoderFactory[ it->first ].second( decoder, NULL, "" );
				decoder->release();
			}
			pool.clear();
		}
		ms_decoderPool.clear();
		ms_decoderFactory.clear();
	}


	void DecoderManager::registerDecoder( const String& _type
		, TDecoderCreatorNew _creatorNew
		, TDecoderCreatorPlacementNew _creatorPlacementNew )
	{
		TDecoderFactory::iterator it = ms_decoderFactory.find( _type );

		if(it != ms_decoderFactory.end())
		{
			return;
		}

		ms_decoderFactory.insert( std::make_pair( _type, std::make_pair( _creatorNew, _creatorPlacementNew ) ) );

		TDecoderBuff pool;
		pool.reserve( MENGE_DEFAULT_DECODER_POOL_SIZE );
		ms_decoderPool.insert( std::make_pair( _type, pool ) );
	}

	void DecoderManager::unregisterDecoder( const String& _type )
	{
		TDecoderFactory::iterator it_find = ms_decoderFactory.find( _type );
	
		if( it_find == ms_decoderFactory.end() )
		{
			return;
		}

		TDecoderBuff & pool = ms_decoderPool[ _type ];

		for( TDecoderBuff::iterator 
			it_buff = pool.begin(),
			it_buff_end = pool.end(); 
		it_buff != it_buff_end;
		++it_buff )
		{
			DecoderInterface* decoder = *it_buff;
			decoder = ms_decoderFactory[ _type ].second( decoder, NULL, "" );
			decoder->release();
		}

		pool.clear();

		ms_decoderPool.erase( _type );
		ms_decoderFactory.erase( _type );
	}

	DecoderInterface * DecoderManager::createDecoder( const String& _filename, const String& _type )
	{
		String strExt;

		std::size_t pos = _filename.find_last_of( "." );

		while( pos != _filename.length() - 1 )
		{
			strExt += _filename[++pos];
		}

		DataStreamInterface* stream = Holder<FileEngine>::hostage()
			->openFile( _filename );

		TDecoderFactory::iterator it_factory = ms_decoderFactory.find( strExt + _type );

		if(it_factory == ms_decoderFactory.end() ||  stream == NULL )
		{
			return NULL;
		}

		TDecoderBuff & pool = ms_decoderPool[ strExt ];
		DecoderInterface* decoderInterface = NULL;
		if( pool.empty() == false )
		{
			decoderInterface = pool.back();
			decoderInterface = it_factory->second.second( decoderInterface, stream, it_factory->first );
			pool.pop_back();
		}
		else
		{
			decoderInterface = it_factory->second.first( stream, it_factory->first );
		}

		if( decoderInterface->getCodecDataInfo() == NULL )
		{
			releaseDecoder( decoderInterface );
			decoderInterface = NULL;
		}

		return decoderInterface;		
	}

	void DecoderManager::releaseDecoder( DecoderInterface* _decoder )
	{
		//_codec->release();
		Holder<FileEngine>::hostage()
			->closeStream( _decoder->getStream() );

		const String& type = _decoder->getType();

		ms_decoderPool[ type ].push_back( _decoder );

		_decoder->destructor();
	}

	void EncoderManager::cleanup()
	{
		for( TEncoderPool::iterator 
			it = ms_encoderPool.begin(), 
			it_end = ms_encoderPool.end();
		it != it_end;
		it++ )
		{
			TEncoderBuff & pool = it->second;

			for( TEncoderBuff::iterator 
				it_buff = pool.begin(),
				it_buff_end = pool.end();
			it_buff != it_buff_end;
			++it_buff )
			{
				EncoderInterface* encoder = *it_buff;
				encoder = ms_encoderFactory[ it->first ].second( encoder, NULL, "" );
				encoder->release();
			}
			pool.clear();
		}
		ms_encoderPool.clear();
		ms_encoderFactory.clear();
	}

	void EncoderManager::registerEncoder( const String& _type
		, TEncoderCreatorNew _creatorNew
		, TEncoderCreatorPlacementNew _creatorPlacementNew )
	{
		TEncoderFactory::iterator it = ms_encoderFactory.find( _type );

		if(it != ms_encoderFactory.end())
		{
			return;
		}

		ms_encoderFactory.insert( std::make_pair( _type, std::make_pair( _creatorNew, _creatorPlacementNew ) ) );
		TEncoderBuff pool;
		pool.reserve( MENGE_DEFAULT_ENCODER_POOL_SIZE );
		ms_encoderPool.insert( std::make_pair( _type, pool ) );
	}

	void EncoderManager::unregisterEncoder( const String& _type )
	{
		TEncoderFactory::iterator it_find = ms_encoderFactory.find( _type );
		if( it_find == ms_encoderFactory.end() )
		{
			return;
		}

		TEncoderBuff & pool = ms_encoderPool[ _type ];
		for( TEncoderBuff::iterator 
			it_buff = pool.begin(),
			it_buff_end = pool.end(); 
		it_buff != it_buff_end;
		++it_buff )
		{
			EncoderInterface* encoder = *it_buff;
			encoder = ms_encoderFactory[ _type ].second( encoder, NULL, "" );
			encoder->release();
		}
		pool.clear();

		ms_encoderPool.erase( _type );
		ms_encoderFactory.erase( _type );
	}

	EncoderInterface * EncoderManager::createEncoder( const String& _filename, const String& _type )
	{
		String strExt;

		std::size_t pos = _filename.find_last_of( "." );

		while( pos != _filename.length() - 1 )
		{
			strExt += _filename[++pos];
		}

		OutStreamInterface* stream = Holder<FileEngine>::hostage()
			->openOutStream( _filename, true );

		TEncoderFactory::iterator it_factory = ms_encoderFactory.find( strExt + _type );

		if(it_factory == ms_encoderFactory.end() ||  stream == NULL )
		{
			return NULL;
		}

		TEncoderBuff & pool = ms_encoderPool[ strExt ];
		EncoderInterface* encoderInterface = NULL;
		if( pool.empty() == false )
		{
			encoderInterface = pool.back();
			encoderInterface = it_factory->second.second( encoderInterface, stream, it_factory->first );
			pool.pop_back();
		}
		else
		{
			encoderInterface = it_factory->second.first( stream, it_factory->first );
		}

		return encoderInterface;
	}

	void EncoderManager::releaseEncoder( EncoderInterface* _encoder )
	{
		//_codec->release();
		Holder<FileEngine>::hostage()
			->closeOutStream( _encoder->getStream() );

		const String& type = _encoder->getType();

		ms_encoderPool[ type ].push_back( _encoder );

		_encoder->destructor();
	}
} // namespace Menge
