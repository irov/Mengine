#	include "Movie.h"

#	include "ResourceManager.h"
#	include "ResourceMovie.h"

#	include "ResourceImageDefault.h"
#	include "ResourceInternalObject.h"
#	include "ResourceAnimation.h"
#	include "ResourceImageSolid.h"

#	include "Camera3D.h"

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
		static void s_applyFrame( Node * _node, const MovieFrameSource & _frame )
		{	
			_node->setOrigin( _frame.anchorPoint );
			_node->setLocalPosition( _frame.position );
			_node->setScale( _frame.scale );

			_node->setRotateX( _frame.rotation.x );
			_node->setRotateY( _frame.rotation.y );
			_node->setRotateZ( _frame.rotation.z );

			_node->setPersonalAlpha( _frame.opacity );
		}
		//////////////////////////////////////////////////////////////////////////
		static void s_applyFrameMovie( Node * _node, const MovieFrameSource & _frame )
		{
			_node->setOrigin( _frame.anchorPoint );
			_node->setLocalPosition( _frame.position );
			_node->setScale( _frame.scale );

			_node->setRotateX( _frame.rotation.x );
			_node->setRotateY( _frame.rotation.y );
			_node->setRotateZ( _frame.rotation.z );

			_node->setLocalAlpha( _frame.opacity );
		}
	}
	//////////////////////////////////////////////////////////////////////////
	Movie::Movie()
		: m_resourceMovie(NULL)
		, m_frameTiming(0.f)
		, m_currentFrame(0)
		, m_parentMovie(false)
		, m_renderCamera3D(0)
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
			m_currentFrame = (size_t)floorf( (_timing / frameDuration) );
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

		this->updateTiming_();
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
	bool Movie::_play()
	{
		if( this->isActivate() == false )
		{
			return false;
		}

		if( m_reverse == true )
		{
			m_frameTiming = 0.f;
			m_currentFrame = m_resourceMovie->getFrameCount();

			//this->setLastFrame();
		}
		else
		{
			m_frameTiming = 0.f;
			m_currentFrame = 0;

			//this->setFirstFrame();
		}

		//this->updateParent_();

		//this->updateCurrentFrame_( m_currentFrame, true );

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
		this->stopAnimation_();

		this->callEventDeferred( EVENT_MOVIE_END, "(OiO)", this->getEmbed(), _enumerator, pybind::get_bool(false) );
	}
	//////////////////////////////////////////////////////////////////////////
	void Movie::_end( size_t _enumerator )
	{
		this->stopAnimation_();

		this->callEventDeferred( EVENT_MOVIE_END, "(OiO)", this->getEmbed(), _enumerator, pybind::get_bool(true) );
	}
	//////////////////////////////////////////////////////////////////////////
	void Movie::updateFrame2D_( const MovieLayer & _layer, Node * _node, const MovieFrameSource & _frame )
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

				PyObject * py_angle = pybind::ptr(_frame.rotation);
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
				PyObject * py_angle = pybind::ptr(_frame.rotation);
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
			if( _layer.movie == false )
			{
				Helper::s_applyFrame( _node, _frame );
			}
			else
			{
				Helper::s_applyFrameMovie( _node, _frame );
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

		const TVectorMovieLayers & layers = m_resourceMovie->getLayers();

		for( TVectorMovieLayers::const_iterator
			it = layers.begin(),
			it_end = layers.end();
		it != it_end;
		++it )
		{
			const MovieLayer & layer = *it;

			Node * node = m_nodies[layer.index];

			if( layer.animatable == true )
			{
				Animatable * animatable = dynamic_cast<Animatable *>(node);

				animatable->setFirstFrame();
			}

			MovieFrameSource frame;
			if( m_resourceMovie->getFrameFirst( layer, frame ) == false )
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

		float out = m_resourceMovie->getDuration();

		const TVectorMovieLayers & layers = m_resourceMovie->getLayers();

		for( TVectorMovieLayers::const_iterator
			it = layers.begin(),
			it_end = layers.end();
		it != it_end;
		++it )
		{
			const MovieLayer & layer = *it;

			Node * node = m_nodies[layer.index];

			if( layer.animatable == true )
			{
				Animatable * animatable = dynamic_cast<Animatable *>(node);

				animatable->setLastFrame();
			}

			MovieFrameSource frame;
			if( m_resourceMovie->getFrameLast( layer, frame ) == false )
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
	void Movie::addMovieNode_(const MovieLayer & _layer2D, Node * _node )
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

		TMapMovieSlot::iterator it_found = m_slots.find( _name );

		if( it_found != m_slots.end() )
		{	
			Node * slot = it_found->second;

			return slot;
		}

		const TVectorMovieLayers & layers = m_resourceMovie->getLayers();

		for( TVectorMovieLayers::const_iterator
			it = layers.begin(),
			it_end = layers.end();
		it != it_end;
		++it )
		{
			const MovieLayer & layer = *it;

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

		MENGE_LOG_ERROR("Movie::getMovieSlot: %s not found slot %s"
			, _name.c_str()
			, m_name.c_str()
			);

		return NULL;
	}
	//////////////////////////////////////////////////////////////////////////
	bool Movie::hasMovieSlot( const ConstString & _name )
	{
		if( this->isCompile() == false )
		{
			MENGE_LOG_ERROR("Movie %s invalid hasMovieSlot: not compile"
				, m_name.c_str()
				);

			return NULL;
		}

		TMapMovieSlot::iterator it_found = m_slots.find( _name );

		if( it_found != m_slots.end() )
		{	
			return true;
		}

		const TVectorMovieLayers & layers = m_resourceMovie->getLayers();

		for( TVectorMovieLayers::const_iterator
			it = layers.begin(),
			it_end = layers.end();
		it != it_end;
		++it )
		{
			const MovieLayer & layer = *it;

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

			return true;
		}
				
		return false;
	}
	//////////////////////////////////////////////////////////////////////////
	Scriptable * Movie::findInternalObject_( const ConstString & _resource, EEventName _event ) const
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
	bool Movie::createMovieSlot_( const MovieLayer & _layer )
	{
		Node * layer_slot = NodeManager::get()
			->createNodeT<Node>( _layer.name, Consts::get()->c_Node, Consts::get()->c_Node );

		layer_slot->enable();
		layer_slot->localHide(true);

		this->addMovieNode_( _layer, layer_slot );
		this->addMovieSlot_( _layer.name, layer_slot );

		return true;
	}
	//////////////////////////////////////////////////////////////////////////
	bool Movie::createMovieNullObject_( const MovieLayer & _layer )
	{
		Node * layer_slot = NodeManager::get()
			->createNodeT<Node>( _layer.name, Consts::get()->c_Node, Consts::get()->c_Node );

		layer_slot->enable();
		layer_slot->localHide(true);

		this->addMovieNode_( _layer, layer_slot );

		return true;
	}
	//////////////////////////////////////////////////////////////////////////
	bool Movie::createMovieImage_( const MovieLayer & _layer )
	{
		Sprite * layer_sprite = NodeManager::get()
			->createNodeT<Sprite>( _layer.name, Consts::get()->c_Sprite, Consts::get()->c_Image );

		layer_sprite->setImageResource( _layer.source );

		//layer_sprite->disable();

		if( layer_sprite->compile() == false )
		{
			MENGE_LOG_ERROR("Movie: '%s' can't compile sprite '%s'"
				, m_name.c_str()
				, _layer.name.c_str()
				);

			layer_sprite->destroy();

			return false;
		}

		layer_sprite->enable();
		layer_sprite->localHide(true);

		this->addMovieNode_( _layer, layer_sprite );

		return true;
	}
	//////////////////////////////////////////////////////////////////////////
	bool Movie::createMovieImageSolid_( const MovieLayer & _layer )
	{
		ResourceImageSolid * resource =  ResourceManager::get()
			->getResourceT<ResourceImageSolid>( _layer.source );

		if( resource == NULL )
		{
			MENGE_LOG_ERROR("Movie: '%s' can't compile sprite '%s' imageSolid resource = NULL"
				, m_name.c_str()
				, _layer.name.c_str()
				);

			return false;
		}

		Sprite * layer_sprite = NodeManager::get()
			->createNodeT<Sprite>( _layer.name, Consts::get()->c_Sprite, Consts::get()->c_Image );

		layer_sprite->setImageResource( Consts::get()->c_WhitePixel );

		const ColourValue& color = resource->getColor();
		layer_sprite->setLocalColor( color );

		const mt::vec2f& size = resource->getSize();
		layer_sprite->setSpriteSize( size );

		if( layer_sprite->compile() == false )
		{
			MENGE_LOG_ERROR("Movie: '%s' can't compile sprite '%s'"
				, m_name.c_str()
				, _layer.name.c_str()
				);

			layer_sprite->destroy();

			return false;
		}

		layer_sprite->enable();
		layer_sprite->localHide(true);

		this->addMovieNode_( _layer, layer_sprite );

		return true;
	}
	//////////////////////////////////////////////////////////////////////////
	bool Movie::createMovieAnimation_( const MovieLayer & _layer )
	{
		Animation * layer_animation = NodeManager::get()
			->createNodeT<Animation>( _layer.name, Consts::get()->c_Animation, Consts::get()->c_Image );

		layer_animation->setAnimationResource( _layer.source );

		layer_animation->setLoop( true );				
		//layer_animation->disable();

		if( layer_animation->compile() == false )
		{
			MENGE_LOG_ERROR("Movie: '%s' can't compile animation '%s'"
				, m_name.c_str()
				, _layer.name.c_str()
				);

			layer_animation->destroy();

			return false;
		}

		layer_animation->enable();
		layer_animation->localHide( true );

		this->addMovieNode_( _layer, layer_animation );

		return true;
	}
	//////////////////////////////////////////////////////////////////////////
	bool Movie::createMovieMovie_( const MovieLayer & _layer )
	{
		Movie * layer_movie = NodeManager::get()
			->createNodeT<Movie>( _layer.name, Consts::get()->c_Movie, Consts::get()->c_Image );

		layer_movie->setResourceMovie( _layer.source );				
		layer_movie->setLoop( true );				
		//layer_movie->disable();

		if( layer_movie->compile() == false )
		{
			MENGE_LOG_ERROR("Movie: '%s' can't compile movie '%s'"
				, m_name.c_str()
				, _layer.name.c_str()
				);

			layer_movie->destroy();

			return false;
		}

		layer_movie->enable();
		layer_movie->localHide( true );
		layer_movie->setParentMovie( true );

		this->addMovieNode_( _layer, layer_movie );

		return true;
	}
	//////////////////////////////////////////////////////////////////////////
	bool Movie::createMovieInternalObject_( const MovieLayer & _layer )
	{
		Scriptable * scriptable = this->findInternalObject_( _layer.source, EVENT_MOVIE_FIND_INTERNAL_NODE );

		if( scriptable == NULL )
		{
			return false;
		}				

		Node * layer_node = dynamic_cast<Node*>(scriptable);

		if( layer_node == 0 )
		{
			MENGE_LOG_ERROR("Movie: '%s' internal node '%s' not type 'Node'"
				, m_name.c_str()
				, _layer.source.c_str()
				);

			return false;
		}

		this->addMovieNode_( _layer, layer_node );

		return true;
	}
	//////////////////////////////////////////////////////////////////////////
	bool Movie::createMovieVideo_( const MovieLayer & _layer )
	{
		Video * layer_video = NodeManager::get()
			->createNodeT<Video>( _layer.name, Consts::get()->c_Video, Consts::get()->c_Image );

		layer_video->setVideoResource( _layer.source );

		layer_video->setLoop( true );				
		//layer_movie->disable();

		if( layer_video->compile() == false )
		{
			MENGE_LOG_ERROR("Movie: '%s' can't compile video '%s'"
				, m_name.c_str()
				, _layer.name.c_str()
				);

			layer_video->destroy();

			return false;
		}

		layer_video->enable();
		layer_video->localHide(true);

		this->addMovieNode_( _layer, layer_video );

		return true;
	}
	//////////////////////////////////////////////////////////////////////////
	bool Movie::createMovieSound_( const MovieLayer & _layer )
	{
		SoundEmitter * layer_sound = NodeManager::get()
			->createNodeT<SoundEmitter>( _layer.name, Consts::get()->c_SoundEmitter, Consts::get()->c_Sound );

		layer_sound->setSoundResource( _layer.source );

		layer_sound->setLoop( true );				
		//layer_movie->disable();

		if( layer_sound->compile() == false )
		{
			MENGE_LOG_ERROR("Movie: '%s' can't compile sound '%s'"
				, m_name.c_str()
				, _layer.name.c_str()
				);

			layer_sound->destroy();

			return false;
		}

		layer_sound->enable();
		layer_sound->localHide( true );

		this->addMovieNode_( _layer, layer_sound );

		return true;
	}
	//////////////////////////////////////////////////////////////////////////
	bool Movie::createMovieEmitterContainer_( const MovieLayer & _layer )
	{
		ParticleEmitter * layer_particles = NodeManager::get()
			->createNodeT<ParticleEmitter>( _layer.name, Consts::get()->c_ParticleEmitter, Consts::get()->c_Image );

		layer_particles->setResource( _layer.source );

		//layer_movie->disable();

		//layer_particles->setEmitterRelative(true);

		if( layer_particles->compile() == false )
		{
			MENGE_LOG_ERROR("Movie: '%s' can't compile ParticleEmitter '%s'"
				, m_name.c_str()
				, _layer.name.c_str()
				);

			layer_particles->destroy();

			return false;
		}

		layer_particles->setLoop( true );
		layer_particles->setEmitterTranslateWithParticle( true );

		layer_particles->enable();
		layer_particles->localHide( true );

		this->addMovieNode_( _layer, layer_particles );

		return true;
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

		size_t maxLayerIndex = m_resourceMovie->getMaxLayerIndex();

		m_nodies.resize( maxLayerIndex + 1 );

		float out = m_resourceMovie->getDuration();

		this->createCamera3D_();

		const TVectorMovieLayers & layers = m_resourceMovie->getLayers();

		for( TVectorMovieLayers::const_iterator
			it = layers.begin(),
			it_end = layers.end();
		it != it_end;
		++it )
		{
			const MovieLayer & layer = *it;
			
			if ( layer.source == Consts::get()->c_MovieSlot )
			{
				if( this->createMovieSlot_( layer ) == false )
				{
					return false;
				}
				
				continue;
			}
			else if ( layer.source == Consts::get()->c_MovieNullObject )
			{
				if( this->createMovieNullObject_( layer ) == false )
				{
					return false;
				}

				continue;
			}

			const ConstString & resourceType = ResourceManager::get()
				->getResourceType( layer.source );

			if( resourceType == Consts::get()->c_ResourceImageDefault
				|| resourceType == Consts::get()->c_ResourceImageInAtlas )
			{
				if( this->createMovieImage_( layer ) == false )
				{
					return false;
				}
			}
			else if( resourceType == Consts::get()->c_ResourceImageSolid )
			{
				if( this->createMovieImageSolid_( layer ) == false )
				{
					return false;
				}
			}
			else if( resourceType == Consts::get()->c_ResourceAnimation )
			{
				if( this->createMovieAnimation_( layer ) == false )
				{
					return false;
				}
			}
			else if( resourceType == Consts::get()->c_ResourceMovie )
			{
				if( this->createMovieMovie_( layer ) == false )
				{
					return false;
				}
			}
			else if( resourceType == Consts::get()->c_ResourceInternalObject )
			{				
				if( this->createMovieInternalObject_( layer ) == false )
				{
					return false;
				}
			}
			else if( resourceType == Consts::get()->c_ResourceVideo )
			{
				if( this->createMovieVideo_( layer ) == false )
				{
					return false;
				}
			}
			else if( resourceType == Consts::get()->c_ResourceSound )
			{
				if( this->createMovieSound_( layer ) == false )
				{
					return false;
				}
			}
			else if( resourceType == Consts::get()->c_ResourceEmitterContainer )
			{
				if( this->createMovieEmitterContainer_( layer ) == false )
				{
					return false;
				}
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
			
		bool reverse = this->getReverse();
		this->updateReverse_( reverse );

		return true;
	}
	//////////////////////////////////////////////////////////////////////////
	void Movie::updateParent_()
	{
		//if( this->isActivate() == false )
		//{
		//	return;
		//}

		const TVectorMovieLayers & layers = m_resourceMovie->getLayers();

		for( TVectorMovieLayers::const_iterator 
			it = layers.begin(),
			it_end = layers.end();
		it != it_end;
		++it )
		{
			const MovieLayer & layer = *it;
			
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

			if( layer.threeD == true )
			{
				node->setRenderCamera( m_renderCamera3D );
			}
		}
	}
	//////////////////////////////////////////////////////////////////////////
	void Movie::stopAnimation_()
	{
		if( this->isCompile() == false )
		{
			MENGE_LOG_ERROR("Movie::stopAnimation_ %s is not compile"
				, m_name.c_str()
				);

			return;
		}

		const TVectorMovieLayers & layers = m_resourceMovie->getLayers();

		for( TVectorMovieLayers::const_iterator
			it = layers.begin(),
			it_end = layers.end();
		it != it_end;
		++it )
		{
			const MovieLayer & layer = *it;

			if( layer.internal == true )
			{
				continue;
			}

			if( layer.animatable == false )
			{
				continue;
			}
			
			Node * node = m_nodies[layer.index];

			Animatable * animatable = dynamic_cast<Animatable *>(node);

			if( animatable->isPlay() == true )
			{
				//printf("Movie %s stop[end] animatable %s\n"
				//	, m_name.c_str()
				//	, node->getName().c_str()
				//	);

				animatable->stop();

				float movie_timing = this->getTiming();

				float animatable_timing = movie_timing - layer.in;

				animatable->setTiming( animatable_timing );
			}
		}
	}
	//////////////////////////////////////////////////////////////////////////
	void Movie::_release()
	{	
		const TVectorMovieLayers & layers = m_resourceMovie->getLayers();

		for( TVectorMovieLayers::const_iterator 
			it = layers.begin(),
			it_end = layers.end();
		it != it_end;
		++it )
		{
			const MovieLayer & layer = *it;

			if( layer.internal == true )
			{
				continue;
			}

			if( layer.parent != 0 )
			{
				continue;
			}
			
			Node * node = m_nodies[layer.index];

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

		const TVectorMovieLayers & layers = m_resourceMovie->getLayers();

		for( TVectorMovieLayers::const_iterator
			it = layers.begin(),
			it_end = layers.end();
		it != it_end;
		++it )
		{
			const MovieLayer & layer = *it;

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
	void Movie::_deactivate()
	{
		this->stop();

		Node::_deactivate();
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

		//if( m_resourceMovieName.to_str() != "Movie205_Corridor_Env" 
		//	&& m_resourceMovieName.to_str() != "Movie205_Corridor_Sledi"
		//	)
		//{
		//	return;
		//}

		//Movie205_Corridor_Env
			//Movie205_Corridor_Sledi
		
		float frameDuration = m_resourceMovie->getFrameDuration();

		float realTiming = _timing * m_speedFactor;

		m_frameTiming += realTiming;

		size_t lastFrame = m_currentFrame;

		size_t frameCount = m_resourceMovie->getFrameCount();

		while( m_frameTiming >= frameDuration )
		{
			m_frameTiming -= frameDuration;

			if( m_reverse == true )
			{
				--m_currentFrame;

				if( m_currentFrame == 0 )
				{
					if( this->getLoop() == false )
					{
						break;
					}
				}
			}
			else
			{
				++m_currentFrame;

				if( m_currentFrame == frameCount )
				{
					if( this->getLoop() == false )
					{
						break;
					}
				}
				else if ( m_currentFrame == frameCount + 1 )
				{
					if( this->getLoop() == true )
					{
						this->updateClipLastFrame_();

						m_currentFrame = 0;
					}
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

		if( m_reverse == true )
		{
			if( m_currentFrame == 0 )
			{
				if( this->getLoop() == false )
				{
					m_frameTiming = 0.f;

					this->end();
				}
			}
		}
		else
		{
			if( m_currentFrame == frameCount )
			{
				if( this->getLoop() == false )
				{
					m_frameTiming = 0.f;

					this->end();
				}
			}
		}
	}
	//////////////////////////////////////////////////////////////////////////
	void Movie::updateCurrentFrame_( size_t _lastFrame, bool _force )
	{
		float frameDuration = m_resourceMovie->getFrameDuration();
		//size_t frameCount = m_resourceMovie->getFrameCount();

		const TVectorMovieLayers & layers = m_resourceMovie->getLayers();

		for( TVectorMovieLayers::const_iterator
			it = layers.begin(),
			it_end = layers.end();
		it != it_end;
		++it )
		{
			const MovieLayer & layer = *it;

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

			if( m_currentFrame > _lastFrame )
			{
				if( indexOut < _lastFrame || indexIn > m_currentFrame )
				{
					continue;
				}
			}
			else
			{
				if( indexIn > m_currentFrame )
				{
					continue;
				}
			}
			
			Node * node = m_nodies[layer.index];

			MovieFrameSource frame;

			if( m_currentFrame >= indexOut || m_currentFrame < indexIn )
			{
				if( m_resourceMovie->getFrameLast( layer, frame ) == false )
				{
					MENGE_LOG_ERROR("Movie.updateCurrentFrame_: '%s' frame last incorect '%s'"
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

						if( animatable->isPlay() == true && this->isPlay() == true )
						{
							animatable->stop();

							float timing = (indexOut - indexIn) * frameDuration;
							animatable->setTiming( timing );
						}
					}

					continue;
				}
			}
			else if( m_currentFrame >= indexIn && m_currentFrame < indexOut )
			{
				if( m_resourceMovie->getFrame( layer, m_currentFrame - indexIn, frame ) == false )
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
							//animatable->setSpeedFactor( m_speedFactor );							

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
	void Movie::updateTiming_()
	{
		float frameDuration = m_resourceMovie->getFrameDuration();
		//size_t frameCount = m_resourceMovie->getFrameCount();

		const TVectorMovieLayers & layers = m_resourceMovie->getLayers();

		for( TVectorMovieLayers::const_iterator
			it = layers.begin(),
			it_end = layers.end();
		it != it_end;
		++it )
		{
			const MovieLayer & layer = *it;

			float layerIn = layer.in;
			float layerOut = layer.out;

			size_t indexIn = floorf((layerIn / frameDuration) + 0.5f);
			size_t indexOut = floorf((layerOut / frameDuration) + 0.5f);
		
			Node * node = m_nodies[layer.index];

			MovieFrameSource frame;
						
			if( m_currentFrame >= indexOut || m_currentFrame < indexIn )
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

						float timing = (indexOut - indexIn) * frameDuration;
						animatable->setTiming( timing );
					}

					continue;
				}
			}
			else if( m_currentFrame >= indexIn && m_currentFrame < indexOut )
			{
				if( m_resourceMovie->getFrame( layer, m_currentFrame - indexIn, frame ) == false )
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

						float timing = (m_currentFrame - indexIn) * frameDuration + m_frameTiming;
						animatable->setTiming( timing );
					}
				}
			}			

			this->updateFrame2D_( layer, node, frame );
		}
	}
	//////////////////////////////////////////////////////////////////////////
	void Movie::updateClipLastFrame_()
	{
		float frameDuration = m_resourceMovie->getFrameDuration();
		//size_t frameCount = m_resourceMovie->getFrameCount();

		const TVectorMovieLayers & layers = m_resourceMovie->getLayers();

		for( TVectorMovieLayers::const_iterator
			it = layers.begin(),
			it_end = layers.end();
		it != it_end;
		++it )
		{
			const MovieLayer & layer = *it;

			float layerIn = layer.in;
			float layerOut = layer.out;

			size_t indexIn = floorf((layerIn / frameDuration) + 0.5f);
			size_t indexOut = floorf((layerOut / frameDuration) + 0.5f);

			if( indexOut < m_currentFrame )
			{
				continue;
			}

			Node * node = m_nodies[layer.index];

			MovieFrameSource frame;

			if( m_currentFrame > indexIn )
			{
				if( layer.internal == false )
				{
					node->localHide( true );
				}
			}	
		}
	}
	//////////////////////////////////////////////////////////////////////////
	void Movie::createCamera3D_()
	{
		if( m_resourceMovie->hasCamera3D() == false )
		{
			return;
		}

		ConstString c_layerCamera("movieCamera");

		m_renderCamera3D = NodeManager::get()
			->createNodeT<Camera3D>( c_layerCamera, Consts::get()->c_Camera3D, Consts::get()->c_builtin_empty );

		const MovieLayerCamera3D & camera3D = m_resourceMovie->getCamera3D();

		m_renderCamera3D->setLocalPosition( camera3D.cameraPosition );
		m_renderCamera3D->setCameraInterest( camera3D.cameraInterest );
		m_renderCamera3D->setCameraFOV( camera3D.cameraFOV );
		m_renderCamera3D->setCameraAspect( camera3D.cameraAspect );

		Viewport vp;
		vp.begin.x = 0.f;
		vp.begin.y = 0.f;

		vp.end.x = 1280;
		vp.end.y = 800;

		m_renderCamera3D->setViewport( vp );

		this->addChildren( m_renderCamera3D );
	}
	//////////////////////////////////////////////////////////////////////////
	void Movie::_setSpeedFactor( float _factor )
	{
		if( this->isCompile() == false )
		{
			MENGE_LOG_ERROR( "Warning: Movie::_setSpeedFactor not compile '%s'"
				, m_name.c_str() 
				);	
			
			return;
		}
		
		const TVectorMovieLayers & layers = m_resourceMovie->getLayers();

		for( TVectorMovieLayers::const_iterator
			it = layers.begin(),
			it_end = layers.end();
		it != it_end;
		++it )
		{
			const MovieLayer & layer = *it;

			if( layer.animatable == false )
			{
				continue;
			}
			
			Node * node = m_nodies[layer.index];
			
			Animatable * animatable = dynamic_cast<Animatable *>(node);
			animatable->setSpeedFactor( _factor );
		}
	}
	//////////////////////////////////////////////////////////////////////////
	void Movie::_setReverse( bool _reverse )
	{
		if( this->isCompile() == false )
		{
			return;
		}

		this->updateReverse_( _reverse );
	}
	//////////////////////////////////////////////////////////////////////////
	void Movie::updateReverse_( bool _reverse )
	{
		const TVectorMovieLayers & layers = m_resourceMovie->getLayers();

		for( TVectorMovieLayers::const_iterator
			it = layers.begin(),
			it_end = layers.end();
		it != it_end;
		++it )
		{
			const MovieLayer & layer = *it;

			if( layer.animatable == false )
			{
				continue;
			}
			
			Node * node = m_nodies[layer.index];

			Animatable * animatable = dynamic_cast<Animatable *>(node);
			animatable->setReverse( _reverse );
		}
	}
	//////////////////////////////////////////////////////////////////////////
	void Movie::_interrupt( size_t _enumerator )
	{

	}
}