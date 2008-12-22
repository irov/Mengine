#	pragma once

#	include "Config/Typedef.h"

#	include <map>
#	include "FileEngine.h"

#	define MENGE_DEFAULT_DECODER_POOL_SIZE 8

#	define MENGE_REGISTER_DECODER( _typename_, _class_, _type_ )\
	Menge::CodecManager<_typename_>::registerDecoder( _type_, _class_::creatorNew, _class_::creatorPlacementNew );

namespace Menge 
{
	class DecoderInterface;
	class DataStreamInterface;

	typedef DecoderInterface* (*TDecoderCreatorNew)( DataStreamInterface* _stream, const String& _type );
	typedef DecoderInterface* (*TDecoderCreatorPlacementNew)( void* _mem, DataStreamInterface* _stream, const String& _type );

	template< class T >
	class CodecManager
	{
	public:
		
		static void cleanup()
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


		static void registerDecoder( const String& _type
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

		static void unregisterDecoder( const String& _type )
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

		static T* createDecoder( const String& _filename )
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

				return static_cast<T*>( decoderInterface );
			}

			return NULL;
		}

		static void releaseDecoder( DecoderInterface* _decoder )
		{
			//_codec->release();
			Holder<FileEngine>::hostage()
				->closeStream( _decoder->getStream() );
			const String& type = _decoder->getType();
			ms_decoderPool[ type ].push_back( _decoder );
			_decoder->destructor();
		}

	protected:
		typedef std::map< String, std::pair<TDecoderCreatorNew, TDecoderCreatorPlacementNew> > TDecoderFactory;
		static TDecoderFactory ms_decoderFactory;
		
		typedef std::map< String, std::vector< DecoderInterface* > > TDecoderPool;
		static TDecoderPool ms_decoderPool;
	};

	template< class T >
	typename CodecManager<T>::TDecoderFactory CodecManager<T>::ms_decoderFactory;
	template< class T >
	typename CodecManager<T>::TDecoderPool CodecManager<T>::ms_decoderPool;

} // namespace Menge
