#	include "Movie.h"

#	include "ResourceManager.h"
#	include "ResourceMovie.h"

#	include "ResourceImageDefault.h"
#	include "Sprite.h"

#	include "NodeManager.h"

#	include "Logger/Logger.h"

#	include "Consts.h"

#	include "BinParser.h"

//#	include "pybind/system.hpp"

namespace Menge
{
	//////////////////////////////////////////////////////////////////////////
	namespace Helper
	{
		//////////////////////////////////////////////////////////////////////////
		static void s_applyFrame( Node * _node, const MovieFrame & _frame )
		{
			_node->setOrigin( _frame.anchorPoint );
			_node->setLocalPosition( _frame.position );
			_node->setScale( _frame.scale );
			_node->setAngle( _frame.angle );
			_node->setLocalAlpha( _frame.opacity );
		}
	}
	//////////////////////////////////////////////////////////////////////////
	Movie::Movie()
		: m_timing(0.f)
		, m_out(0.f)
		, m_play(false)
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
	void Movie::play()
	{
		if( isActivate() == false )
		{
			return;
		}

		m_play = true;

		m_timing = 0.f;

		this->setFirstFrame();
	}
	//////////////////////////////////////////////////////////////////////////
	void Movie::stop()
	{
		m_play = false;

		m_timing = 0.f;

		this->callEventDeferred( EVENT_MOVIE_END, "(OO)", this->getEmbed(), pybind::ret_bool(false) );
	}
	//////////////////////////////////////////////////////////////////////////
	void Movie::setFirstFrame()
	{
		std::size_t layerSize = m_resourceMovie->getLayerSize();

		for( std::size_t i = 0; i != layerSize; ++i )
		{
			const MovieLayer & layer = m_resourceMovie->getLayer( i );

			Node * node = m_nodies[layer.index];

			MovieFrame frame;
			if( m_resourceMovie->getFrameFirst( layer, frame ) == false )
			{
				MENGE_LOG_ERROR("Movie: '%s' frame first incorect '%s'"
					, m_name.c_str()
					, layer.name.c_str()
					);

				return;
			}

			Helper::s_applyFrame( node, frame );
		}
	}
	//////////////////////////////////////////////////////////////////////////
	void Movie::setLastFrame()
	{
		std::size_t layerSize = m_resourceMovie->getLayerSize();

		for( std::size_t i = 0; i != layerSize; ++i )
		{
			const MovieLayer & layer = m_resourceMovie->getLayer( i );

			Node * node = m_nodies[layer.index];

			MovieFrame frame;
			if( m_resourceMovie->getFrameLast( layer, frame ) == false )
			{
				MENGE_LOG_ERROR("Movie: '%s' frame last incorect '%s'"
					, m_name.c_str()
					, layer.name.c_str()
					);

				return;
			}

			Helper::s_applyFrame( node, frame );
		}
	}
	//////////////////////////////////////////////////////////////////////////
	void Movie::loader( BinParser * _parser )
	{
		Node::loader(_parser);

		bool depricated_autoplay;
		BIN_SWITCH_ID(_parser)
		{
			BIN_CASE_ATTRIBUTE_METHOD( Protocol::Movie_Name, &Movie::setResourceMovie );
			BIN_CASE_ATTRIBUTE( Protocol::AutoPlay_Value, depricated_autoplay ); //depricated
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

			if( m_out < layer.out )
			{
				m_out = layer.out;
			}

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

				m_nodies[layer.index] = layer_sprite;
			}
			else
			{
				MovieInternal il;
				if( m_resourceMovie->getMovieInternal( layer.source, il ) == false )
				{
					MENGE_LOG_ERROR("Movie: '%s' can't find internal '%s'"
						, m_name.c_str()
						, layer.source.c_str()
						);

					return false;
				}

				Scriptable * scriptable = 0;
				this->askEvent(scriptable, EVENT_MOVIE_FIND_INTERNAL_NODE, "(ss)", layer.source.c_str(), il.group.c_str() );

				if( scriptable == 0 )
				{
					MENGE_LOG_ERROR("Movie: '%s' can't find internal sprite '%s':'%s'"
						, m_name.c_str()
						, layer.source.c_str()
						, il.group.c_str()
						);

					return false;
				}

				Node * layer_node = dynamic_cast<Node*>(scriptable);

				if( layer_node == 0 )
				{
					MENGE_LOG_ERROR("Movie: '%s' internal node not type 'Node' - '%s':'%s'"
						, m_name.c_str()
						, layer.source.c_str()
						, il.group.c_str()
						);

					return false;
				}


				m_nodies[layer.index] = layer_node;
			}
		}

		for( std::size_t i = 0; i != layerSize; ++i )
		{
			const MovieLayer & layer = m_resourceMovie->getLayer( i );

			Node * node = m_nodies[layer.index];

			if( layer.parent == 0 )
			{
				if( layer.internal == false )
				{
					this->addChildren( node );
				}
			}
			else
			{
				Node * node_parent = m_nodies[layer.parent];

				node_parent->addChildren( node );
			}
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

		return true;
	}
	//////////////////////////////////////////////////////////////////////////
	void Movie::_setEventListener( PyObject * _embed )
	{
		Node::_setEventListener(_embed);

		Eventable::registerEvent( EVENT_MOVIE_FIND_INTERNAL_NODE, ("onMovieFindInternalSprite"), _embed );
		Eventable::registerEvent( EVENT_MOVIE_END, ("onMovieEnd"), _embed );
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
			Node * node = m_nodies[layer.index];

			if( layer.internal == false )
			{
				if( layer.in >= lastTiming && layer.in < m_timing )
				{
					this->activateLayer_( layer.index );
				}
			}

			if( layer.out < lastTiming )
			{
				continue;
			}

			MovieFrame frame;
			if( layer.out >= lastTiming && layer.out < m_timing )
			{
				if( m_resourceMovie->getFrameLast( layer, frame ) == false )
				{
					MENGE_LOG_ERROR("Movie: '%s' frame last incorect '%s'"
						, m_name.c_str()
						, layer.name.c_str()
						);

					continue;
				}

				if( layer.internal == false )
				{
					node->disable();
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

			Helper::s_applyFrame( node, frame );
		}

		if( m_out >= lastTiming && m_out < m_timing )
		{
			this->movieEnd_();
		}
	}
	//////////////////////////////////////////////////////////////////////////
	void Movie::movieEnd_()
	{
		m_play = false;

		this->callEventDeferred( EVENT_MOVIE_END, "(OO)", this->getEmbed(), pybind::ret_bool(true) );
	}
	//////////////////////////////////////////////////////////////////////////
	void Movie::activateLayer_( std::size_t _index ) const
	{
		TMapNode::const_iterator it_found = m_nodies.find(_index);

		if( it_found == m_nodies.end() )
		{
			return;
		}

		it_found->second->enable();
	}
}