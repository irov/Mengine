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
		static void s_applyFrame2D( Node * _node, const MovieFrame2D & _frame )
		{
			_node->setOrigin( _frame.anchorPoint );
			_node->setLocalPosition( _frame.position );
			_node->setScale( _frame.scale );
			_node->setAngle( _frame.angle );
			_node->setLocalAlpha( _frame.opacity );
		}
		//////////////////////////////////////////////////////////////////////////
		static void s_applyFrame3D( Sprite * _sprite, const MovieFrame3D & _frame )
		{
			_sprite->setVertices( _frame.vertices );
		}
	}
	//////////////////////////////////////////////////////////////////////////
	Movie::Movie()
		: m_timing(0.f)
		, m_out(0.f)
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
	bool Movie::_play()
	{
		if( isActivate() == false )
		{
			return false;
		}

		m_timing = 0.f;

		//this->updateParent_();

		this->setFirstFrame();

		return true;
	}
	//////////////////////////////////////////////////////////////////////////
	void Movie::_stop( std::size_t _enumerator )
	{
		m_timing = 0.f;

		this->callEventDeferred( EVENT_MOVIE_END, "(OiO)", this->getEmbed(), _enumerator, pybind::ret_bool(false) );
	}
	//////////////////////////////////////////////////////////////////////////
	void Movie::_end( std::size_t _enumerator )
	{
		m_timing = 0.f;

		this->callEventDeferred( EVENT_MOVIE_END, "(OiO)", this->getEmbed(), _enumerator, pybind::ret_bool(true) );
	}
	//////////////////////////////////////////////////////////////////////////
	void Movie::setFirstFrame()
	{
		if( this->isCompile() == false )
		{
			return;
		}

		const TVectorMovieLayers2D & layers2D = m_resourceMovie->getLayers2D();

		for( TVectorMovieLayers2D::const_iterator
			it = layers2D.begin(),
			it_end = layers2D.end();
		it != it_end;
		++it )
		{
			const MovieLayer2D & layer = *it;

			Node * node = m_nodies[layer.index];

			MovieFrame2D frame;
			if( m_resourceMovie->getFrame2DFirst( layer, frame ) == false )
			{
				MENGE_LOG_ERROR("Movie: '%s' frame first incorect '%s'"
					, m_name.c_str()
					, layer.name.c_str()
					);

				return;
			}

			Helper::s_applyFrame2D( node, frame );
		}

		const TVectorMovieLayers3D & layers3D = m_resourceMovie->getLayers3D();

		for( TVectorMovieLayers3D::const_iterator
			it = layers3D.begin(),
			it_end = layers3D.end();
		it != it_end;
		++it )
		{
			const MovieLayer3D & layer = *it;

			Sprite * sprite = m_flexSprites[layer.index];

			MovieFrame3D frame;
			if( m_resourceMovie->getFrame3DFirst( layer, frame ) == false )
			{
				MENGE_LOG_ERROR("Movie: '%s' frame first incorect '%s'"
					, m_name.c_str()
					, layer.name.c_str()
					);

				return;
			}

			Helper::s_applyFrame3D( sprite, frame );
		}
	}
	//////////////////////////////////////////////////////////////////////////
	void Movie::setLastFrame()
	{
		if( this->isCompile() == false )
		{
			return;
		}

		const TVectorMovieLayers2D & layers2D = m_resourceMovie->getLayers2D();

		for( TVectorMovieLayers2D::const_iterator
			it = layers2D.begin(),
			it_end = layers2D.end();
		it != it_end;
		++it )
		{
			const MovieLayer2D & layer = *it;

			Node * node = m_nodies[layer.index];

			MovieFrame2D frame;
			if( m_resourceMovie->getFrame2DLast( layer, frame ) == false )
			{
				MENGE_LOG_ERROR("Movie: '%s' frame last incorect '%s'"
					, m_name.c_str()
					, layer.name.c_str()
					);

				return;
			}

			Helper::s_applyFrame2D( node, frame );
		}

		const TVectorMovieLayers3D & layers3D = m_resourceMovie->getLayers3D();

		for( TVectorMovieLayers3D::const_iterator
			it = layers3D.begin(),
			it_end = layers3D.end();
		it != it_end;
		++it )
		{
			const MovieLayer3D & layer = *it;

			Sprite * sprite = m_flexSprites[layer.index];

			MovieFrame3D frame;
			if( m_resourceMovie->getFrame3DLast( layer, frame ) == false )
			{
				MENGE_LOG_ERROR("Movie: '%s' frame last incorect '%s'"
					, m_name.c_str()
					, layer.name.c_str()
					);

				return;
			}

			Helper::s_applyFrame3D( sprite, frame );
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

		const TVectorMovieLayers2D & layers2D = m_resourceMovie->getLayers2D();

		for( TVectorMovieLayers2D::const_iterator
			it = layers2D.begin(),
			it_end = layers2D.end();
		it != it_end;
		++it )
		{
			const MovieLayer2D & layer = *it;

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
					MENGE_LOG_ERROR("Movie: '%s' can't find internal node '%s':'%s'"
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

		const TVectorMovieLayers3D & layers3D = m_resourceMovie->getLayers3D();

		for( TVectorMovieLayers3D::const_iterator
			it = layers3D.begin(),
			it_end = layers3D.end();
		it != it_end;
		++it )
		{
			const MovieLayer3D & layer = *it;

			if( m_out < layer.out )
			{
				m_out = layer.out;
			}

			if( layer.internal == false )
			{
				//Sprite * layer_sprite = NodeManager::get()
				//	->createNodeT<Sprite>( layer.name, Consts::get()->c_Sprite, Consts::get()->c_Image );

				//String movieImageResource = "MovieLayerImage";
				//movieImageResource += Helper::to_str(layer.source);

				//ConstString c_movieImageResource(movieImageResource);

				//layer_sprite->setImageResource( c_movieImageResource );

				//layer_sprite->disable();

				//if( layer_sprite->compile() == false )
				//{
				//	MENGE_LOG_ERROR("Movie: '%s' can't compile sprite '%s'"
				//		, m_name.c_str()
				//		, layer.name.c_str()
				//		);

				//	return false;
				//}

				//m_nodies[layer.index] = layer_sprite;
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
				this->askEvent(scriptable, EVENT_MOVIE_FIND_INTERNAL_SPRITE, "(ss)", layer.source.c_str(), il.group.c_str() );

				if( scriptable == 0 )
				{
					MENGE_LOG_ERROR("Movie: '%s' can't find internal node '%s':'%s'"
						, m_name.c_str()
						, layer.source.c_str()
						, il.group.c_str()
						);

					return false;
				}

				Sprite * layer_sprite = dynamic_cast<Sprite*>(scriptable);

				if( layer_sprite == 0 )
				{
					MENGE_LOG_ERROR("Movie: '%s' internal node not type 'Sprite' - '%s':'%s'"
						, m_name.c_str()
						, layer.source.c_str()
						, il.group.c_str()
						);

					return false;
				}

				layer_sprite->setFlexible( true );

				m_flexSprites[layer.index] = layer_sprite;
			}
		}

		this->updateParent_();

		return true;
	}
	//////////////////////////////////////////////////////////////////////////
	void Movie::updateParent_()
	{
		const TVectorMovieLayers2D & layers2D = m_resourceMovie->getLayers2D();

		for( TVectorMovieLayers2D::const_iterator 
			it = layers2D.begin(),
			it_end = layers2D.end();
		it != it_end;
		++it )
		{
			const MovieLayer2D & layer = *it;

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
				TMapNode::iterator it_found = m_nodies.find( layer.parent );

				if( it_found == m_nodies.end() )
				{
					continue;
				}

				Node * node_parent = it_found->second;

				node_parent->addChildren( node );
			}
		}

		const TVectorMovieLayers3D & layers3D = m_resourceMovie->getLayers3D();

		for( TVectorMovieLayers3D::const_iterator 
			it = layers3D.begin(),
			it_end = layers3D.end();
		it != it_end;
		++it )
		{
			const MovieLayer3D & layer = *it;

			Node * node = m_flexSprites[layer.index];

			if( layer.parent == 0 )
			{
				if( layer.internal == false )
				{
					this->addChildren( node );
				}
			}
			else
			{
				TMapFlexSprite::iterator it_found = m_flexSprites.find( layer.parent );

				if( it_found == m_flexSprites.end() )
				{
					continue;
				}

				Node * node_parent = it_found->second;

				node_parent->addChildren( node );
			}
		}
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

		Eventable::registerEvent( EVENT_MOVIE_FIND_INTERNAL_NODE, ("onMovieFindInternalNode"), _embed );
		Eventable::registerEvent( EVENT_MOVIE_FIND_INTERNAL_SPRITE, ("onMovieFindInternalSprite"), _embed );
		Eventable::registerEvent( EVENT_MOVIE_END, ("onMovieEnd"), _embed );
	}
	//////////////////////////////////////////////////////////////////////////
	void Movie::_update( float _timing )
	{
		if( this->isPlay() == false )
		{
			return;
		}

		float lastTiming = m_timing;
		m_timing += _timing * 0.1f;

		const TVectorMovieLayers2D & layers2D = m_resourceMovie->getLayers2D();

		for( TVectorMovieLayers2D::const_iterator
			it = layers2D.begin(),
			it_end = layers2D.end();
		it != it_end;
		++it )
		{
			const MovieLayer2D & layer = *it;

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

			MovieFrame2D frame;
			if( layer.out >= lastTiming && layer.out < m_timing )
			{
				if( m_resourceMovie->getFrame2DLast( layer, frame ) == false )
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
				if( m_resourceMovie->getFrame2D( layer, m_timing, frame ) == false )
				{
					continue;
				}
			}

			Helper::s_applyFrame2D( node, frame );
		}

		const TVectorMovieLayers3D & layers3D = m_resourceMovie->getLayers3D();

		for( TVectorMovieLayers3D::const_iterator
			it = layers3D.begin(),
			it_end = layers3D.end();
		it != it_end;
		++it )
		{
			const MovieLayer3D & layer = *it;

			Sprite * sprite = m_flexSprites[layer.index];

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

			MovieFrame3D frame;
			if( layer.out >= lastTiming && layer.out < m_timing )
			{
				if( m_resourceMovie->getFrame3DLast( layer, frame ) == false )
				{
					MENGE_LOG_ERROR("Movie: '%s' frame last incorect '%s'"
						, m_name.c_str()
						, layer.name.c_str()
						);

					continue;
				}

				if( layer.internal == false )
				{
					sprite->disable();
					continue;
				}
			}
			else
			{
				if( m_resourceMovie->getFrame3D( layer, m_timing, frame ) == false )
				{
					continue;
				}
			}

			Helper::s_applyFrame3D( sprite, frame );
		}

		if( m_out >= lastTiming && m_out < m_timing )
		{
			this->end();
		}
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