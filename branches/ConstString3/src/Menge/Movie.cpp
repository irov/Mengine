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
#	include "MovieInternalObject.h"

#	include "SoundEmitter.h"
#	include "NodeManager.h"

#	include "LogEngine.h"

#	include "Consts.h"

#	include "pybind/system.hpp"
#	include "pybind/extract.hpp"

namespace Menge
{
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
		this->callEvent( EVENT_MOVIE_END, "(OiO)", this->getEmbed(), _enumerator, pybind::get_bool(false) );
	}
	//////////////////////////////////////////////////////////////////////////
	void Movie::_end( size_t _enumerator )
	{
		this->stopAnimation_();

		this->callEvent( EVENT_MOVIE_END, "(OiO)", this->getEmbed(), _enumerator, pybind::get_bool(true) );
	}
	//////////////////////////////////////////////////////////////////////////
	void Movie::updateFrameNode_( const MovieLayer & _layer, Node * _node, const MovieFrameSource & _frame )
	{
		_node->setTransformation( 
			_frame.position, 
			_frame.anchorPoint, 
			_frame.scale, 
			_frame.rotation 
			);
			
		if( _layer.movie == false )
		{
			_node->setPersonalAlpha( _frame.opacity );
		}
		else
		{
			_node->setLocalAlpha( _frame.opacity );
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

			this->updateFrameNode_( layer, node, frame );

			if( fabsf(layer.in) <= 0.001f )
			{
				node->localHide(false);
			}
		}

		this->updateTiming_();
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

			this->updateFrameNode_( layer, node, frame );

			if( fabsf(layer.out - out) <= 0.001f )
			{
				node->localHide(false);
			}
		}
		//this->updateTiming_();
	}
	//////////////////////////////////////////////////////////////////////////
	void Movie::addMovieNode_( const MovieLayer & _layer, Node * _node )
	{		
		m_nodies[_layer.index] = _node;

		this->addChildren( _node );
	}
	//////////////////////////////////////////////////////////////////////////
	Node * Movie::getMovieSlot( const ConstString & _name ) const
	{
		if( this->isCompile() == false )
		{
			MENGE_LOG_ERROR("Movie %s invalid getMovieSlot: not compile"
				, m_name.c_str()
				);

			return NULL;
		}

		TMapMovieSlot::const_iterator it_found = m_slots.find( _name );

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
	bool Movie::hasMovieSlot( const ConstString & _name ) const
	{
		if( this->isCompile() == false )
		{
			MENGE_LOG_ERROR("Movie %s invalid hasMovieSlot: not compile"
				, m_name.c_str()
				);

			return NULL;
		}

		TMapMovieSlot::const_iterator it_found = m_slots.find( _name );

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
	PyObject * Movie::findInternalObject_( const ConstString & _resource ) const
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

		const ConstString & internalGroup = resourceInternalObject->getInternalGroup();
		const ConstString & internalName = resourceInternalObject->getInternalName();		
		
		PyObject * py_object = 0;
		this->askEvent( py_object, EVENT_MOVIE_GET_INTERNAL, "(ss)", internalGroup.c_str(), internalName.c_str() );

		resourceInternalObject->decrementReference();

		if( py_object == NULL )
		{
			MENGE_LOG_ERROR("Movie: '%s' can't find internal object '%s:%s'"
				, m_name.c_str()
				, internalGroup.c_str()
				, internalName.c_str()				
				);

			return NULL;
		}

		return py_object;
	}
	//////////////////////////////////////////////////////////////////////////
	bool Movie::createMovieSlot_( const MovieLayer & _layer )
	{
		Node * layer_slot = NodeManager::get()
			->createNodeT<Node>( _layer.name, Consts::get()->c_Node, Consts::get()->c_Node );

		layer_slot->enable();
		layer_slot->localHide(true);

		m_slots.insert( std::make_pair(_layer.name, layer_slot) );

		this->addMovieNode_( _layer, layer_slot );
		
		return true;
	}
	//////////////////////////////////////////////////////////////////////////
	bool Movie::createMovieNullObject_( const MovieLayer & _layer )
	{
		Node * layer_slot = NodeManager::get()
			->createNodeT<Node>( _layer.name, Consts::get()->c_Node, Consts::get()->c_Node );

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

		if( layer_sprite->compile() == false )
		{
			MENGE_LOG_ERROR("Movie: '%s' can't compile sprite '%s'"
				, m_name.c_str()
				, _layer.name.c_str()
				);

			layer_sprite->destroy();

			return false;
		}

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

		if( layer_animation->compile() == false )
		{
			MENGE_LOG_ERROR("Movie: '%s' can't compile animation '%s'"
				, m_name.c_str()
				, _layer.name.c_str()
				);

			layer_animation->destroy();

			return false;
		}

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

		if( layer_movie->compile() == false )
		{
			MENGE_LOG_ERROR("Movie: '%s' can't compile movie '%s'"
				, m_name.c_str()
				, _layer.name.c_str()
				);

			layer_movie->destroy();

			return false;
		}

		layer_movie->localHide( true );
		layer_movie->setParentMovie( true );

		this->addMovieNode_( _layer, layer_movie );

		return true;
	}
	//////////////////////////////////////////////////////////////////////////
	bool Movie::createMovieInternalObject_( const MovieLayer & _layer )
	{
		MovieInternalObject * movie_internal = NodeManager::get()
			->createNodeT<MovieInternalObject>( _layer.name, Consts::get()->c_MovieInternalObject, Consts::get()->c_Image );

		movie_internal->setMovie( this );

		PyObject * py_object = this->findInternalObject_( _layer.source );

		movie_internal->setInternalObject( py_object );

		if( movie_internal->compile() == false )
		{
			MENGE_LOG_ERROR("Movie: '%s' can't compile InternalObject '%s'"
				, m_name.c_str()
				, _layer.name.c_str()
				);

			movie_internal->destroy();

			return false;
		}

		movie_internal->localHide(true);

		this->addMovieNode_( _layer, movie_internal );

		return true;
	}
	//////////////////////////////////////////////////////////////////////////
	bool Movie::createMovieVideo_( const MovieLayer & _layer )
	{
		Video * layer_video = NodeManager::get()
			->createNodeT<Video>( _layer.name, Consts::get()->c_Video, Consts::get()->c_Image );

		layer_video->setVideoResource( _layer.source );

		layer_video->setLoop( true );				

		if( layer_video->compile() == false )
		{
			MENGE_LOG_ERROR("Movie: '%s' can't compile video '%s'"
				, m_name.c_str()
				, _layer.name.c_str()
				);

			layer_video->destroy();

			return false;
		}

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

		if( layer_sound->compile() == false )
		{
			MENGE_LOG_ERROR("Movie: '%s' can't compile sound '%s'"
				, m_name.c_str()
				, _layer.name.c_str()
				);

			layer_sound->destroy();

			return false;
		}

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
	void Movie::updateCamera_()
	{
		const TVectorMovieLayers & layers = m_resourceMovie->getLayers();

		for( TVectorMovieLayers::const_iterator 
			it = layers.begin(),
			it_end = layers.end();
		it != it_end;
		++it )
		{
			const MovieLayer & layer = *it;
			
			Node * node = m_nodies[layer.index];

			if( layer.threeD == true )
			{
				node->setRenderCamera( m_renderCamera3D );
			}
		}
	}
	//////////////////////////////////////////////////////////////////////////
	void Movie::updateStartInterval_()
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
			animatable->setStartInterval( layer.startInterval );
		}
	}
	//////////////////////////////////////////////////////////////////////////
	void Movie::setupParent_()
	{
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
				//this->addChildren( node );
			}
			else
			{
				Node * node_parent = m_nodies[layer.parent];

				if( node_parent == NULL )
				{
					MENGE_LOG_ERROR("Movie::updateParent_ %s invalid parent node %d"
						, this->getName().c_str()
						, layer.parent
						);

					continue;
				}

				node->setRelationTransformation( node_parent );	
			}
		}
	}
	//////////////////////////////////////////////////////////////////////////
	void Movie::removeParent_()
	{
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
				//this->addChildren( node );
			}
			else
			{
				node->setRelationTransformation( 0 );	
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
		this->setupParent_();
		this->updateCamera_();
		this->updateStartInterval_();

		return true;
	}
	//////////////////////////////////////////////////////////////////////////
	void Movie::_deactivate()
	{
		this->stop();

		this->removeParent_();

		Node::_deactivate();
	}
	//////////////////////////////////////////////////////////////////////////
	void Movie::_setEventListener( PyObject * _embed )
	{
		Node::_setEventListener(_embed);

		Eventable::registerEvent( EVENT_MOVIE_GET_INTERNAL, ("onMovieGetInternal"), _embed );
		Eventable::registerEvent( EVENT_MOVIE_HIDE_INTERNAL, ("onMovieHideInternal"), _embed );
		Eventable::registerEvent( EVENT_MOVIE_ACTIVATE_INTERNAL, ("onMovieActivateInternal"), _embed );
		Eventable::registerEvent( EVENT_MOVIE_DEACTIVATE_INTERNAL, ("onMovieDeactivateInternal"), _embed );
		//Eventable::registerEvent( EVENT_MOVIE_APPLY_INTERNAL_TRANSFORMATION, ("onMovieApplyInternalTransformation"), _embed );
		//Eventable::registerEvent( EVENT_MOVIE_APPLY_INTERNAL_OPACITY, ("onMovieApplyInternalOpacity"), _embed );

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
			
			Node * node = m_nodies[layer.index];			MovieFrameSource frame;

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
			}
			else if( m_currentFrame >= indexIn && m_currentFrame < indexOut )
			{
				if( m_resourceMovie->getFrame( layer, m_currentFrame - indexIn, frame ) == false )
				{
					continue;
				}

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
							if( _lastFrame > indexIn )
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
							if( indexOut - 1 > _lastFrame )
							{
								size_t frame = (indexOut - 1) - _lastFrame + m_currentFrame;
								float timing = frame * frameDuration + m_frameTiming;

								animatable->setTiming( -timing );
							}
							else
							{
								size_t frame = m_currentFrame - indexIn;
								float timing = frame * frameDuration + m_frameTiming;

								animatable->setTiming( -timing );
							}
						}
						//animatable->update(realTiming);
					}
				}
			}

			this->updateFrameNode_( layer, node, frame );
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
						
			if( m_currentFrame >= indexOut )
			{
				if( m_resourceMovie->getFrameLast( layer, frame ) == false )
				{
					MENGE_LOG_ERROR("Movie: '%s' frame last incorect '%s'"
						, m_name.c_str()
						, layer.name.c_str()
						);

					continue;
				}

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
			}
			else if( m_currentFrame < indexIn )
			{
				if( m_resourceMovie->getFrameFirst( layer, frame ) == false )
				{
					MENGE_LOG_ERROR("Movie: '%s' frame first incorrect '%s'"
						, m_name.c_str()
						, layer.name.c_str()
						);

					continue;
				}

				node->localHide(true);

				if( layer.animatable == true )
				{
					Animatable * animatable = dynamic_cast<Animatable *>(node);

					float timing = (indexOut - indexIn) * frameDuration;
					animatable->setTiming( timing );
				}
			}
			else if( m_currentFrame >= indexIn && m_currentFrame < indexOut )
			{
				if( m_resourceMovie->getFrame( layer, m_currentFrame - indexIn, frame ) == false )
				{
					continue;
				}
	
				//printf("Movie %s enable %f %d\n", m_name.c_str(), m_timing, layer.index);
				node->localHide(false);

				if( layer.animatable == true )
				{
					Animatable * animatable = dynamic_cast<Animatable *>(node);

					float timing = (m_currentFrame - indexIn) * frameDuration + m_frameTiming;
					animatable->setTiming( timing );
				}
			}			

			this->updateFrameNode_( layer, node, frame );
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
				node->localHide( true );
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