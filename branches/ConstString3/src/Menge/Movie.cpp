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
			_node->setRotateX( -_frame.angle );
			_node->setPersonalAlpha( _frame.opacity );
		}
		//////////////////////////////////////////////////////////////////////////
		static void s_applyFrame2DMovie( Node * _node, const MovieFrame2D & _frame )
		{
			_node->setOrigin( _frame.anchorPoint );
			_node->setLocalPosition( _frame.position );
			_node->setScale( _frame.scale );
			_node->setRotateX( -_frame.angle );
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
		//, m_timing(0.f)
		, m_frameTiming(0.f)
		, m_currentFrame(0)
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

		float frameDuration = m_resourceMovie->getFrameDuration();

		if( _timing > 0.f )
		{
			m_currentFrame = (size_t)floorf(_timing / frameDuration);
			m_frameTiming = _timing - m_currentFrame * frameDuration;
		}
		else
		{
			m_currentFrame = 0;
			m_frameTiming = _timing;
		}

		//printf("Movie._setTiming %s %d [%.2f]\n"
		//	, m_name.c_str()
		//	, m_currentFrame
		//	, _timing
		//	);

		this->updateCurrentFrame_( m_currentFrame, true );

		//m_timing = _timing;

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

		//this->update( 0.f );
	}
	//////////////////////////////////////////////////////////////////////////
	float Movie::_getTiming() const
	{
		if( this->isCompile() == false )
		{
			MENGE_LOG_ERROR( "Movie.getTiming: '%s' not compile"
				, m_name.c_str()
				);

			return 0.f;
		}


		float frameDuration = m_resourceMovie->getFrameDuration();

		float timing = m_currentFrame * frameDuration + m_frameTiming;

		return timing;
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
		if( this->isActivate() == false )
		{
			return false;
		}

		if( m_reverse == true )
		{
			//float out = this->getWorkAreaDuration();

			//m_frameTiming = 0.f;
			//m_currentFrame = 0;

			//this->setLastFrame();
		}
		else
		{
			m_frameTiming = 0.f;
			m_currentFrame = 0;

			//this->setFirstFrame();
		}

		this->updateParent_();

		this->updateCurrentFrame_( m_currentFrame, true );

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
		
		//const TVectorMovieLayers2D & layers2D = m_resourceMovie->getLayers2D();

		//for( TVectorMovieLayers2D::const_iterator
		//	it = layers2D.begin(),
		//	it_end = layers2D.end();
		//it != it_end;
		//++it )
		//{
		//	const MovieLayer2D & layer = *it;

		//	if( layer.internal == true )
		//	{
		//		continue;
		//	}

		//	if( layer.animatable == false )
		//	{
		//		continue;
		//	}

		//	TMapNode::iterator it_found = m_nodies.find( layer.index );

		//	if( it_found == m_nodies.end() )
		//	{
		//		MENGE_LOG_ERROR("Movie._stop: '%s' not found layer '%s' '%d'"
		//			, m_name.c_str()
		//			, layer.name.c_str()
		//			, layer.index
		//			);

		//		continue;
		//	}

		//	Node * node = it_found->second;

		//	Animatable * animatable = dynamic_cast<Animatable *>(node);

		//	if( animatable->isPlay() == true )
		//	{
		//		//printf("Movie %s stop[end] animatable %s\n"
		//		//	, m_name.c_str()
		//		//	, node->getName().c_str()
		//		//	);

		//		animatable->stop();


		//		//float timing = (indexOut - indexIn) * frameDuration;
		//		//animatable->setLastFrame( timing );
		//	}
		//}

		this->callEventDeferred( EVENT_MOVIE_END, "(OiO)", this->getEmbed(), _enumerator, pybind::get_bool(false) );
	}
	//////////////////////////////////////////////////////////////////////////
	void Movie::_end( size_t _enumerator )
	{
		//m_timing = 0.f;

		//const TVectorMovieLayers2D & layers2D = m_resourceMovie->getLayers2D();

		//for( TVectorMovieLayers2D::const_iterator
		//	it = layers2D.begin(),
		//	it_end = layers2D.end();
		//it != it_end;
		//++it )
		//{
		//	const MovieLayer2D & layer = *it;

		//	if( layer.internal == true )
		//	{
		//		continue;
		//	}

		//	if( layer.animatable == false )
		//	{
		//		continue;
		//	}

		//	TMapNode::iterator it_found = m_nodies.find( layer.index );

		//	if( it_found == m_nodies.end() )
		//	{
		//		MENGE_LOG_ERROR("Movie._stop: '%s' not found layer '%s' '%d'"
		//			, m_name.c_str()
		//			, layer.name.c_str()
		//			, layer.index
		//			);

		//		continue;
		//	}

		//	Node * node = it_found->second;

		//	Animatable * animatable = dynamic_cast<Animatable *>(node);

		//	if( animatable->isPlay() == true )
		//	{
		//		//printf("Movie %s stop[end] animatable %s\n"
		//		//	, m_name.c_str()
		//		//	, node->getName().c_str()
		//		//	);

		//		animatable->stop();

		//		//float timing = (indexOut - indexIn) * frameDuration;
		//		//animatable->setTiming( timing );
		//	}
		//}

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
				const mt::mat4f & wm = this->getWorldMatrix();

				mt::vec3f wm_pos;
				mt::mul_v3_m4(wm_pos, _frame.position, wm);
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

		//m_timing = 0.f;

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
			
			if ( layer.source == Consts::get()->c_MovieNullObject )
			{
				Node * layer_slot = NodeManager::get()
					->createNodeT<Node>( layer.name, Consts::get()->c_Node, Consts::get()->c_Node );
				layer_slot->enable();
				layer_slot->localHide(true);
				this->addMovieNode_( layer, layer_slot );
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

				layer_particles->setLoop( true );
				layer_particles->setEmitterTranslateWithParticle( true );

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

		//printf("Movie._update %s %f:%f:%f\n"
		//	, m_name.c_str()
		//	, _timing
		//	, m_frameTiming
		//	, this->getTiming()
		//	);

		//if( m_parentMovie == false )
		//{
		//	printf("Movie %s update %f:%f\n", m_name.c_str(), m_timing, _timing);
		//}

		//float lastTiming = m_timing;

		float frameDuration = m_resourceMovie->getFrameDuration();

		if( frameDuration == 0.f )
		{
			return;
		}

		float realTiming = _timing * m_speedFactor;

		m_frameTiming += realTiming;

		size_t lastFrame = m_currentFrame;

		size_t frameCount = m_resourceMovie->getFrameCount();

		while( m_frameTiming >= frameDuration )
		{
			m_frameTiming -= frameDuration;

			++m_currentFrame;

			if( m_currentFrame == frameCount - 1 )
			{
				if( this->getLoop() == false )
				{
					break;
				}
			}
			else if ( m_currentFrame == frameCount )
			{
				if( this->getLoop() == true )
				{
					m_currentFrame = 0;
				}
			}	
		}

		//if( m_currentFrame == frameCount + 1 )
		//{
		//	m_currentFrame = frameCount;
		//}

		if( lastFrame != m_currentFrame )
		{
			this->updateCurrentFrame_( lastFrame, false );
		}

		if( m_currentFrame == frameCount )
		{
			if( this->getLoop() == false )
			{
				this->end();
			}
		}
	}
	//////////////////////////////////////////////////////////////////////////
	void Movie::updateCurrentFrame_( size_t _lastFrame, bool _force )
	{
		//printf("Movie.updateCurrentFrame_ %s %d-%d [%.2f]\n"
		//	, m_name.c_str()
		//	, _lastFrame
		//	, m_currentFrame			
		//	, this->getTiming()
		//	);

		float frameDuration = m_resourceMovie->getFrameDuration();
		size_t frameCount = m_resourceMovie->getFrameCount();

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
			
			//if( layerOut > out )
			//{
			//	layerOut = out;
			//}

			size_t indexIn = floorf((layerIn / frameDuration) + 0.5f);
			size_t indexOut = floorf((layerOut / frameDuration) + 0.5f);

			//if( indexOut > frameCount )
			//{
			//	indexOut = frameCount;
			//}

			if( indexOut < _lastFrame || indexIn > m_currentFrame )
			{
				continue;
			}

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

			MovieFrame2D frame;

			if( m_currentFrame >= indexOut || m_currentFrame < indexIn )
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

						//printf("Movie %s stop[layer] animatable %s\n"
						//	, m_name.c_str()
						//	, node->getName().c_str()
						//	);

						if( animatable->isPlay() == true )
						{
							animatable->stop();

							float timing = (indexOut - indexIn) * frameDuration;
							animatable->setTiming( timing );
						}
					}

					continue;
				}
			}
			else if( m_currentFrame >= frameCount && m_currentFrame <= indexOut )
			{
				if( m_resourceMovie->getFrame2D( layer, frameCount - indexIn, frame ) == false )
				{
					continue;
				}

				if( layer.internal == false )
				{
					//printf("Movie %s disable %f %d\n", m_name.c_str(), m_timing, layer.index);
					//node->localHide(true);

					if( layer.animatable == true )
					{
						Animatable * animatable = dynamic_cast<Animatable *>(node);

						if( animatable->isPlay() == true )
						{
							//printf("Movie %s stop[end] animatable %s\n"
							//	, m_name.c_str()
							//	, node->getName().c_str()
							//	);

							animatable->stop();

							float timing = (frameCount - indexIn) * frameDuration;
							animatable->setTiming( timing );
						}
					}

					continue;
				}
			}
			else if( m_currentFrame >= indexIn && m_currentFrame < indexOut )
			{
				if( m_resourceMovie->getFrame2D( layer, m_currentFrame - indexIn, frame ) == false )
				{
					continue;
				}

				if( layer.internal == false )
				{			
					//printf("Movie %s enable %f %d\n", m_name.c_str(), m_timing, layer.index);
					node->localHide(false);

					if( layer.animatable == true )
					{
						Animatable * animatable = dynamic_cast<Animatable *>(node);

						if( animatable->isPlay() == false || _force == true )
						{
							animatable->setSpeedFactor( m_speedFactor );							

							//printf("Movie %s play animatable %s\n"
							//	, m_name.c_str()
							//	, node->getName().c_str()
							//	);

							animatable->play();

							if( _lastFrame <= m_currentFrame )
							{
								if( _lastFrame >= indexIn )
								{
									float timing = (_lastFrame - indexIn) * frameDuration + m_frameTiming;
									animatable->setTiming( timing );
								}
								else
								{
									float timing = (indexIn - _lastFrame) * frameDuration + m_frameTiming;
									animatable->setTiming( -timing );
								}
							}
							else
							{
								size_t frame = (indexOut - 1) - _lastFrame + m_currentFrame;
								float timing = frame * frameDuration + m_frameTiming;

								//printf("------ %d %d %d %f %f == %d %f\n"
								//	, indexOut
								//	, _lastFrame
								//	, m_currentFrame
								//	, frameDuration
								//	, m_frameTiming
								//	, frame
								//	, timing
								//	);

								animatable->setTiming( -timing );
							}
							//animatable->update(realTiming);
						}
					}
				}
			}			

			this->updateFrame2D_( layer, node, frame );
		}
	}
	//////////////////////////////////////////////////////////////////////////
	void Movie::_setReverse( bool _value )
	{
		
	}
}