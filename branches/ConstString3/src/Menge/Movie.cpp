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
		////////////////////////////////////////////////////////////////////////////
		//static void s_applyRelationFrame2D( Node * _node, const MovieFrame2D & _frame, const mt::mat3f & _wm  )
		//{
		//	mt::vec2f wm_anchorPoint;
		//	mt::mul_v2_m3(wm_anchorPoint, _frame.anchorPoint, _wm);
		//	_node->setOrigin( wm_anchorPoint );

		//	mt::vec2f wm_pos;
		//	mt::mul_v2_m3(wm_pos, _frame.position, _wm);
		//	_node->setLocalPosition( wm_pos );

		//	mt::vec2f wm_scale;
		//	wm_scale, _frame.scale, _wm);
		//	_node->setScale( wm_scale );

		//	float wm_angle;
		//	_node->setAngle( _frame.angle );
		//	_node->setPersonalAlpha( _frame.opacity );
		//}
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
	void Movie::_setTiming( float _timing )
	{
		if( this->isActivate() == false )
		{
			MENGE_LOG_ERROR( "Movie.setTiming: '%s' not activate"
				, m_name.c_str()
				);

			return;
		}

		m_timing = _timing;

//		const TVectorMovieLayers2D & layers2D = m_resourceMovie->getLayers2D();
//
//		for( TVectorMovieLayers2D::const_iterator
//			it = layers2D.begin(),
//			it_end = layers2D.end();
//		it != it_end;
//		++it )
//		{
//			const MovieLayer2D & layer = *it;
//
//			TMapNode::iterator it_found = m_nodies.find( layer.index );
//
//			Node * node = it_found->second;
//
//
//			if( layer.animatable == true )
//			{
//				Animatable * animatable = dynamic_cast<Animatable *>(node);
//
//				animatable->setTiming( m_timing );
//
////				if( Animatable->isActivate() == true )
////				{
////					animatable->setTiming( m_timing );
////				}
//
//			}
//		}

		this->update( 0.f );
	}
	//////////////////////////////////////////////////////////////////////////
	float Movie::_getTiming() const
	{
		return m_timing;
	}
	//////////////////////////////////////////////////////////////////////////
	float Movie::getWorkAreaDuration() const
	{
		if( this->isCompile() == false )
		{
			MENGE_LOG_ERROR("Movie.getWorkAreaDuration %s not compile"
				, m_name.c_str()
				);

			return 0.f;
		}

		float duration = m_resourceMovie->getWorkAreaDuration();

		return duration; 
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
			float out = this->getWorkAreaDuration();

			m_timing = out;

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
		//m_timing = 0.f;

		this->callEventDeferred( EVENT_MOVIE_END, "(OiO)", this->getEmbed(), _enumerator, pybind::get_bool(false) );
	}
	//////////////////////////////////////////////////////////////////////////
	void Movie::_end( size_t _enumerator )
	{
		//m_timing = 0.f;

		this->callEventDeferred( EVENT_MOVIE_END, "(OiO)", this->getEmbed(), _enumerator, pybind::get_bool(true) );
	}
	//////////////////////////////////////////////////////////////////////////
	void Movie::updateFrame2D_( const MovieLayer2D & _layer, Node * _node, const MovieFrame2D & _frame )
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
				const mt::mat3f & wm = this->getWorldMatrix();

				mt::vec2f wm_pos;
				mt::mul_v2_m3(wm_pos, _frame.position, wm);
				PyObject * py_position = pybind::ptr(wm_pos);

				PyObject * py_node = pybind::ptr(_node);
				PyObject * py_anchorPoint = pybind::ptr(_frame.anchorPoint);

				//const mt::vec2f & movie_scale = this->getScale();

				//mt::vec2f wm_scale;
				//wm_scale.x = movie_scale.x * _frame.scale.x;
				//wm_scale.y = movie_scale.y * _frame.scale.y;

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
	void Movie::_setFirstFrame()
	{
		if( this->isActivate() == false )
		{
			MENGE_LOG_ERROR("Movie %s invalid setFirstFrame: not activate"
				, m_name.c_str()
				);

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

			if( layer.animatable == true )
			{
				Animatable * animatable = dynamic_cast<Animatable *>(node);

				animatable->setFirstFrame();
			}

			MovieFrame2D frame;
			if( m_resourceMovie->getFrame2DFirst( layer, frame ) == false )
			{
				MENGE_LOG_ERROR("Movie: '%s' frame first incorect '%s'"
					, m_name.c_str()
					, layer.name.c_str()
					);

				return;
			}

			this->updateFrame2D_( layer, node, frame );

			if( fabsf(layer.in) <= 0.001f )
			{
				node->localHide(false);
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

			TMapFlexSprite::iterator it_index = m_flexSprites.find( layer.index );

			if( it_index == m_flexSprites.end() )
			{
				MENGE_LOG_ERROR("Movie::setFirstFrame layer3D index %s:%d not found flex sprite"
					, layer.name.c_str()
					, layer.index
					);

				continue;
			}

			Sprite * sprite = it_index->second;

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

			if( fabsf(layer.in) <= 0.001f )
			{
				sprite->localHide(false);
			}
		}
	}
	//////////////////////////////////////////////////////////////////////////
	void Movie::_setLastFrame()
	{
		if( this->isActivate() == false )
		{
			MENGE_LOG_ERROR("Movie %s invalid setLastFrame: not activate"
				, m_name.c_str()
				);

			return;
		}

		float out = this->getWorkAreaDuration();

		const TVectorMovieLayers2D & layers2D = m_resourceMovie->getLayers2D();

		for( TVectorMovieLayers2D::const_iterator
			it = layers2D.begin(),
			it_end = layers2D.end();
		it != it_end;
		++it )
		{
			const MovieLayer2D & layer = *it;

			Node * node = m_nodies[layer.index];

			if( layer.animatable == true )
			{
				Animatable * animatable = dynamic_cast<Animatable *>(node);

				animatable->setLastFrame();
			}

			MovieFrame2D frame;
			if( m_resourceMovie->getFrame2DLast( layer, frame ) == false )
			{
				MENGE_LOG_ERROR("Movie: '%s' frame last incorect '%s'"
					, m_name.c_str()
					, layer.name.c_str()
					);

				return;
			}

			this->updateFrame2D_( layer, node, frame );

			if( fabsf(layer.out - out) <= 0.001f )
			{
				node->localHide(false);
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

			MovieFrame3D frame;
			if( m_resourceMovie->getFrame3DLast( layer, frame ) == false )
			{
				MENGE_LOG_ERROR("Movie: '%s' frame last incorect '%s'"
					, m_name.c_str()
					, layer.name.c_str()
					);

				return;
			}

			TMapFlexSprite::iterator it_index = m_flexSprites.find( layer.index );

			if( it_index == m_flexSprites.end() )
			{
				MENGE_LOG_ERROR("Movie::setLastFrame layer3D index %s:%d not found flex sprite"
					, layer.name.c_str()
					, layer.index
					);

				continue;
			}

			Sprite * sprite = it_index->second;

			Helper::s_applyFrame3D( sprite, frame );

			if( fabsf(layer.out - out) <= 0.001f )
			{
				sprite->localHide(false);
			}
		}
	}
	//////////////////////////////////////////////////////////////////////////
	void Movie::addMovieNode_(const MovieLayer2D & _layer2D, Node * _node )
	{		
		m_nodies[_layer2D.index] = _node;
	}
	//////////////////////////////////////////////////////////////////////////
	void Movie::addMovieSlot_( const ConstString & _slotName, Node * _node )
	{
		m_slots.insert(std::make_pair( _slotName, _node ));
	}
	//////////////////////////////////////////////////////////////////////////
	Node * Movie::getMovieSlot( const ConstString & _name )
	{
		if( this->isCompile() == false )
		{
			MENGE_LOG_ERROR("Movie %s invalid getMovieSlot: not compile"
				, m_name.c_str()
				);

			return NULL;
		}

		TMapMovieSlot::iterator it_index = m_slots.find( _name );

		if( it_index == m_slots.end() )
		{	
			const TVectorMovieLayers2D & layers2D = m_resourceMovie->getLayers2D();

			for( TVectorMovieLayers2D::const_iterator
				it = layers2D.begin(),
				it_end = layers2D.end();
			it != it_end;
			++it )
			{
				const MovieLayer2D & layer = *it;

				if( layer.movie == false )
				{
					continue;
				}

				Node * node = m_nodies[layer.index];

				Movie * movie = static_cast<Movie *>(node);

				Node * slot = movie->getMovieSlot( _name );

				if( slot == NULL )
				{
					continue;
				}

				return slot;
			}

//			MENGE_LOG_ERROR("Movie::getMovieSlot: %s not found slot %s"
//				, _name.c_str()
//				, m_name.c_str()
//				);

			return NULL;
		}

		Node * slot = it_index->second;

		return slot;
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

		float out = this->getWorkAreaDuration();

		const TVectorMovieLayers2D & layers2D = m_resourceMovie->getLayers2D();

		for( TVectorMovieLayers2D::const_iterator
			it = layers2D.begin(),
			it_end = layers2D.end();
		it != it_end;
		++it )
		{
			const MovieLayer2D & layer = *it;
			
			if ( layer.source == Consts::get()->c_MovieSlot )
			{
				Node * layer_slot = NodeManager::get()
					->createNodeT<Node>( layer.name, Consts::get()->c_Node, Consts::get()->c_Node );
				layer_slot->enable();
				layer_slot->localHide(true);
				this->addMovieNode_( layer, layer_slot );
				this->addMovieSlot_( layer.name, layer_slot );
				
				continue;
			}
			
			const ConstString & resourceType = ResourceManager::get()
				->getResourceType( layer.source );

			if( resourceType == Consts::get()->c_ResourceImageDefault
				|| resourceType == Consts::get()->c_ResourceImageInAtlas )
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

				this->addMovieNode_( layer, layer_sprite );
			}
			else if( resourceType == Consts::get()->c_ResourceAnimation )
			{
				Animation * layer_animation = NodeManager::get()
					->createNodeT<Animation>( layer.name, Consts::get()->c_Animation, Consts::get()->c_Image );

				//ResourceAnimation * resourceAnimation = ResourceManager::get()
				//	->getResourceT<ResourceAnimation>( layer.source );

				//if( resourceAnimation == NULL )
				//{
				//	MENGE_LOG_ERROR("Movie: '%s' can't get resourceAnimation '%s'"
				//		, m_name.c_str()
				//		, layer.source.c_str()
				//		);

				//	return false;
				//}

				layer_animation->setAnimationResource( layer.source );

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

				this->addMovieNode_( layer, layer_animation );
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

				this->addMovieNode_( layer, layer_movie );
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

				this->addMovieNode_( layer, layer_node );
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

				this->addMovieNode_( layer, layer_video );
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

				this->addMovieNode_( layer, layer_sound );
			}
			else if( resourceType == Consts::get()->c_ResourceEmitterContainer )
			{
				ParticleEmitter * layer_particles = NodeManager::get()
					->createNodeT<ParticleEmitter>( layer.name, Consts::get()->c_ParticleEmitter, Consts::get()->c_Image );

				layer_particles->setResource( layer.source );

				layer_particles->setLoop( true );				
				//layer_movie->disable();

				//layer_particles->setEmitterRelative(true);

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

				this->addMovieNode_( layer, layer_particles );
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
				layer_sprite->localHide( true );

				layer_sprite->setFlexible( true );

				m_flexSprites[layer.index] = layer_sprite;
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
		//if( this->isActivate() == false )
		//{
		//	return;
		//}

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
				MENGE_LOG_ERROR("Movie.updateParent_: '%s' not found layer '%s' '%d'"
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
					MENGE_LOG_ERROR("Movie.updateParent_: '%s' not found layer '%s' '%d' parent '%d'"
						, m_name.c_str()
						, layer.name.c_str()
						, layer.index
						, layer.parent
						);

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

			TMapFlexSprite::iterator it_index = m_flexSprites.find( layer.index );

			if( it_index == m_flexSprites.end() )
			{
				MENGE_LOG_ERROR("Movie::updateParent_ layer3D index %s:%d not found flex sprite"
					, layer.name.c_str()
					, layer.index
					);

				continue;
			}

			Node * node = it_index->second;

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
					MENGE_LOG_ERROR("Movie::updateParent_ layer3D parent %s:%d not found flex sprite"
						, layer.name.c_str()
						, layer.parent
						);

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
				continue;
			}

			if( layer.parent != 0 )
			{
				continue;
			}

			TMapNode::const_iterator it_node = m_nodies.find( layer.index );

			if( it_node == m_nodies.end() )
			{
				MENGE_LOG_ERROR("Movie._release: '%s' not found layer2D '%s' '%d'"
					, m_name.c_str()
					, layer.name.c_str()
					, layer.index
					);

				continue;
			}

			Node * node = it_node->second;

			node->destroy();
		}

		m_nodies.clear();

		const TVectorMovieLayers3D & layers3D = m_resourceMovie->getLayers3D();

		for( TVectorMovieLayers3D::const_iterator 
			it = layers3D.begin(),
			it_end = layers3D.end();
		it != it_end;
		++it )
		{
			const MovieLayer3D & layer = *it;

			if( layer.internal == true )
			{
				continue;
			}

			if( layer.parent != 0 )
			{
				continue;
			}

			TMapFlexSprite::const_iterator it_node = m_flexSprites.find( layer.index );

			if( it_node == m_flexSprites.end() )
			{
				MENGE_LOG_ERROR("Movie._release: '%s' not found layer3D '%s' '%d'"
					, m_name.c_str()
					, layer.name.c_str()
					, layer.index
					);

				continue;
			}

			Sprite * sprite = it_node->second;

			sprite->destroy();
		}

		m_flexSprites.clear();

		m_slots.clear();

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

		float realTiming = _timing * m_speedFactor;

		if( m_reverse == true )
		{
			m_timing -= realTiming;
		}
		else
		{
			m_timing += realTiming;
		}

		float out = this->getWorkAreaDuration();

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

			if( layerOut > out )
			{
				layerOut = out;
			}

			//if (m_reverse)
			//{
			//	layerIn = layer.out;
			//	layerOut = layer.in;
			//}

			TMapNode::iterator it_found = m_nodies.find( layer.index );

			if( it_found == m_nodies.end() )
			{
				MENGE_LOG_ERROR("Movie.update: '%s' not found layer '%s' '%d'"
					, m_name.c_str()
					, layer.name.c_str()
					, layer.index
					);

				continue;
			}

			Node * node = it_found->second;

			//Node * node = m_nodies[layer.index];

			if( layer.internal == false )
			{
				if( layerIn <= m_timing && m_timing <= layerOut )
				{
					//printf("Movie %s enable %f %d\n", m_name.c_str(), m_timing, layer.index);
					node->localHide(false);

					if( layer.animatable == true )
					{
						Animatable * animatable = dynamic_cast<Animatable *>(node);

						if( animatable->isPlay() == false )
						{
							animatable->setSpeedFactor( m_speedFactor );							

							animatable->play();
							animatable->setTiming( lastTiming - layerIn );
							//animatable->update(realTiming);
						}
					}
				}
			}

			if( layerOut < lastTiming || layerIn > lastTiming )
			{
				continue;
			}
			
			MovieFrame2D frame;

			if( m_reverse == false )
			{
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
						//if( layerIn > 0.001f || fabsf(layerOut - out) > 0.001f )
						//{
						//printf("Movie %s disable %f %d\n", m_name.c_str(), m_timing, layer.index);
						node->localHide(true);

						if( layer.animatable == true )
						{
							Animatable * animatable = dynamic_cast<Animatable *>(node);

							animatable->stop();
						}

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
			}
			else
			{
				if( layerIn <= lastTiming && layerIn >= m_timing )
				{
					if( m_resourceMovie->getFrame2DFirst( layer, frame ) == false )
					{
						MENGE_LOG_ERROR("Movie: '%s' frame first incorect '%s'"
							, m_name.c_str()
							, layer.name.c_str()
							);

						continue;
					}

					if( layer.internal == false )
					{
						//if( layerIn > 0.001f || fabsf(layerOut - out) > 0.001f )
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
			}

			this->updateFrame2D_( layer, node, frame );
		}

		//const TVectorMovieLayers3D & layers3D = m_resourceMovie->getLayers3D();

		//for( TVectorMovieLayers3D::const_iterator
		//	it = layers3D.begin(),
		//	it_end = layers3D.end();
		//it != it_end;
		//++it )
		//{
		//	const MovieLayer3D & layer = *it;

		//	float layerIn = layer.in;
		//	float layerOut = layer.out;

		//	TMapFlexSprite::iterator it_index = m_flexSprites.find( layer.index );

		//	if( it_index == m_flexSprites.end() )
		//	{
		//		MENGE_LOG_ERROR("Movie::update layer3D index %s:%d not found flex sprite"
		//			, layer.name.c_str()
		//			, layer.index
		//			);

		//		continue;
		//	}

		//	Sprite * sprite = it_index->second;

		//	if( layer.internal == false )
		//	{
		//		if( layerIn >= lastTiming && layerIn <= m_timing )
		//		{
		//			sprite->localHide( false );
		//		}
		//	}

		//	if( layer.out < lastTiming )
		//	{
		//		continue;
		//	}

		//	MovieFrame3D frame;
		//	if( layerOut >= lastTiming && layerOut <= m_timing )
		//	{
		//		if( m_resourceMovie->getFrame3DLast( layer, frame ) == false )
		//		{
		//			MENGE_LOG_ERROR("Movie: '%s' frame last incorect '%s'"
		//				, m_name.c_str()
		//				, layer.name.c_str()
		//				);

		//			continue;
		//		}

		//		if( layer.internal == false )
		//		{
		//			sprite->localHide( true );

		//			continue;
		//		}
		//	}
		//	else
		//	{
		//		if( m_resourceMovie->getFrame3D( layer, m_timing, frame ) == false )
		//		{
		//			continue;
		//		}
		//	}

		//	Helper::s_applyFrame3D( sprite, frame );
		//}

		if( m_reverse == false )
		{
			if( out >= lastTiming && out <= m_timing )
			{
				if( this->getLoop() == true )
				{
					//m_timing = m_timing - out;
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
		else
		{
			if( lastTiming >= 0.f && m_timing <= 0.f )
			{
				if( this->getLoop() == true )
				{
					m_timing = out + m_timing;

					this->setLastFrame();

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
	void Movie::_setReverse( bool _value )
	{
		
	}
}