/*#	include "Codec.h"
#	include "Interface/CodecInterface.h"
#	include "Interface/ImageCodecInterface.h"
#	include "Interface/SoundCodecInterface.h"
#	include "Interface/VideoCodecInterface.h"
#	include "FileEngine.h"
#	include "ImageDecoderPNG.h"*/
//#	include "ImageCodecPNG.h"
//#	include "ImageCodecJPEG.h"
//#	include "ImageCodecMNE.h"
//#	include "CodecTheora.h"

namespace Menge
{
	//////////////////////////////////////////////////////////////////////////
	template< class T >
	typename CodecManager<T>::TDecoderFactory CodecManager<T>::ms_decoderFactory;
	template< class T >
	typename CodecManager<T>::TDecoderPool CodecManager<T>::ms_decoderPool;
	//////////////////////////////////////////////////////////////////////////
	/*void CodecManager::initialize()
	{
		MENGE_REGISTER_DECODER( "png", ImageDecoderPNG );
		//registerCodec( "png", &ImageCodecPNG::creatorNew, &ImageCodecPNG::creatorPlacementNew );
		//registerCodec( "jpg", &ImageCodecJPEG::creator );
		//registerCodec( "jpeg", &ImageCodecJPEG::creator );
		//registerCodec( "jfif", &ImageCodecJPEG::creator );
		//registerCodec( "mne", &ImageCodecMNE::creator );
		//registerCodec( "theora", &ImageCodecTheora::creator );
	}*/
	//////////////////////////////////////////////////////////////////////////
	template< class T >
	void CodecManager<T>::cleanup()
	{
		for( TDecoderPool::iterator it = ms_decoderPool.begin(), it_end = ms_decoderPool.end();
			it != it_end;
			it++ )
		{
			std::vector<DecoderInterface*>& pool = it->second;
			for( std::vector<DecoderInterface*>::size_type i = 0; i < pool.size(); i++ )
			{
				DecoderInterface* decoder = pool[i];
				decoder = ms_decoderFactory[ it->first ].second( decoder, NULL, "" );
				decoder->release();
			}
			pool.clear();
		}
		ms_decoderPool.clear();
		ms_decoderFactory.clear();
	}
	//////////////////////////////////////////////////////////////////////////
	template< class T >
	void CodecManager<T>::registerDecoder( const String& _type
		, TDecoderCreatorNew _creatorNew
		, TDecoderCreatorPlacementNew _creatorPlacementNew )
	{
		TDecoderFactory::iterator it = ms_decoderFactory.find( _type );

		if(it != ms_decoderFactory.end())
		{
			return;
		}

		ms_decoderFactory.insert( std::make_pair( _type, std::make_pair( _creatorNew, _creatorPlacementNew ) ) );
		std::vector<DecoderInterface*> pool;
		pool.reserve( MENGE_DEFAULT_DECODER_POOL_SIZE );
		ms_decoderPool.insert( std::make_pair( _type, pool ) );
	}
	//////////////////////////////////////////////////////////////////////////
	template< class T >
	void CodecManager<T>::unregisterDecoder( const String& _type )
	{
		TDecoderFactory::iterator it_find = ms_decoderFactory.find( _type );
		if( it_find == ms_decoderFactory.end() )
		{
			return;
		}

		std::vector<DecoderInterface*> pool = ms_decoderPool[ _type ];
		for( std::vector<DecoderInterface*>::size_type i = 0; i < pool.size(); i++ )
		{
			DecoderInterface* decoder = pool[i];
			decoder = ms_decoderFactory[ _type ].second( decoder, NULL, "" );
			decoder->release();
		}
		pool.clear();

		ms_decoderPool.erase( _type );
		ms_decoderFactory.erase( _type );
	}
	//////////////////////////////////////////////////////////////////////////
	template< class T >
	T* CodecManager<T>::createDecoder( const String& _filename )
	{
		String strExt;

		std::size_t pos = _filename.find_last_of( "." );

		while( pos != _filename.length() - 1 )
		{
			strExt += _filename[++pos];
		}

		DataStreamInterface* stream = Holder<FileEngine>::hostage()
											->openFile( _filename );

		TDecoderFactory::iterator it_factory = ms_decoderFactory.find( strExt );

		if(it_factory != ms_decoderFactory.end() &&  stream != NULL )
		{
			std::vector<DecoderInterface*>& pool = ms_decoderPool[ strExt ];
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

		return NULL;
	}
	//////////////////////////////////////////////////////////////////////////
	/*ImageDecoderInterface* CodecManager::createImageDecoder( const String& _filename )
	{
		DecoderInterface* decoder = createDecoder( _filename );
		return static_cast<ImageDecoderInterface*>( decoder );
	}
	//////////////////////////////////////////////////////////////////////////
	SoundDecoderInterface* CodecManager::createSoundDecoder( const String& _filename )
	{
		DecoderInterface* decoder = createDecoder( _filename );
		return static_cast<SoundDecoderInterface*>( decoder );
	}
	//////////////////////////////////////////////////////////////////////////
	VideoDecoderInterface* CodecManager::createVideoDecoder( const String& _filename )
	{
		DecoderInterface* decoder = createDecoder( _filename );
		return static_cast<VideoDecoderInterface*>( decoder );
	}*/
	//////////////////////////////////////////////////////////////////////////
	template< class T >
	void CodecManager<T>::releaseDecoder( DecoderInterface* _decoder )
	{
		//_codec->release();
		Holder<FileEngine>::hostage()
			->closeStream( _decoder->getStream() );
		const String& type = _decoder->getType();
		ms_decoderPool[ type ].push_back( _decoder );
		_decoder->destructor();
	}
	//////////////////////////////////////////////////////////////////////////
}