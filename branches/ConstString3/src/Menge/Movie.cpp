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
	namespace Helper
	{
		//////////////////////////////////////////////////////////////////////////
		static void s_applyFrame( Sprite * _sprite, const MovieLayer::Frame & _frame )
		{
			_sprite->setOrigin( _frame.anchorPoint );
			_sprite->setLocalPosition( _frame.position );
			_sprite->setScale( _frame.scale );
			_sprite->setAngle( _frame.angle );
			_sprite->setLocalAlpha( _frame.opacity );
		}
	}
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

		m_sprites.resize( layerSize );

		for( std::size_t i = 0; i != layerSize; ++i )
		{
			const MovieLayer & layer = m_resourceMovie->getLayer( i );

			if( layer.internal == false )
			{
				Sprite * layer_sprite = NodeManager::get()
					->createNodeT<Sprite>( layer.name, Consts::get()->c_Sprite, Consts::get()->c_Image );

				String movieImageResource = "MovieLayerImage";
				movieImageResource += Helper::to_str(layer.source);

				ConstString c_movieImageResource(movieImageResource);

				layer_sprite->setImageResource( c_movieImageResource );

				layer_sprite->disable();

				if( layer_sprite->compile() == false )
				{
					MENGE_LOG_ERROR("Movie: '%s' can't compile sprite '%s'"
						, m_name.c_str()
						, layer.name.c_str()
						);

					return false;
				}

				m_sprites[ layer.index - 1] = layer_sprite;
			}
			else
			{
				MovieInternal il;
				if( m_resourceMovie->getMovieInternal( layer.source, il ) == false )
				{
					return false;
				}

				Scriptable * scriptable = 0;
				this->askEvent(scriptable, EVENT_MOVIE_FIND_INTERNAL_SPRITE, "(ss)", layer.source.c_str(), il.group.c_str() );

				if( scriptable == 0 )
				{
					MENGE_LOG_ERROR("Movie: '%s' can't find internal sprite '%s':'%s'"
						, m_name.c_str()
						, layer.source.c_str()
						, il.group.c_str()
						);

					return false;
				}

				Sprite * layer_sprite = dynamic_cast<Sprite*>(scriptable);

				if( layer_sprite == 0 )
				{
					MENGE_LOG_ERROR("Movie: '%s' internal sprite not type 'Sprite' - '%s':'%s'"
						, m_name.c_str()
						, layer.source.c_str()
						, il.group.c_str()
						);

					return false;
				}

				m_sprites[ layer.index - 1] = layer_sprite;
			}
		}

		for( std::size_t i = 0; i != layerSize; ++i )
		{
			const MovieLayer & layer = m_resourceMovie->getLayer( i );

			Sprite * sprite = m_sprites[ layer.index - 1 ];

			if( layer.parent == 0 )
			{
				if( layer.internal == false )
				{
					this->addChildren( sprite );
				}
			}
			else
			{
				Sprite * sprite_parent = m_sprites[ layer.parent - 1 ];

				sprite_parent->addChildren( sprite );
			}

			MovieLayer::Frame frame;
			if( m_resourceMovie->getFrameFirst( layer, frame ) == false )
			{
				MENGE_LOG_ERROR("Movie: '%s' frame line incorect '%s'"
					, m_name.c_str()
					, layer.name.c_str()
					);

				return false;
			}

			Helper::s_applyFrame( sprite, frame );
		}

		MENGE_LOG("Movie: compile");

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
	void Movie::_setEventListener( PyObject * _embed )
	{
		Node::_setEventListener(_embed);

		Eventable::registerEvent( EVENT_MOVIE_FIND_INTERNAL_SPRITE, ("onMovieFindInternalSprite"), _embed );
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
		m_timing += _timing;

		for( std::size_t i = 0; i != layerSize; ++i )
		{
			const MovieLayer & layer = m_resourceMovie->getLayer(i);
			Sprite * sprite = m_sprites[ layer.index - 1 ];

			if( layer.internal == false )
			{
				if( layer.in >= lastTiming && layer.in < m_timing )
				{
					this->activateLayer_( i );
				}
			}

			if( layer.out < lastTiming )
			{
				continue;
			}

			MovieLayer::Frame frame;
			if( layer.out >= lastTiming && layer.out < m_timing )
			{
				if( layer.internal == true )
				{
					if( m_resourceMovie->getFrameLast( layer, frame ) == false )
					{
						continue;
					}

					continue;
				}
				else
				{
					sprite->disable();
					continue;
				}
			}
			else
			{
				if( m_resourceMovie->getFrame( layer, m_timing, frame ) == false )
				{
					continue;
				}
			}

			Helper::s_applyFrame( sprite, frame );
		}
	}
	//////////////////////////////////////////////////////////////////////////
	void Movie::activateLayer_( int _index ) const
	{
		Sprite * sprite = m_sprites[_index];

		sprite->enable();
	}
}