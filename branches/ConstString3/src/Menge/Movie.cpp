#	include "Movie.h"

#	include "ResourceManager.h"
#	include "ResourceMovie.h"

#	include "ResourceImageDefault.h"
#	include "ResourceInternalObject.h"
#	include "ResourceAnimation.h"

#	include "Sprite.h"
#	include "Animation.h"
#	include "Video.h"
#	include "ParticleEmitter.h"

#	include "SoundEmitter.h"
#	include "NodeManager.h"

#	include "LogEngine.h"

#	include "Consts.h"

#	include "BinParser.h"

#	include "pybind/system.hpp"
#	include "pybind/extract.hpp"

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
			_node->setPersonalAlpha( _frame.opacity );
		}
		//////////////////////////////////////////////////////////////////////////
		static void s_applyFrame2DMovie( Node * _node, const MovieFrame2D & _frame )
		{
			_node->setOrigin( _frame.anchorPoint );
			_node->setLocalPosition( _frame.position );
			_node->setScale( _frame.scale );
			_node->setAngle( _frame.angle );
			_node->setLocalAlpha( _frame.opacity );
		}
		//////////////////////////////////////////////////////////////////////////
		static void s_applyRelationFrame2D( Node * _node, const MovieFrame2D & _frame, const mt::mat3f & _wm  )
		{
			_node->setOrigin( _frame.anchorPoint );

			mt::vec2f wm_pos;
			mt::mul_v2_m3(wm_pos, _frame.position, _wm);

			_node->setLocalPosition( wm_pos );
			_node->setScale( _frame.scale );
			_node->setAngle( _frame.angle );
			_node->setPersonalAlpha( _frame.opacity );
		}
		//////////////////////////////////////////////////////////////////////////
		static void s_applyFrame3D( Sprite * _sprite, const MovieFrame3D & _frame )
		{
			_sprite->setVertices( _frame.vertices );
		}
	}
	//////////////////////////////////////////////////////////////////////////
	Movie::Movie()
		: m_resourceMovie(NULL)
		, m_timing(0.f)
		, m_out(0.f)
		, m_reverse(false)
		, m_parentMovie(false)		
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
	void Movie::setParentMovie( bool _value )
	{
		m_parentMovie = _value;
	}
	//////////////////////////////////////////////////////////////////////////
	bool Movie::isParentMovie( bool _value ) const
	{
		return m_parentMovie;
	}
	//////////////////////////////////////////////////////////////////////////
	bool Movie::_play()
	{
		if( isActivate() == false )
		{
			return false;
		}
		
		if( m_reverse == true )
		{
			m_timing = m_out;
			this->setLastFrame();
		}
		else
		{
			m_timing = 0.f;
			this->setFirstFrame();
		}
		
		this->updateParent_();
		
		

		return true;
	}
	//////////////////////////////////////////////////////////////////////////
	bool Movie::_restart( size_t _enumerator )
	{
		if( isActivate() == false )
		{
			return false;
		}

		return true;
	}
	//////////////////////////////////////////////////////////////////////////
	void Movie::_stop( size_t _enumerator )
	{
		m_timing = 0.f;

		this->callEventDeferred( EVENT_MOVIE_END, "(OiO)", this->getEmbed(), _enumerator, pybind::get_bool(false) );
	}
	//////////////////////////////////////////////////////////////////////////
	void Movie::_end( size_t _enumerator )
	{
		m_timing = 0.f;

		this->callEventDeferred( EVENT_MOVIE_END, "(OiO)", this->getEmbed(), _enumerator, pybind::get_bool(true) );
	}
	//////////////////////////////////////////////////////////////////////////
	void Movie::updateFrame2D_( const MovieLayer2D & _layer, Node * _node, const MovieFrame2D & _frame, const mt::mat3f & _wm )
	{
		if( _layer.internal == true )
		{
			//_node->setOrigin( _frame.anchorPoint );
			//_node->setLocalPosition( _frame.position );
			//_node->setScale( _frame.scale );
			//_node->setAngle( _frame.angle );
			//_node->setLocalAlpha( _frame.opacity );

			if( _layer.parent == 0 )
			{
				mt::vec2f wm_pos;
				mt::mul_v2_m3(wm_pos, _frame.position, _wm);

				PyObject * py_node = pybind::ptr(_node);
				PyObject * py_anchorPoint = pybind::ptr(_frame.anchorPoint);
				PyObject * py_position = pybind::ptr(wm_pos);
				PyObject * py_scale = pybind::ptr(_frame.scale);
				PyObject * py_angle = pybind::ptr(_frame.angle);
				PyObject * py_opacity = pybind::ptr(_frame.opacity);

				Eventable::callEvent( EVENT_MOVIE_APPLY_INTERNAL_SPRITE, "(OOOOOO)", py_node, py_anchorPoint, py_position, py_scale, py_angle, py_opacity );

				pybind::decref(py_node);
				pybind::decref(py_anchorPoint);
				pybind::decref(py_position);
				pybind::decref(py_scale);
				pybind::decref(py_angle);
				pybind::decref(py_opacity);
			}
			else
			{
				PyObject * py_node = pybind::ptr(_node);
				PyObject * py_anchorPoint = pybind::ptr(_frame.anchorPoint);
				PyObject * py_position = pybind::ptr(_frame.position);
				PyObject * py_scale = pybind::ptr(_frame.scale);
				PyObject * py_angle = pybind::ptr(_frame.angle);
				PyObject * py_opacity = pybind::ptr(_frame.opacity);

				Eventable::callEvent( EVENT_MOVIE_APPLY_INTERNAL_SPRITE, "(OOOOOO)", py_node, py_anchorPoint, py_position, py_scale, py_angle, py_opacity );

				pybind::decref(py_node);
				pybind::decref(py_anchorPoint);
				pybind::decref(py_position);
				pybind::decref(py_scale);
				pybind::decref(py_angle);
				pybind::decref(py_opacity);
			}
		}
		else
		{
			//if( _layer.parent == 0 )
			//{
			//	Helper::s_applyRelationFrame2D( _node, _frame, _wm );
			//}
			//else
			//{
			//	Helper::s_applyFrame2D( _node, _frame );				
			//}

			if( _layer.movie == false )
			{
				Helper::s_applyFrame2D( _node, _frame );			
			}
			else
			{
				Helper::s_applyFrame2DMovie( _node, _frame );
			}
		}
	}
	//////////////////////////////////////////////////////////////////////////
	void Movie::setFirstFrame()
	{
		if( this->isActivate() == false )
		{
			MENGE_LOG_ERROR("Movie %s invalid setFirstFrame: not activate"
				, m_name.c_str()
				);

			return;
		}

		const mt::mat3f & wm = this->getWorldMatrix();

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

			this->updateFrame2D_( layer, node, frame, wm );
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
		if( this->isActivate() == false )
		{
			MENGE_LOG_ERROR("Movie %s invalid setLastFrame: not activate"
				, m_name.c_str()
				);

			return;
		}

		const mt::mat3f & wm = this->getWorldMatrix();

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

			this->updateFrame2D_( layer, node, frame, wm );
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
	Scriptable * Movie::findInternalObject_( const ConstString & _resource, EEventName _event )
	{
		ResourceInternalObject * resourceInternalObject = ResourceManager::get()
			->getResourceT<ResourceInternalObject>( _resource );

		if( resourceInternalObject == NULL )
		{
			MENGE_LOG_ERROR("Movie: '%s' can't find internal resource '%s'"
				, m_name.c_str()
				, _resource.c_str()
				);

			return NULL;
		}

		const ConstString & internalName = resourceInternalObject->getInternalName();
		const ConstString & internalGroup = resourceInternalObject->getInternalGroup();

		resourceInternalObject->decrementReference();

		Scriptable * scriptable = 0;
		this->askEvent( scriptable, _event, "(ss)", internalName.c_str(), internalGroup.c_str() );

		if( scriptable == NULL )
		{
			MENGE_LOG_ERROR("Movie: '%s' can't find internal node '%s:%s'"
				, m_name.c_str()
				, internalName.c_str()
				, internalGroup.c_str()
				);

			return NULL;
		}


		return scriptable;
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

		m_out = m_resourceMovie->getWorkAreaDuration();

		const TVectorMovieLayers2D & layers2D = m_resourceMovie->getLayers2D();

		for( TVectorMovieLayers2D::const_iterator
			it = layers2D.begin(),
			it_end = layers2D.end();
		it != it_end;
		++it )
		{
			const MovieLayer2D & layer = *it;

			const ConstString & resourceType = ResourceManager::get()
				->getResourceType( layer.source );

			if( resourceType == Consts::get()->c_ResourceImageDefault )
			{
				Sprite * layer_sprite = NodeManager::get()
					->createNodeT<Sprite>( layer.name, Consts::get()->c_Sprite, Consts::get()->c_Image );

				layer_sprite->setImageResource( layer.source );
								
				//layer_sprite->disable();

				if( layer_sprite->compile() == false )
				{
					MENGE_LOG_ERROR("Movie: '%s' can't compile sprite '%s'"
						, m_name.c_str()
						, layer.name.c_str()
						);

					return false;
				}

				layer_sprite->enable();
				layer_sprite->localHide(true);

				m_nodies[layer.index] = layer_sprite;
			}
			else if( resourceType == Consts::get()->c_ResourceAnimation )
			{
				Animation * layer_animation = NodeManager::get()
					->createNodeT<Animation>( layer.name, Consts::get()->c_Animation, Consts::get()->c_Image );

				ResourceAnimation * resourceAnimation = ResourceManager::get()
					->getResourceT<ResourceAnimation>( layer.source );

				if( resourceAnimation == NULL )
				{
					MENGE_LOG_ERROR("Movie: '%s' can't get resourceAnimation '%s'"
						, m_name.c_str()
						, layer.source.c_str()
						);

					return false;
				}

				const ConstString & resourceImageName = resourceAnimation->getResourceImageName();
				const ConstString & resourceSequenceName = resourceAnimation->getResourceSequenceName();

				layer_animation->setImageResource( resourceImageName );
				layer_animation->setSequenceResource( resourceSequenceName );

				layer_animation->setLoop( true );				
				//layer_animation->disable();

				if( layer_animation->compile() == false )
				{
					MENGE_LOG_ERROR("Movie: '%s' can't compile animation '%s'"
						, m_name.c_str()
						, layer.name.c_str()
						);

					return false;
				}

				layer_animation->enable();
				layer_animation->localHide(true);

				m_nodies[layer.index] = layer_animation;
			}
			else if( resourceType == Consts::get()->c_ResourceMovie )
			{
				Movie * layer_movie = NodeManager::get()
					->createNodeT<Movie>( layer.name, Consts::get()->c_Movie, Consts::get()->c_Image );

				layer_movie->setResourceMovie( layer.source );				
				layer_movie->setLoop( true );				
				//layer_movie->disable();

				if( layer_movie->compile() == false )
				{
					MENGE_LOG_ERROR("Movie: '%s' can't compile movie '%s'"
						, m_name.c_str()
						, layer.name.c_str()
						);

					return false;
				}

				layer_movie->enable();
				layer_movie->localHide(true);
				layer_movie->setParentMovie(true);

				m_nodies[layer.index] = layer_movie;
			}
			else if( resourceType == Consts::get()->c_ResourceInternalObject )
			{
				
				Scriptable * scriptable = this->findInternalObject_( layer.source, EVENT_MOVIE_FIND_INTERNAL_NODE );

				if( scriptable == NULL )
				{
					return false;
				}				
				
				Node * layer_node = dynamic_cast<Node*>(scriptable);

				if( layer_node == 0 )
				{
					MENGE_LOG_ERROR("Movie: '%s' internal node '%s' not type 'Node'"
						, m_name.c_str()
						, layer.source.c_str()
						);

					return false;
				}

				m_nodies[layer.index] = layer_node;
			}
			else if( resourceType == Consts::get()->c_ResourceVideo )
			{
				Video * layer_video = NodeManager::get()
					->createNodeT<Video>( layer.name, Consts::get()->c_Video, Consts::get()->c_Image );

				layer_video->setVideoResource( layer.source );

				layer_video->setLoop( true );				
				//layer_movie->disable();

				if( layer_video->compile() == false )
				{
					MENGE_LOG_ERROR("Movie: '%s' can't compile video '%s'"
						, m_name.c_str()
						, layer.name.c_str()
						);

					return false;
				}

				layer_video->enable();
				layer_video->localHide(true);

				m_nodies[layer.index] = layer_video;
			}
			else if( resourceType == Consts::get()->c_ResourceSound )
			{
				SoundEmitter * layer_sound = NodeManager::get()
					->createNodeT<SoundEmitter>( layer.name, Consts::get()->c_SoundEmitter, Consts::get()->c_Sound );

				layer_sound->setSoundResource(layer.source );

				layer_sound->setLoop( true );				
				//layer_movie->disable();

				if( layer_sound->compile() == false )
				{
					MENGE_LOG_ERROR("Movie: '%s' can't compile video '%s'"
						, m_name.c_str()
						, layer.name.c_str()
						);

					return false;
				}

				layer_sound->enable();
				layer_sound->localHide(true);

				m_nodies[layer.index] = layer_sound;
			}
			else if( resourceType == Consts::get()->c_ResourceEmitterContainer )
			{
				ParticleEmitter * layer_particles = NodeManager::get()
					->createNodeT<ParticleEmitter>( layer.name, Consts::get()->c_ParticleEmitter, Consts::get()->c_Image );

				layer_particles->setResource( layer.source );
				
				layer_particles->setLoop( true );				
				//layer_movie->disable();
				
				layer_particles->setEmitterRelative(true);
				
				if( layer_particles->compile() == false )
				{
					MENGE_LOG_ERROR("Movie: '%s' can't compile video '%s'"
						, m_name.c_str()
						, layer.name.c_str()
						);

					return false;
				}

				layer_particles->enable();
				layer_particles->localHide(true);

				m_nodies[layer.index] = layer_particles;
			}
			else
			{
				MENGE_LOG_ERROR("Movie: '%s' can't create layer_node '%s' type '%s'"
					, m_name.c_str()
					, layer.source.c_str()
					, resourceType.c_str()
					);

				return false;
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
				Scriptable * scriptable = this->findInternalObject_( layer.source, EVENT_MOVIE_FIND_INTERNAL_SPRITE );
				
				if( scriptable == NULL )
				{
					return false;
				}

				Sprite * layer_sprite = dynamic_cast<Sprite*>(scriptable);

				if( layer_sprite == 0 )
				{
					MENGE_LOG_ERROR("Movie: '%s' internal node '%s' not type 'Sprite'"
						, m_name.c_str()
						, layer.source.c_str()
						);

					return false;
				}

				layer_sprite->setFlexible( true );

				m_flexSprites[layer.index] = layer_sprite;
			}
		}

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

			TMapNode::const_iterator it_node = m_nodies.find( layer.index );

			if( it_node == m_nodies.end() )
			{
				MENGE_LOG_ERROR("Movie: '%s' not found layer '%s' parent '%d'"
					, m_name.c_str()
					, layer.name.c_str()
					, layer.index
					);

				continue;
			}

			Node * node = it_node->second;

			if( layer.parent == 0 )
			{
				if( layer.internal == false )
				{
					this->addChildren( node );
				}
			}
			else
			{
				TMapNode::iterator it_parent = m_nodies.find( layer.parent );

				if( it_parent == m_nodies.end() )
				{
					continue;
				}

				Node * node_parent = it_parent->second;

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
		if( m_resourceMovie != 0 )
		{
			m_resourceMovie->decrementReference();
			m_resourceMovie = 0;
		}
	}
	//////////////////////////////////////////////////////////////////////////
	bool Movie::_activate()
	{
		if( Node::_activate() == false )
		{
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

			if( layer.internal == true )
			{
				ResourceInternalObject * resourceInternalObject = ResourceManager::get()
					->getResourceT<ResourceInternalObject>( layer.source );

				if( resourceInternalObject == NULL )
				{
					MENGE_LOG_ERROR("Movie: '%s' can't find internal resource '%s'"
						, m_name.c_str()
						, layer.source.c_str()
						);

					return NULL;
				}

				const ConstString & internalName = resourceInternalObject->getInternalName();
				const ConstString & internalGroup = resourceInternalObject->getInternalGroup();

				resourceInternalObject->decrementReference();

				this->callEvent( EVENT_MOVIE_PREPARE_INTERNAL_NODE, "(ss)", internalName.c_str(), internalGroup.c_str() );
			}
		}

		this->updateParent_();

		return true;
	}
	//////////////////////////////////////////////////////////////////////////
	void Movie::_setEventListener( PyObject * _embed )
	{
		Node::_setEventListener(_embed);

		Eventable::registerEvent( EVENT_MOVIE_FIND_INTERNAL_NODE, ("onMovieFindInternalNode"), _embed );
		Eventable::registerEvent( EVENT_MOVIE_PREPARE_INTERNAL_NODE, ("onMoviePrepareInternalNode"), _embed );		
		Eventable::registerEvent( EVENT_MOVIE_FIND_INTERNAL_SPRITE, ("onMovieFindInternalSprite"), _embed );
		Eventable::registerEvent( EVENT_MOVIE_APPLY_INTERNAL_SPRITE, ("onMovieApplyInternalSprite"), _embed );
		Eventable::registerEvent( EVENT_MOVIE_END, ("onMovieEnd"), _embed );
	}
	//////////////////////////////////////////////////////////////////////////
	void Movie::_update( float _timing )
	{
		if( this->isPlay() == false )
		{
			return;
		}

		//if( m_parentMovie == false )
		//{
		//	printf("Movie %s update %f:%f\n", m_name.c_str(), m_timing, _timing);
		//}

		float lastTiming = m_timing;

		if( m_reverse == true )
		{
			m_timing -= _timing;
		}
		else
		{
			m_timing += _timing;
		}

		const mt::mat3f & wm = this->getWorldMatrix();
			
		const TVectorMovieLayers2D & layers2D = m_resourceMovie->getLayers2D();

		for( TVectorMovieLayers2D::const_iterator
			it = layers2D.begin(),
			it_end = layers2D.end();
		it != it_end;
		++it )
		{
			const MovieLayer2D & layer = *it;

			float layerIn = layer.in;
			float layerOut = layer.out;

			if( layerOut > m_out )
			{
				layerOut = m_out;
			}

			//if (m_reverse)
			//{
			//	layerIn = layer.out;
			//	layerOut = layer.in;
			//}

			Node * node = m_nodies[layer.index];

			if( layer.internal == false )
			{
				if( layerIn >= lastTiming && layerIn <= m_timing )
				{
					//printf("Movie %s enable %f %d\n", m_name.c_str(), m_timing, layer.index);
					node->localHide(false);

					if( layer.animatable == true )
					{
						Animatable * animatable = dynamic_cast<Animatable *>(node);

						animatable->play();
					}
				}
			}
			
			if( layerOut < lastTiming )
			{
				continue;
			}

			MovieFrame2D frame;
			if( layerOut >= lastTiming && layerOut <= m_timing )
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
					//if( layerIn > 0.001f || fabsf(layerOut - m_out) > 0.001f )
					//{
					//printf("Movie %s disable %f %d\n", m_name.c_str(), m_timing, layer.index);
					node->localHide(true);

					if( layer.animatable == true )
					{
						Animatable * animatable = dynamic_cast<Animatable *>(node);

						animatable->stop();
					}
					//node->disable();
					continue;
					//}
				}
			}
			else
			{
				if( m_resourceMovie->getFrame2D( layer, m_timing, frame ) == false )
				{
					continue;
				}
			}

			this->updateFrame2D_( layer, node, frame, wm );
		}

		const TVectorMovieLayers3D & layers3D = m_resourceMovie->getLayers3D();

		for( TVectorMovieLayers3D::const_iterator
			it = layers3D.begin(),
			it_end = layers3D.end();
		it != it_end;
		++it )
		{
			const MovieLayer3D & layer = *it;

			float layerIn = layer.in;
			float layerOut = layer.out;

			Sprite * sprite = m_flexSprites[layer.index];

			if( layer.internal == false )
			{
				if( layerIn >= lastTiming && layerIn <= m_timing )
				{
					sprite->enable();
				}
			}

			if( layer.out < lastTiming )
			{
				continue;
			}

			MovieFrame3D frame;
			if( layerOut >= lastTiming && layerOut <= m_timing )
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

		if( m_reverse == true )
		{
			if( lastTiming >= 0.f && m_timing <= 0.f )
			{
				if( this->getLoop() == true )
				{
					m_timing = m_out + m_timing;
				}
				else
				{
					this->end();
				}
			}		
		}
		else
		{
			if( m_out >= lastTiming && m_out <= m_timing )
			{
				if( this->getLoop() == true )
				{
					//m_timing = m_timing - m_out;
					m_timing = 0.f;

					this->setFirstFrame();
					this->_update( 0.f );
				}
				else
				{
					this->end();
				}
			}
		}
	}

	//////////////////////////////////////////////////////////////////////////
	void Movie::setReverse( bool _value )
	{
		m_reverse = _value;
	}
	//////////////////////////////////////////////////////////////////////////
	bool Movie::getReverse() const
	{
		return m_reverse;
	}
}