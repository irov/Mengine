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
        , m_playIterator(0)
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

		m_currentFrame = (size_t)floorf( (_timing / frameDuration) );
		m_frameTiming = _timing - m_currentFrame * frameDuration;

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

        //if( m_resourceMovieName != "Movie103_Courtyard_EnvVeronika_run" 
        //    && m_resourceMovieName != "Movie103_Courtyard_veronika_01"
        //    //&& m_resourceMovieName != "Movie401_StreetToChurch_WallZOOM_otv"
        //    )
        //{
        //    return false;
        //}

        m_frameTiming = 0.f;

		if( m_reverse == true )
		{
			size_t frameCount = m_resourceMovie->getFrameCount();

            this->updateForwardFrame_( frameCount, m_currentFrame );
		}
		else
		{
            this->updateForwardFrame_( 0, m_currentFrame );            
		}

        m_playIterator = this->getPlayCount();

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

		EVENTABLE_CALL(this, EVENT_MOVIE_END)( "(OiO)", this->getEmbed(), _enumerator, pybind::get_bool(false) );
	}
	//////////////////////////////////////////////////////////////////////////
	void Movie::_end( size_t _enumerator )
	{
		this->stopAnimation_();

		EVENTABLE_CALL(this, EVENT_MOVIE_END)( "(OiO)", this->getEmbed(), _enumerator, pybind::get_bool(true) );
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

		this->setTiming( 0.f );
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
		
		float duration = m_resourceMovie->getDuration();
		float frameDuration = m_resourceMovie->getFrameDuration();

		this->setTiming( duration - frameDuration );
	}
	//////////////////////////////////////////////////////////////////////////
	void Movie::addMovieNode_( const MovieLayer & _layer, Node * _node )
	{		
		m_nodies[_layer.index] = _node;

		this->addChildren( _node );
	}
	//////////////////////////////////////////////////////////////////////////
	Node * Movie::getMovieNode_( const MovieLayer & _layer ) const
	{		
		return m_nodies[_layer.index];
	}
	//////////////////////////////////////////////////////////////////////////
	Node * Movie::getMovieParent_( const MovieLayer & _layer ) const
	{
		return m_nodies[_layer.parent];
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

			Node * node = this->getMovieNode_( layer );

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

			Node * node = this->getMovieNode_( layer );

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
		//this->askEvent( py_object, EVENT_MOVIE_GET_INTERNAL, "(ss)", internalGroup.c_str(), internalName.c_str() );

		EVENTABLE_ASK(this, EVENT_MOVIE_GET_INTERNAL)( py_object, (PyObject*)0, "(ss)", internalGroup.c_str(), internalName.c_str() );

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

		//layer_slot->enable();
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

		if( _layer.blendingMode == Consts::get()
			->c_BlendingModeAdd )
		{
			layer_sprite->setBlendAdd( true );
		}
		else if( _layer.blendingMode != Consts::get()
			->c_BlendingModeNormal )
		{
			MENGE_LOG_ERROR("Movie: '%s'  sprite  blending mode not supported '%s'"
				, m_name.c_str()
				, _layer.name.c_str()
				);
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

		const mt::vec2f& size = resource->getMaxSize();
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

		//layer_animation->setLoop( true );
		layer_animation->setStartInterval( _layer.startInterval );
        layer_animation->setPlayCount( _layer.playCount );

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
		//layer_movie->setLoop( true );
		layer_movie->setStartInterval( _layer.startInterval );
        layer_movie->setPlayCount( _layer.playCount );

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
		//layer_video->setLoop( true );
		layer_video->setStartInterval( _layer.startInterval );
        layer_video->setPlayCount( _layer.playCount );

		if( layer_video->compile() == false )
		{
			MENGE_LOG_ERROR("Movie: '%s' can't compile video '%s'"
				, m_name.c_str()
				, _layer.name.c_str()
				);

			layer_video->destroy();

			return false;
		}
		
		if( _layer.blendingMode == Consts::get()
			->c_BlendingModeAdd )
		{
			layer_video->setBlendAdd( true );
		}
		else if( _layer.blendingMode != Consts::get()
			->c_BlendingModeNormal )
		{
			MENGE_LOG_ERROR("Movie: '%s' video blending mode not supported '%s'"
				, m_name.c_str()
				, _layer.name.c_str()
				);
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
		//layer_sound->setLoop( true );
		layer_sound->setStartInterval( _layer.startInterval );
        layer_sound->setPlayCount( _layer.playCount );

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
        layer_particles->setPlayCount( _layer.playCount );

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
			MENGE_LOG_ERROR("Movie::_compile: '%s' can't setup resource '%s'"
				, this->getName().c_str()
				, m_resourceMovieName.c_str()
				);

			return false;
		}

		size_t maxLayerIndex = m_resourceMovie->getMaxLayerIndex();

		m_nodies.resize( maxLayerIndex + 1, (Node *)0 );

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
				|| resourceType == Consts::get()->c_ResourceImageInAtlasDefault
				|| resourceType == Consts::get()->c_ResourceImageInAtlasCombineRGBAndAlpha
				|| resourceType == Consts::get()->c_ResourceImageCombineRGBAndAlpha )
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

		this->setupParent_();

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
			
			Node * node = this->getMovieNode_( layer );

			if( layer.threeD == true )
			{
				node->setRenderCamera( m_renderCamera3D );
			}
		}
	}
	//////////////////////////////////////////////////////////////////////////
	void Movie::setupParent_()
	{
        float frameDuration = m_resourceMovie->getFrameDuration();
        
		const TVectorMovieLayers & layers = m_resourceMovie->getLayers();

		for( TVectorMovieLayers::const_iterator 
			it = layers.begin(),
			it_end = layers.end();
		it != it_end;
		++it )
		{
			const MovieLayer & layer = *it;

			Node * node = this->getMovieNode_( layer );
                        
            MovieFrameSource frame;
            if( m_resourceMovie->getFrame( layer, 0, frame ) == false )
            {
                continue;
            }

            this->updateFrameNode_( layer, node, frame );

			if( layer.parent == 0 )
			{
				//this->addChildren( node );
			}
			else
			{
				Node * node_parent = this->getMovieParent_( layer );

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

			Node * node = this->getMovieNode_( layer );

			if( layer.parent == 0 )
			{
				//this->addChildren( node );

				continue;
			}

			Node * parent = node->getParent();
			node->setRelationTransformation( parent );
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
			
			Node * node = this->getMovieNode_( layer );

			Animatable * animatable = dynamic_cast<Animatable *>(node);

			if( animatable->isPlay() == true )
			{
				//printf("Movie %s stop[end] animatable %s\n"
				//	, m_name.c_str()
				//	, node->getName().c_str()
				//	);

				animatable->stop();

				//float movie_timing = this->getTiming();

				//float animatable_timing = movie_timing - layer.in;

				//animatable->setTiming( animatable_timing );
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

			Node * node = this->getMovieNode_( layer );

			Animatable * animatable = dynamic_cast<Animatable *>(node);
			
			animatable->setTiming( 0.f );
		}
	}
	//////////////////////////////////////////////////////////////////////////
	void Movie::_release()
	{	
		this->removeParent_();

		const TVectorMovieLayers & layers = m_resourceMovie->getLayers();

		for( TVectorMovieLayers::const_iterator 
			it = layers.begin(),
			it_end = layers.end();
		it != it_end;
		++it )
		{
			const MovieLayer & layer = *it;

			//if( layer.parent != 0 )
			//{
			//	continue;
			//}
			
			Node * node = this->getMovieNode_( layer );

			node->destroy();
		}

		m_nodies.clear();

		m_slots.clear();

		this->destroyCamera3D_();

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
		
		this->updateCamera_();

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

		Eventable::registerEvent( EVENT_MOVIE_GET_INTERNAL, ("onMovieGetInternal"), _embed );
		Eventable::registerEvent( EVENT_MOVIE_HIDE_INTERNAL, ("onMovieHideInternal"), _embed );
		Eventable::registerEvent( EVENT_MOVIE_ACTIVATE_INTERNAL, ("onMovieActivateInternal"), _embed );
		Eventable::registerEvent( EVENT_MOVIE_DEACTIVATE_INTERNAL, ("onMovieDeactivateInternal"), _embed );
		//Eventable::registerEvent( EVENT_MOVIE_APPLY_INTERNAL_TRANSFORMATION, ("onMovieApplyInternalTransformation"), _embed );
		//Eventable::registerEvent( EVENT_MOVIE_APPLY_INTERNAL_OPACITY, ("onMovieApplyInternalOpacity"), _embed );

		Eventable::registerEvent( EVENT_MOVIE_END, ("onMovieEnd"), _embed );
	}
	//////////////////////////////////////////////////////////////////////////
	void Movie::_update( float _current, float _timing )
	{
		if( this->isPlay() == false )
		{
			return;
		}

		if( m_playTime > _current )
		{
			float deltha = m_playTime - _current;
			_timing -= deltha;
		}

        float frameDuration = m_resourceMovie->getFrameDuration();

		float realTiming = _timing * m_speedFactor;

		m_frameTiming += realTiming;

		size_t frameCount = m_resourceMovie->getFrameCount();


        if( m_reverse == true )
        {
            this->updateBackward_();
        }
        else
        {
            this->updateForward_();
        }
	}
    //////////////////////////////////////////////////////////////////////////
    void Movie::updateForward_()
    {
        float frameDuration = m_resourceMovie->getFrameDuration();        
        size_t frameCount = m_resourceMovie->getFrameCount();

        size_t lastFrame = m_currentFrame;

        bool needUpdate = false;

        if( m_currentFrame != frameCount )
        {
            while( m_frameTiming >= frameDuration )
            {
                m_frameTiming -= frameDuration;

                ++m_currentFrame;

                needUpdate = true;

                if ( m_currentFrame == frameCount )
                {
                    this->updateForwardFrame_( lastFrame, frameCount );

                    needUpdate = false;

                    if( this->getLoop() == false && --m_playIterator == 0 )
                    {
                        lastFrame = frameCount;
                        m_currentFrame = frameCount;

                        this->end();                            

                        break;
                    }
                    else
                    {
                        this->setTiming( 0.f );

                        lastFrame = m_currentFrame;
                    }
                }	
            }
        }
        else
        {
            this->end();
        }

        if( needUpdate == true )
        {
            this->updateForwardFrame_( lastFrame, m_currentFrame );
        }
    }
    //////////////////////////////////////////////////////////////////////////
    void Movie::updateBackward_()
    {
        float frameDuration = m_resourceMovie->getFrameDuration();
        float duration = m_resourceMovie->getDuration();
        size_t frameCount = m_resourceMovie->getFrameCount();

        size_t lastFrame = m_currentFrame;
        
        if( m_currentFrame != 0 )
        {
            while( m_frameTiming >= frameDuration )
            {
                m_frameTiming -= frameDuration;
                
                --m_currentFrame;

                if( m_currentFrame == 0 )
                {
                    this->updateBackwardFrame_( lastFrame, 0 );

                    if( this->getLoop() == false && --m_playIterator == 0 )
                    {           
                        lastFrame = 0;
                        m_currentFrame = 0;

                        this->end();

                        break;
                    }
                    else
                    {                      
                        this->setTiming( duration );

                        lastFrame = m_currentFrame;
                    }
                }
            }
        }

        this->updateBackwardFrame_( lastFrame, m_currentFrame );

        if( m_currentFrame == 0 )
        {
            if( this->getLoop() == true )
            {
                //Nothing
            }
            else
            {
                this->end();
            }
        }
    }
	//////////////////////////////////////////////////////////////////////////
	void Movie::updateForwardFrame_( size_t _beginFrame, size_t _endFrame )
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

			size_t indexIn = (size_t)((layerIn / frameDuration) + 0.5f);
			size_t indexOut = (size_t)((layerOut / frameDuration) + 0.5f);

            if( indexIn > _endFrame || indexOut < _beginFrame )
            {
                continue;
            }

			Node * node = this->getMovieNode_( layer );			
			
			if( _beginFrame <= indexIn && _endFrame >= indexIn && _endFrame < indexOut )
			{
                MovieFrameSource frame;
                if( m_resourceMovie->getFrame( layer, _endFrame - indexIn, frame ) == false )
                {
                    continue;
                }

                this->updateFrameNode_( layer, node, frame );

				node->localHide(false);

				if( layer.animatable == true )
				{
					Animatable * animatable = dynamic_cast<Animatable *>(node);

                    if( animatable->isPlay() == true )
                    {
                        animatable->stop();
                    }
                        
                    size_t frame = _endFrame - indexIn;
                    float timing = frame * frameDuration;

                    animatable->setTiming( timing );

                    float playTime = this->getPlayTime();
                    animatable->play( playTime );                                       
				}
			}
			else if( _endFrame >= indexOut && _beginFrame >= indexIn && _beginFrame < indexOut )
			{
                //printf("Movie %s enable %f %d\n", m_name.c_str(), m_timing, layer.index);
				node->localHide(true);

				if( layer.animatable == true )
				{
					Animatable * animatable = dynamic_cast<Animatable *>(node);

					if( animatable->isPlay() == true )
					{
                        animatable->stop();

                        float timing = (indexOut - indexIn) * frameDuration;
                        animatable->setTiming( timing );
					}
				}
			}
            else if( _beginFrame > indexIn && _endFrame >= indexIn && _endFrame < indexOut )
            {
                MovieFrameSource frame;
                if( m_resourceMovie->getFrame( layer, _endFrame - indexIn, frame ) == false )
                {
                    continue;
                }

                this->updateFrameNode_( layer, node, frame );
            }			
		}
	}
    //////////////////////////////////////////////////////////////////////////
    void Movie::updateBackwardFrame_( size_t _beginFrame, size_t _endFrame )
    {
        float frameDuration = m_resourceMovie->getFrameDuration();

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

            size_t indexIn = (size_t)((layerIn / frameDuration) + 0.5f);
            size_t indexOut = (size_t)((layerOut / frameDuration) + 0.5f);

            if( indexIn > _beginFrame || indexOut < _endFrame )
            {
                continue;
            }
            
            Node * node = this->getMovieNode_( layer );			

            if( _beginFrame >= indexOut && _endFrame <= indexOut && _endFrame > indexIn )
            {
                MovieFrameSource frame;
                if( m_resourceMovie->getFrame( layer, _endFrame - indexIn, frame ) == false )
                {
                    continue;
                }

                this->updateFrameNode_( layer, node, frame );

                node->localHide(false);

                if( layer.animatable == true )
                {
                    Animatable * animatable = dynamic_cast<Animatable *>(node);

                    if( animatable->isPlay() == true )
                    {
                        animatable->stop();
                    }

                    size_t frame = _endFrame - indexIn;
                    float timing = frame * frameDuration;

                    animatable->setTiming( timing );

                    float playTime = this->getPlayTime();
                    animatable->play( playTime );                                       
                }
            }
            else if( _endFrame <= indexIn && _beginFrame <= indexOut && _beginFrame > indexIn )
            {
                //printf("Movie %s enable %f %d\n", m_name.c_str(), m_timing, layer.index);
                node->localHide(true);

                if( layer.animatable == true )
                {
                    Animatable * animatable = dynamic_cast<Animatable *>(node);

                    if( animatable->isPlay() == true )
                    {
                        animatable->stop();

                        float timing = (indexOut - indexIn) * frameDuration;
                        animatable->setTiming( timing );
                    }
                }
            }
            else if( _beginFrame < indexOut && _endFrame <= indexOut && _endFrame > indexIn )
            {
                MovieFrameSource frame;
                if( m_resourceMovie->getFrame( layer, _endFrame - indexIn, frame ) == false )
                {
                    continue;
                }

                this->updateFrameNode_( layer, node, frame );
            }			
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

			size_t indexIn = (size_t)((layerIn / frameDuration) + 0.5f);
			size_t indexOut = (size_t)((layerOut / frameDuration) + 0.5f);
		
			Node * node = this->getMovieNode_( layer );

			MovieFrameSource frame;
						
			if( m_currentFrame >= indexOut )
			{
				node->localHide(true);
			}
			else if( m_currentFrame < indexIn )
			{
				node->localHide(true);
			}
			else if( m_currentFrame >= indexIn && m_currentFrame < indexOut )
			{
				if( m_resourceMovie->getFrame( layer, m_currentFrame - indexIn, frame ) == false )
				{
					continue;
				}
	
				node->localHide(false);

				if( layer.animatable == true )
				{
					Animatable * animatable = dynamic_cast<Animatable *>(node);

					float timing = (m_currentFrame - indexIn) * frameDuration + m_frameTiming;
					animatable->setTiming( timing );
				}

                this->updateFrameNode_( layer, node, frame );
			}			
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

			size_t indexIn = (size_t)((layerIn / frameDuration) + 0.5f);
			size_t indexOut = (size_t)((layerOut / frameDuration) + 0.5f);

			if( indexOut < m_currentFrame )
			{
				continue;
			}

			Node * node = this->getMovieNode_( layer );

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
	void Movie::destroyCamera3D_()
	{
		if( m_renderCamera3D == NULL )
		{
			return;
		}

		m_renderCamera3D->destroy();
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
			
			Node * node = this->getMovieNode_( layer );
			
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
			
			Node * node = this->getMovieNode_( layer );

			Animatable * animatable = dynamic_cast<Animatable *>(node);
			animatable->setReverse( _reverse );
		}
	}
	//////////////////////////////////////////////////////////////////////////
	bool Movie::_interrupt( size_t _enumerator )
	{
		return false;
	}
}