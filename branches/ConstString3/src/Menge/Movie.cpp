#	include "Movie.h"

#	include "ResourceManager.h"
#	include "ResourceMovie.h"

#	include "ResourceImageDefault.h"
#	include "Sprite.h"

#	include "NodeManager.h"

#	include "Logger/Logger.h"

#	include "Consts.h"

#	include "BinParser.h"

namespace Menge
{
	//////////////////////////////////////////////////////////////////////////
	Movie::Movie()
		: m_timing(0.f)
		, m_play(false)
		, m_autoPlay(false)
	{
	}
	//////////////////////////////////////////////////////////////////////////
	void Movie::setResourceMovie( const ConstString & _resourceName )
	{
		m_resourceMovieName = _resourceName;
	}
	//////////////////////////////////////////////////////////////////////////
	const ConstString & Movie::getResourceMovie() const
	{
		return m_resourceMovieName;
	}
	//////////////////////////////////////////////////////////////////////////
	void Movie::setAutoPlay( bool _value )
	{
		m_autoPlay = _value;
	}
	//////////////////////////////////////////////////////////////////////////
	void Movie::play()
	{
		m_play = true;
		m_timing = 0.f;

		if( this->compile() == false )
		{
			return;
		}
	}
	//////////////////////////////////////////////////////////////////////////
	void Movie::stop()
	{
		m_play = false;
	}
	//////////////////////////////////////////////////////////////////////////
	void Movie::loader( BinParser * _parser )
	{
		Node::loader(_parser);

		BIN_SWITCH_ID(_parser)
		{
			BIN_CASE_ATTRIBUTE_METHOD( Protocol::Movie_Name, &Movie::setResourceMovie );
			BIN_CASE_ATTRIBUTE_METHOD( Protocol::AutoPlay_Value, &Movie::setAutoPlay );
		}
	}
	//////////////////////////////////////////////////////////////////////////
	bool Movie::_compile()
	{
		if( Node::_compile() == false )
		{
			return false;
		}

		m_play = false;
		m_timing = 0.f;

		m_resourceMovie = ResourceManager::get()
			->getResourceT<ResourceMovie>( m_resourceMovieName );

		if( m_resourceMovie == 0 )
		{
			MENGE_LOG_ERROR("Movie can't setup resource '%s'"
				, m_resourceMovieName.c_str()
				);

			return false;
		}

		std::size_t layerSize = m_resourceMovie->getLayerSize();

		for( std::size_t i = 0; i != layerSize; ++i )
		{
			const MovieLayer & layer = m_resourceMovie->getLayer( i );

			//String movieImageResource = "MovieLayerImage";
			//movieImageResource += layer.name;

			//if( ResourceManager::get()->hasResource( movieImageResource ) == false )
			//{
			//	ResourceImageDefault * layer_resource = ResourceManager::get()
			//		->createResourceT<ResourceImageDefault>( movieImageResource, Consts::get()->c_ResourceImageDefault );

			//	ResourceManager::get()
			//		->registerResource( layer_resource );

			//	layer_resource->addImagePath( "Movies/" + layer.path, mt::vec2f(-1.f,-1.f) );
			//}

			//Sprite * layer_sprite = NodeManager::get()
			//	->createNodeT<Sprite>( layer.name, Consts::get()->c_Sprite, "Image" );

			//layer_sprite->setImageResource( movieImageResource );
			//
			//layer_sprite->disable();

			//if( layer_sprite->compile() == false )
			//{
			//	return false;
			//}

			//this->addChildren( layer_sprite );

			//m_sprites.push_back( layer_sprite );
		}

		return true;
	}
	//////////////////////////////////////////////////////////////////////////
	void Movie::_release()
	{
		ResourceManager::get()
			->releaseResource( m_resourceMovie );

		m_resourceMovie = 0;
	}
	//////////////////////////////////////////////////////////////////////////
	bool Movie::_activate()
	{
		if( Node::_activate() == false )
		{
			return false;
		}

		if( m_autoPlay == true )
		{
			this->play();
		}

		return true;
	}
	//////////////////////////////////////////////////////////////////////////
	namespace Helper
	{
		template<class C, class T>
		static bool s_getKeyFrameValue( C & _c, float _begin, float _end, T & _value )
		{
			for( typename C::size_type 
				it = 0,
				it_end = _c.size();
			it != it_end; )
			{
				switch( it->interp )
				{
				case 0:
					{
						if( it->time >= m_timming && it->time < m_timming + _timing )
						{
							sprite->setOrigin( it->value );	
						}
					} break;
				case 1:
					{
						if( it->time >= m_timming && it->time < m_timming + _timing )
						{

						}
					}
				}
			}
		}
	}
	//////////////////////////////////////////////////////////////////////////
	void Movie::_update( float _timing )
	{
		if( m_play == false )
		{
			return;
		}

		std::size_t layerSize = m_resourceMovie->getLayerSize();

		float lastTiming = m_timing;
		m_timing += _timing * 0.25f;

		for( std::size_t i = 0; i != layerSize; ++i )
		{
			const MovieLayer & layer = m_resourceMovie->getLayer(i);
			Sprite * sprite = m_sprites[i];

			if( layer.in >= lastTiming && layer.in <= m_timing )
			{
				this->activateLayer_( i );
			}

			if( layer.out < lastTiming || layer.out < m_timing )
			{
				sprite->disable();
				continue;
			}
		}
	}
	//////////////////////////////////////////////////////////////////////////
	void Movie::activateLayer_( int _index )
	{
		const MovieLayer & layer = m_resourceMovie->getLayer(_index);
		Sprite * sprite = m_sprites[_index];

		sprite->enable();
	}
}