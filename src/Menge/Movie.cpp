#	include "Movie.h"

#	include "Interface/ResourceInterface.h"

#	include "ResourceMovie.h"

#	include "ResourceImageDefault.h"
#	include "ResourceInternalObject.h"
#	include "ResourceAnimation.h"
#	include "ResourceImageSolid.h"

#   include "Interface/ApplicationInterface.h"

#	include "Kernel/Layer.h"
#	include "Camera3D.h"

#	include "Sprite.h"
#	include "Animation.h"
#	include "Video.h"
#   include "TextField.h"
#	include "ParticleEmitter.h"
#   include "MovieSlot.h"
#   include "MovieNodeExtra.h"
#   include "MovieEvent.h"
#   include "HotSpotImage.h"
#   include "MovieSceneEffect.h"
#	include "MovieInternalObject.h"

#	include "Kernel/Soundable.h"

#	include "SoundEmitter.h"

#   include "Interface/NodeInterface.h"

#	include "Logger/Logger.h"

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
		, m_renderCamera3D(0)
        , m_parentMovie(false)
	{
	}
	//////////////////////////////////////////////////////////////////////////
	void Movie::setResourceMovieName( const ConstString & _resourceName )
	{
		m_resourceMovieName = _resourceName;
	}
	//////////////////////////////////////////////////////////////////////////
	const ConstString & Movie::getResourceMovieName() const
	{	
		return m_resourceMovieName;
	}
    //////////////////////////////////////////////////////////////////////////
    ResourceMovie * Movie::getResourceMovie() const
    {
        return m_resourceMovie;
    }
	//////////////////////////////////////////////////////////////////////////
	void Movie::setParentMovie( bool _value )
	{
		m_parentMovie = _value;
	}
	//////////////////////////////////////////////////////////////////////////
	bool Movie::isParentMovie() const
	{
		return m_parentMovie;
	}	
	//////////////////////////////////////////////////////////////////////////
	void Movie::_setTiming( float _timing )
	{
		if( this->isActivate() == false )
		{
			LOGGER_ERROR(m_serviceProvider)( "Movie.setTiming: '%s' not activate"
				, m_name.c_str()
				);

			return;
		}

        //const ConstString & group = m_resourceMovie->getGroup();

        //if( group != "Hint" )
        //{
        //    return;
        //}
	
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
			LOGGER_ERROR(m_serviceProvider)( "Movie.getTiming: '%s' not compile"
				, m_name.c_str()
				);

			return 0.f;
		}


		float frameDuration = m_resourceMovie->getFrameDuration();

		float timing = m_currentFrame * frameDuration + m_frameTiming;

		return timing;
	}
	//////////////////////////////////////////////////////////////////////////
	bool Movie::_play( float _time )
	{
        (void)_time;

		if( this->isActivate() == false )
		{
			return false;
		}

        //if( m_resourceMovieName == "MovieCutScene_CutScene1"
        //    //&& m_resourceMovieName != "Movie103_Courtyard_veronika_01"
        //    //&& m_resourceMovieName != "Movie401_StreetToChurch_WallZOOM_otv"
        //    )
        //{
        //    printf("!");
        //}
        
        //m_frameTiming = 0.f;

        //const ConstString & group = m_resourceMovie->getGroup();

        //if( group != "Inventory" )
        //{
        //    return false;
        //}

        if( m_reverse == true )
        {
            size_t frameCount = m_resourceMovie->getFrameCount();

            this->updateForwardFrame_( _time, frameCount, m_currentFrame );
            //this->updateAnimatablePlay_();
        }
        else
        {
            //this->updateForwardFrame_( _time, 0, m_currentFrame );
            this->updateAnimatablePlay_();
        }

        m_playIterator = this->getPlayCount();

		return true;
	}
	//////////////////////////////////////////////////////////////////////////
	bool Movie::_restart( float _time, size_t _enumerator )
	{
        (void)_enumerator;

		if( this->isActivate() == false )
		{
			return false;
		}

        if( m_reverse == true )
        {
            size_t frameCount = m_resourceMovie->getFrameCount();

            this->updateForwardFrame_( _time, frameCount, m_currentFrame );
            //this->updateAnimatablePlay_();
        }
        else
        {
            //this->updateForwardFrame_( _time, 0, m_currentFrame );
            this->updateAnimatablePlay_();
        }

        m_playIterator = this->getPlayCount();

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
        const mt::vec3f coordinate(0.f, 0.f, 0.f);

		_node->setTransformation( 
			_frame.position, 
			_frame.anchorPoint, 
            coordinate,
			_frame.scale, 
			_frame.rotation 
			);
		
		if( _layer.isMovie() == false )
		{
			_node->setPersonalAlpha( _frame.opacity );
		}
		else
		{
			_node->setLocalAlpha( _frame.opacity );
		}

		if( _layer.isAudio() == true )
		{
			Soundable * sounding = dynamic_cast<Soundable *>( _node );

			sounding->setVolume( _frame.volume );
		}
	}
	//////////////////////////////////////////////////////////////////////////
	void Movie::_setFirstFrame()
	{
		if( this->isActivate() == false )
		{
			LOGGER_ERROR(m_serviceProvider)("Movie %s invalid setFirstFrame: not activate"
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
			LOGGER_ERROR(m_serviceProvider)("Movie %s invalid setLastFrame: not activate"
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
        Nodies ns;
        ns.node = _node;
        ns.child = (_layer.parent != -1);

		m_nodies[_layer.index] = ns;
	}
	//////////////////////////////////////////////////////////////////////////
	Node * Movie::getMovieNode_( const MovieLayer & _layer ) const
	{	
        const Nodies & ns = m_nodies[_layer.index];

		return ns.node;
	}
	//////////////////////////////////////////////////////////////////////////
	Node * Movie::getMovieParent_( const MovieLayer & _layer ) const
	{
        const Nodies & ns = m_nodies[_layer.parent];

		return ns.node;
	}    
	//////////////////////////////////////////////////////////////////////////
	Node * Movie::getMovieSlot( const ConstString & _name ) const
	{
		if( this->isCompile() == false )
		{
			LOGGER_ERROR(m_serviceProvider)("Movie.getMovieSlot %s invalid not compile"
				, m_name.c_str()
				);

			return NULL;
		}

		TMapMovieSlots::const_iterator it_found = m_slots.find( _name );

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

			if( layer.isMovie() == false || layer.isSubMovie() == true )
			{
				continue;
			}

			Node * node = this->getMovieNode_( layer );

			Movie * movie = dynamic_cast<Movie *>(node);

			Node * slot = movie->getMovieSlot( _name );

			if( slot == NULL )
			{
				continue;
			}

			return slot;
		}

		LOGGER_ERROR(m_serviceProvider)("Movie::getMovieSlot: %s not found slot %s"
			, m_name.c_str()
			, _name.c_str()
			);

		return NULL;
	}
	//////////////////////////////////////////////////////////////////////////
	bool Movie::hasMovieSlot( const ConstString & _name ) const
	{
		if( this->isCompile() == false )
		{
			LOGGER_ERROR(m_serviceProvider)("Movie.hasMovieSlot %s invalid not compile"
				, m_name.c_str()
				);

			return NULL;
		}

		TMapMovieSlots::const_iterator it_found = m_slots.find( _name );

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

			if( layer.isMovie() == false || layer.isSubMovie() == true )
			{
				continue;
			}

			Node * node = this->getMovieNode_( layer );

			Movie * movie = dynamic_cast<Movie *>(node);

			if( movie->hasMovieSlot( _name ) == false )
			{
				continue;
			}

			return true;
		}
				
		return false;
	}
    //////////////////////////////////////////////////////////////////////////
    Movie * Movie::getSubMovie( const ConstString & _name ) const
    {
        if( this->isCompile() == false )
        {
            LOGGER_ERROR(m_serviceProvider)("Movie::getSubMovie %s invalid not compile"
                , m_name.c_str()
                );

            return NULL;
        }

        TMapSubMovies::const_iterator it_found = m_subMovies.find( _name );

        if( it_found != m_subMovies.end() )
        {	
            Movie * submovie = it_found->second;

            return submovie;
        }

        const TVectorMovieLayers & layers = m_resourceMovie->getLayers();

        for( TVectorMovieLayers::const_iterator
            it = layers.begin(),
            it_end = layers.end();
        it != it_end;
        ++it )
        {
            const MovieLayer & layer = *it;

            if( layer.isMovie() == false || layer.isSubMovie() == true )
            {
                continue;
            }

            Node * node = this->getMovieNode_( layer );

            Movie * movie = dynamic_cast<Movie *>(node);

            Movie * submovie = movie->getSubMovie( _name );

            if( submovie == NULL )
            {
                continue;
            }

            return submovie;
        }

        LOGGER_ERROR(m_serviceProvider)("Movie::getSubMovie: %s not found submovie %s"
            , m_name.c_str()
            , _name.c_str()
            );

        return NULL;
    }
    //////////////////////////////////////////////////////////////////////////
    bool Movie::hasSubMovie( const ConstString & _name ) const
    {
        if( this->isCompile() == false )
        {
            LOGGER_ERROR(m_serviceProvider)("Movie.hasSubMovie %s invalid not compile"
                , m_name.c_str()
                );

            return NULL;
        }

        TMapSubMovies::const_iterator it_found = m_subMovies.find( _name );

        if( it_found != m_subMovies.end() )
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

            if( layer.isMovie() == false || layer.isSubMovie() == true )
            {
                continue;
            }

            Node * node = this->getMovieNode_( layer );

            Movie * movie = dynamic_cast<Movie *>(node);

            if( movie->hasSubMovie( _name ) == false )
            {
                continue;
            }

            return true;
        }

        return false;
    }
    //////////////////////////////////////////////////////////////////////////
    HotSpotImage * Movie::getSocket( const ConstString & _name ) const
    {
        if( this->isCompile() == false )
        {
            LOGGER_ERROR(m_serviceProvider)("Movie.getSocket %s invalid not compile"
                , m_name.c_str()
                );

            return NULL;
        }

        TMapSockets::const_iterator it_found = m_sockets.find( _name );

        if( it_found != m_sockets.end() )
        {	
            HotSpotImage * slot = it_found->second;

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

            if( layer.isMovie() == false || layer.isSubMovie() == true )
            {
                continue;
            }

            Node * node = this->getMovieNode_( layer );

            Movie * movie = dynamic_cast<Movie *>(node);

            HotSpotImage * socket = movie->getSocket( _name );

            if( socket == NULL )
            {
                continue;
            }

            return socket;
        }

        LOGGER_ERROR(m_serviceProvider)("Movie::getSocket: %s not found slot %s"
            , m_name.c_str()
            , _name.c_str()
            );

        return NULL;
    }
    //////////////////////////////////////////////////////////////////////////
    bool Movie::hasSocket( const ConstString & _name ) const
    {
        if( this->isCompile() == false )
        {
            LOGGER_ERROR(m_serviceProvider)("Movie.hasSocket %s invalid not compile"
                , m_name.c_str()
                );

            return NULL;
        }

        TMapSockets::const_iterator it_found = m_sockets.find( _name );

        if( it_found != m_sockets.end() )
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

            if( layer.isMovie() == false || layer.isSubMovie() == true )
            {
                continue;
            }

            Node * node = this->getMovieNode_( layer );

            Movie * movie = dynamic_cast<Movie *>(node);

            if( movie->hasSocket( _name ) == false )
            {
                continue;
            }

            return true;
        }

        return false;
    }
    //////////////////////////////////////////////////////////////////////////
    bool Movie::setMovieEvent( const ConstString & _name, PyObject * _cb ) const
    {
        if( this->isCompile() == false )
        {
            LOGGER_ERROR(m_serviceProvider)("Movie.setMovieEvent %s invalid not compile"
                , m_name.c_str()
                );

            return false;
        }

        TMapMovieEvent::const_iterator it_found = m_events.find( _name );

        if( it_found != m_events.end() )
        {	
            MovieEvent * event = it_found->second;

            event->setEvent( _cb );

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

            if( layer.isMovie() == false || layer.isSubMovie() == true )
            {
                continue;
            }

            Node * node = this->getMovieNode_( layer );

            Movie * movie = dynamic_cast<Movie *>(node);

            if( movie->setMovieEvent( _name, _cb ) == true )
            {
                return true;
            }
        }

        LOGGER_ERROR(m_serviceProvider)("Movie::setMovieEvent: %s not found event %s"
            , m_name.c_str()
            , _name.c_str()
            );

        return false;
    }
    //////////////////////////////////////////////////////////////////////////
    bool Movie::hasMovieEvent( const ConstString & _name ) const
    {
        if( this->isCompile() == false )
        {
            LOGGER_ERROR(m_serviceProvider)("Movie.hasMovieEvent %s invalid not compile"
                , m_name.c_str()
                );

            return false;
        }

        TMapMovieEvent::const_iterator it_found = m_events.find( _name );

        if( it_found != m_events.end() )
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

            if( layer.isMovie() == false || layer.isSubMovie() == true )
            {
                continue;
            }

            Node * node = this->getMovieNode_( layer );

            Movie * movie = dynamic_cast<Movie *>(node);

            if( movie->hasMovieEvent( _name ) == true )
            {
                return true;
            }
        }

        return false;
    }
	//////////////////////////////////////////////////////////////////////////
	PyObject * Movie::findInternalObject_( const ConstString & _resource ) const
	{
		ResourceInternalObject * resourceInternalObject = RESOURCE_SERVICE(m_serviceProvider)
			->getResourceT<ResourceInternalObject>( _resource );

		if( resourceInternalObject == NULL )
		{
			LOGGER_ERROR(m_serviceProvider)("Movie: '%s' can't find internal resource '%s'"
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
			LOGGER_ERROR(m_serviceProvider)("Movie: '%s' can't find internal object '%s:%s'"
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
		MovieSlot * layer_slot = NODE_SERVICE(m_serviceProvider)
			->createNodeT<MovieSlot>( CONST_STRING(m_serviceProvider, MovieSlot) );

        layer_slot->setName( _layer.name );

		//layer_slot->enable();
		layer_slot->localHide(true);
        

		m_slots.insert( std::make_pair(_layer.name, layer_slot) );

		this->addMovieNode_( _layer, layer_slot );
		
		return true;
	}
    //////////////////////////////////////////////////////////////////////////
    bool Movie::createMovieSceneEffect_( const MovieLayer & _layer )
    {
        MovieSceneEffect * sceneeffect_slot = NODE_SERVICE(m_serviceProvider)
            ->createNodeT<MovieSceneEffect>( CONST_STRING(m_serviceProvider, MovieSceneEffect) );

        sceneeffect_slot->setName( _layer.name );

        Layer * layer = this->getLayer();

        sceneeffect_slot->setPropagateNode( layer );

        //layer_slot->enable();
        sceneeffect_slot->localHide(true);

        this->addMovieNode_( _layer, sceneeffect_slot );

        return true;
    }
	//////////////////////////////////////////////////////////////////////////
	bool Movie::createMovieNullObject_( const MovieLayer & _layer )
	{
		Node * layer_slot = NODE_SERVICE(m_serviceProvider)
			->createNodeT<Node>( CONST_STRING(m_serviceProvider, Node) );

        layer_slot->setName( _layer.name );

		layer_slot->localHide(true);

		this->addMovieNode_( _layer, layer_slot );

		return true;
	}
	//////////////////////////////////////////////////////////////////////////
	bool Movie::createMovieImage_( const MovieLayer & _layer )
	{
		Sprite * layer_sprite = NODE_SERVICE(m_serviceProvider)
			->createNodeT<Sprite>( CONST_STRING(m_serviceProvider, Sprite) );

		layer_sprite->setImageResourceName( _layer.source );

        layer_sprite->setName( _layer.name );

		if( layer_sprite->compile() == false )
		{
			LOGGER_ERROR(m_serviceProvider)("Movie: '%s' can't compile sprite '%s'"
				, m_name.c_str()
				, _layer.name.c_str()
				);

			layer_sprite->destroy();

			return false;
		}

		if( _layer.blendingMode == CONST_STRING(m_serviceProvider, BlendingModeAdd) )
		{
			layer_sprite->setBlendAdd( true );
		}
		else if( _layer.blendingMode != CONST_STRING(m_serviceProvider, BlendingModeNormal) )
		{
			LOGGER_ERROR(m_serviceProvider)("Movie: '%s'  sprite  blending mode not supported '%s'"
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
		ResourceImageSolid * resource = RESOURCE_SERVICE(m_serviceProvider)
			->getResourceT<ResourceImageSolid>( _layer.source );

		if( resource == NULL )
		{
			LOGGER_ERROR(m_serviceProvider)("Movie: '%s' can't compile sprite '%s' imageSolid resource = NULL"
				, m_name.c_str()
				, _layer.name.c_str()
				);

			return false;
		}

		Sprite * layer_sprite = NODE_SERVICE(m_serviceProvider)
			->createNodeT<Sprite>( CONST_STRING(m_serviceProvider, Sprite) );

		layer_sprite->setImageResourceName( CONST_STRING(m_serviceProvider, WhitePixel) );

        layer_sprite->setName( _layer.name );

		const ColourValue& color = resource->getColor();
		layer_sprite->setLocalColor( color );

		const mt::vec2f& size = resource->getSize();
		layer_sprite->setSpriteSize( size );

        if( _layer.blendingMode == CONST_STRING(m_serviceProvider, BlendingModeAdd) )
        {
            layer_sprite->setBlendAdd( true );
        }
        else if( _layer.blendingMode != CONST_STRING(m_serviceProvider, BlendingModeNormal) )
        {
            LOGGER_ERROR(m_serviceProvider)("Movie: '%s' blending mode not supported '%s'"
                , m_name.c_str()
                , _layer.name.c_str()
                );
        }

		if( layer_sprite->compile() == false )
		{
			LOGGER_ERROR(m_serviceProvider)("Movie: '%s' can't compile sprite '%s'"
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
    bool Movie::createMovieSocket_( const MovieLayer & _layer )
    {
        HotSpotImage * layer_hotspotimage = NODE_SERVICE(m_serviceProvider)
            ->createNodeT<HotSpotImage>( CONST_STRING(m_serviceProvider, Sprite) );

        layer_hotspotimage->setResourceHIT( _layer.source );

        layer_hotspotimage->setName( _layer.name );

        if( layer_hotspotimage->compile() == false )
        {
            LOGGER_ERROR(m_serviceProvider)("Movie: '%s' can't compile HotSpotImage '%s'"
                , m_name.c_str()
                , _layer.name.c_str()
                );

            layer_hotspotimage->destroy();

            return false;
        }

        layer_hotspotimage->localHide(true);

        m_sockets.insert( std::make_pair(_layer.name, layer_hotspotimage) );

        this->addMovieNode_( _layer, layer_hotspotimage );

        return true;
    }
	//////////////////////////////////////////////////////////////////////////
	bool Movie::createMovieAnimation_( const MovieLayer & _layer )
	{
		Animation * layer_animation = NODE_SERVICE(m_serviceProvider)
			->createNodeT<Animation>( CONST_STRING(m_serviceProvider, Animation) );

		layer_animation->setAnimationResource( _layer.source );
        layer_animation->setName( _layer.name );

		layer_animation->setIntervalStart( _layer.startInterval );
        layer_animation->setPlayCount( _layer.playCount );
        //layer_animation->setLoop( _layer.loop );

        if( _layer.blendingMode == CONST_STRING(m_serviceProvider, BlendingModeAdd) )
        {
            layer_animation->setBlendAdd( true );
        }
        else if( _layer.blendingMode != CONST_STRING(m_serviceProvider, BlendingModeNormal) )
        {
            LOGGER_ERROR(m_serviceProvider)("Movie: '%s' blending mode not supported '%s'"
                , m_name.c_str()
                , _layer.name.c_str()
                );
        }

		if( layer_animation->compile() == false )
		{
			LOGGER_ERROR(m_serviceProvider)("Movie: '%s' can't compile animation '%s'"
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
		Movie * layer_movie = NODE_SERVICE(m_serviceProvider)
			->createNodeT<Movie>( CONST_STRING(m_serviceProvider, Movie) );

		layer_movie->setResourceMovieName( _layer.source );				
		
		layer_movie->setIntervalStart( _layer.startInterval );
        
        layer_movie->setPlayCount( _layer.playCount );
        //layer_movie->setLoop( _layer.loop );

		if( layer_movie->compile() == false )
		{
			LOGGER_ERROR(m_serviceProvider)("Movie: '%s' can't compile movie '%s'"
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
    bool Movie::createMovieSubMovie_( const MovieLayer & _layer )
    {
        Movie * layer_movie = NODE_SERVICE(m_serviceProvider)
            ->createNodeT<Movie>( CONST_STRING(m_serviceProvider, Movie) );

        layer_movie->setResourceMovieName( _layer.source );				

        layer_movie->setIntervalStart( _layer.startInterval );

        layer_movie->setPlayCount( _layer.playCount );
        //layer_movie->setLoop( _layer.loop );

        if( layer_movie->compile() == false )
        {
            LOGGER_ERROR(m_serviceProvider)("Movie: '%s' can't compile movie '%s'"
                , m_name.c_str()
                , _layer.name.c_str()
                );

            layer_movie->destroy();

            return false;
        }

        layer_movie->localHide( true );
        layer_movie->setParentMovie( true );

        m_subMovies.insert( std::make_pair(_layer.name, layer_movie) );

        this->addMovieNode_( _layer, layer_movie );

        return true;
    }
	//////////////////////////////////////////////////////////////////////////
	bool Movie::createMovieInternalObject_( const MovieLayer & _layer )
	{
		MovieInternalObject * movie_internal = NODE_SERVICE(m_serviceProvider)
			->createNodeT<MovieInternalObject>( CONST_STRING(m_serviceProvider, MovieInternalObject) );

		movie_internal->setMovie( this );

		PyObject * py_object = this->findInternalObject_( _layer.source );

		movie_internal->setInternalObject( py_object );

		if( movie_internal->compile() == false )
		{
			LOGGER_ERROR(m_serviceProvider)("Movie: '%s' can't compile InternalObject '%s'"
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
		Video * layer_video = NODE_SERVICE(m_serviceProvider)
			->createNodeT<Video>( CONST_STRING(m_serviceProvider, Video) );

		layer_video->setVideoResource( _layer.source );
        layer_video->setName( _layer.name );

		layer_video->setIntervalStart( _layer.startInterval );
        layer_video->setPlayCount( _layer.playCount );
        //layer_video->setLoop( _layer.loop );

		if( layer_video->compile() == false )
		{
			LOGGER_ERROR(m_serviceProvider)("Movie: '%s' can't compile video '%s'"
				, m_name.c_str()
				, _layer.name.c_str()
				);

			layer_video->destroy();

			return false;
		}
		
		if( _layer.blendingMode == CONST_STRING(m_serviceProvider, BlendingModeAdd) )
		{
			layer_video->setBlendAdd( true );
		}
		else if( _layer.blendingMode != CONST_STRING(m_serviceProvider, BlendingModeNormal) )
		{
			LOGGER_ERROR(m_serviceProvider)("Movie: '%s' blending mode not supported '%s'"
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
		SoundEmitter * layer_sound = NODE_SERVICE(m_serviceProvider)
			->createNodeT<SoundEmitter>( CONST_STRING(m_serviceProvider, SoundEmitter) );

		layer_sound->setSoundResource( _layer.source );
        layer_sound->setName( _layer.name );
		layer_sound->setIntervalStart( _layer.startInterval );
        layer_sound->setPlayCount( _layer.playCount );
        //layer_sound->setLoop( _layer.loop );

		if( layer_sound->compile() == false )
		{
			LOGGER_ERROR(m_serviceProvider)("Movie: '%s' can't compile sound '%s'"
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
    bool Movie::createMovieText_( const MovieLayer & _layer )
    {
        TextField * layer_text = NODE_SERVICE(m_serviceProvider)
            ->createNodeT<TextField>( CONST_STRING(m_serviceProvider, TextField) );

        layer_text->setName( _layer.name );
        layer_text->setTextByKey( _layer.name ); //Name = TextID

        if( layer_text->compile() == false )
        {
            LOGGER_ERROR(m_serviceProvider)("Movie: '%s' can't compile text '%s'"
                , m_name.c_str()
                , _layer.name.c_str()
                );

            layer_text->destroy();

            return false;
        }

        layer_text->localHide( true );

        this->addMovieNode_( _layer, layer_text );

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    bool Movie::createMovieEvent_( const MovieLayer & _layer )
    {
        MovieEvent * layer_event = NODE_SERVICE(m_serviceProvider)
            ->createNodeT<MovieEvent>( CONST_STRING(m_serviceProvider, MovieEvent) );

        layer_event->setName( _layer.name );
        layer_event->setResourceMovie( m_resourceMovie );

        if( layer_event->compile() == false )
        {
            LOGGER_ERROR(m_serviceProvider)("Movie: '%s' can't compile event '%s'"
                , m_name.c_str()
                , _layer.name.c_str()
                );

            layer_event->destroy();

            return false;
        }

        layer_event->localHide( true );

        m_events.insert( std::make_pair(_layer.name, layer_event) );

        this->addMovieNode_( _layer, layer_event );

        return true;
    }
	//////////////////////////////////////////////////////////////////////////
	bool Movie::createMovieEmitterContainer_( const MovieLayer & _layer )
	{
		ParticleEmitter * layer_particles = NODE_SERVICE(m_serviceProvider)
			->createNodeT<ParticleEmitter>( CONST_STRING(m_serviceProvider, ParticleEmitter) );

		layer_particles->setResource( _layer.source );
        layer_particles->setName( _layer.name );

        layer_particles->setIntervalStart( _layer.startInterval );        
        layer_particles->setPlayCount( _layer.playCount );
        //layer_particles->setLoop( _layer.loop );

        if( layer_particles->compile() == false )
        {
            LOGGER_ERROR(m_serviceProvider)("Movie: '%s' can't compile ParticleEmitter '%s'"
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

		m_resourceMovie = RESOURCE_SERVICE(m_serviceProvider)
			->getResourceT<ResourceMovie>( m_resourceMovieName );

		if( m_resourceMovie == 0 )
		{
			LOGGER_ERROR(m_serviceProvider)("Movie::_compile: '%s' can't setup resource '%s'"
				, this->getName().c_str()
				, m_resourceMovieName.c_str()
				);

			return false;
		}

		size_t maxLayerIndex = m_resourceMovie->getMaxLayerIndex();

        Nodies ns;
        ns.child = false;
        ns.node = 0;
		m_nodies.resize( maxLayerIndex + 1, ns );

		this->createCamera3D_();

		const TVectorMovieLayers & layers = m_resourceMovie->getLayers();

		for( TVectorMovieLayers::const_iterator
			it = layers.begin(),
			it_end = layers.end();
		it != it_end;
		++it )
		{
			const MovieLayer & layer = *it;
			
			if ( layer.layerType == CONST_STRING(m_serviceProvider, MovieSlot) )
			{
				if( this->createMovieSlot_( layer ) == false )
				{
					return false;
				}
			}
            else if ( layer.layerType == CONST_STRING(m_serviceProvider, MovieSceneEffect) )
            {
                if( this->createMovieSceneEffect_( layer ) == false )
                {
                    return false;
                }
            }
            else if ( layer.layerType == CONST_STRING(m_serviceProvider, MovieText) )
            {
                if( this->createMovieText_( layer ) == false )
                {
                    return false;
                }
            }
			else if ( layer.layerType == CONST_STRING(m_serviceProvider, MovieNullObject) )
			{
				if( this->createMovieNullObject_( layer ) == false )
				{
					return false;
				}
			}
			else if( layer.layerType == CONST_STRING(m_serviceProvider, Image) )
			{
				if( this->createMovieImage_( layer ) == false )
				{
					return false;
				}
			}
			else if( layer.layerType == CONST_STRING(m_serviceProvider, SolidSprite) )
			{
				if( this->createMovieImageSolid_( layer ) == false )
				{
					return false;
				}
			}
            else if( layer.layerType == CONST_STRING(m_serviceProvider, MovieSocket) )
            {
                if( this->createMovieSocket_( layer ) == false )
                {
                    return false;
                }
            }
			else if( layer.layerType == CONST_STRING(m_serviceProvider, Animation) )
			{
				if( this->createMovieAnimation_( layer ) == false )
				{
					return false;
				}
			}
			else if( layer.layerType == CONST_STRING(m_serviceProvider, Movie) )
			{
				if( this->createMovieMovie_( layer ) == false )
				{
					return false;
				}
			}
            else if( layer.layerType == CONST_STRING(m_serviceProvider, SubMovie) )
            {
                if( this->createMovieSubMovie_( layer ) == false )
                {
                    return false;
                }
            }
			else if( layer.layerType == CONST_STRING(m_serviceProvider, MovieInternalObject) )
			{				
				if( this->createMovieInternalObject_( layer ) == false )
				{
					return false;
				}
			}
			else if( layer.layerType == CONST_STRING(m_serviceProvider, Video) )
			{
				if( this->createMovieVideo_( layer ) == false )
				{
					return false;
				}
			}
			else if( layer.layerType == CONST_STRING(m_serviceProvider, Sound) )
			{
				if( this->createMovieSound_( layer ) == false )
				{
					return false;
				}
			}
			else if( layer.layerType == CONST_STRING(m_serviceProvider, ParticleEmitter) )
			{
				if( this->createMovieEmitterContainer_( layer ) == false )
				{
					return false;
				}
			}
            else if( layer.layerType == CONST_STRING(m_serviceProvider, MovieEvent) )
            {
                if( this->createMovieEvent_( layer ) == false )
                {
                    return false;
                }
            }
			else
			{
				LOGGER_ERROR(m_serviceProvider)("Movie: '%s' can't create layer_node '%s' type '%s'"
					, m_name.c_str()
					, layer.source.c_str()
					, layer.layerType.c_str()
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

			if( layer.isThreeD() == true )
			{
				node->setRenderCamera( m_renderCamera3D );
			}
		}
	}
	//////////////////////////////////////////////////////////////////////////
	void Movie::setupParent_()
	{
        for( TVectorNodies::reverse_iterator
            it = m_nodies.rbegin(),
            it_end = m_nodies.rend();
        it != it_end;
        ++it )
        {
            const Nodies & ns = *it;

            if( ns.node == NULL )
            {
                continue;
            }

            if( ns.child == false )
            {
                continue;
            }

            this->addChildren( ns.node );
        }

        //float frameDuration = m_resourceMovie->getFrameDuration();

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
                 
            if( layer.parent != 0 && layer.parent != -1 )
			{
				Node * node_parent = this->getMovieParent_( layer );

				if( node_parent == NULL )
				{
					LOGGER_ERROR(m_serviceProvider)("Movie::updateParent_ %s invalid parent node %d"
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
			LOGGER_ERROR(m_serviceProvider)("Movie::stopAnimation_ %s is not compile"
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

			if( layer.isAnimatable() == false )
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

			if( layer.isAnimatable() == false )
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

		//const TVectorMovieLayers & layers = m_resourceMovie->getLayers();

		for( TVectorNodies::const_iterator 
			it = m_nodies.begin(),
			it_end = m_nodies.end();
		it != it_end;
		++it )
		{
			const Nodies & ns = *it;

			if( ns.node == 0 )
			{
				continue;
			}

			ns.node->destroy();
		}

		m_nodies.clear();

		m_slots.clear();
        m_events.clear();

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

		this->registerEvent( EVENT_MOVIE_GET_INTERNAL, ("onMovieGetInternal"), _embed );
		this->registerEvent( EVENT_MOVIE_HIDE_INTERNAL, ("onMovieHideInternal"), _embed );
		this->registerEvent( EVENT_MOVIE_ACTIVATE_INTERNAL, ("onMovieActivateInternal"), _embed );
		this->registerEvent( EVENT_MOVIE_DEACTIVATE_INTERNAL, ("onMovieDeactivateInternal"), _embed );
		//Eventable::registerEvent( EVENT_MOVIE_APPLY_INTERNAL_TRANSFORMATION, ("onMovieApplyInternalTransformation"), _embed );
		//Eventable::registerEvent( EVENT_MOVIE_APPLY_INTERNAL_OPACITY, ("onMovieApplyInternalOpacity"), _embed );

		this->registerEvent( EVENT_MOVIE_END, ("onMovieEnd"), _embed );
	}
	//////////////////////////////////////////////////////////////////////////
	void Movie::_update( float _current, float _timing )
	{
		if( this->isPlay() == false )
		{
			return;
		}

        //if( m_resourceMovieName == "MovieCutScene_CutScene1"
        //    //&& m_resourceMovieName != "Movie103_Courtyard_veronika_01"
        //    //&& m_resourceMovieName != "Movie401_StreetToChurch_WallZOOM_otv"
        //    )
        //{
        //    printf("!");
        //}

		if( m_playTime > _current )
		{
			float deltha = m_playTime - _current;
			_timing -= deltha;
		}

        //float frameDuration = m_resourceMovie->getFrameDuration();

		float realTiming = _timing * m_speedFactor;

		m_frameTiming += realTiming;
        
        if( m_reverse == true )
        {
            this->updateBackward_();
        }
        else
        {
            this->updateForward_( _current );
        }
	}
    //////////////////////////////////////////////////////////////////////////
    void Movie::updateForward_( float _time )
    {
        float frameDuration = m_resourceMovie->getFrameDuration();
        size_t frameCount = m_resourceMovie->getFrameCount();

        if( m_intervalEnd > 0.f )
        {
            frameCount = (size_t)((m_intervalStart + m_intervalEnd) / frameDuration + 0.5f);
        }

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
                    this->updateForwardFrame_( _time, lastFrame, frameCount );

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
                        //this->setTiming( 0.f );
                        this->setTiming( m_frameTiming );

                        lastFrame = m_currentFrame;

                        needUpdate = true;
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
            //printf("Movie::update %s %d:%d\n"
            //    , this->getName().c_str()
            //    , lastFrame
            //    , m_currentFrame
            //    );

            this->updateForwardFrame_( _time, lastFrame, m_currentFrame );
        }
    }
    //////////////////////////////////////////////////////////////////////////
    void Movie::updateBackward_()
    {
        float frameDuration = m_resourceMovie->getFrameDuration();
        float duration = m_resourceMovie->getDuration();
        //size_t frameCount = m_resourceMovie->getFrameCount();

        size_t lastFrame = m_currentFrame;
        
        bool needUpdate = false;

        if( m_currentFrame != 0 )
        {
            while( m_frameTiming >= frameDuration )
            {
                m_frameTiming -= frameDuration;
                
                --m_currentFrame;

                needUpdate = true;

                if( m_currentFrame == 0 )
                {
                    this->updateBackwardFrame_( lastFrame, 0 );

                    needUpdate = false;

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

                        needUpdate = true;
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
            this->updateBackwardFrame_( lastFrame, m_currentFrame );
        }
    }
	//////////////////////////////////////////////////////////////////////////
	void Movie::updateForwardFrame_( float _time, size_t _beginFrame, size_t _endFrame )
	{
		const TVectorMovieLayers & layers = m_resourceMovie->getLayers();

		for( TVectorMovieLayers::const_iterator
			it = layers.begin(),
			it_end = layers.end();
		it != it_end;
		++it )
		{
			const MovieLayer & layer = *it;

            if( layer.isSubMovie() == true )
            {
                continue;
            }
            
            Node * node = this->getMovieNode_( layer );

            if( layer.isNode() == true )
            {
                this->updateForwardFrameNode_( _time, _beginFrame, _endFrame, layer, node );
            }
            else if( layer.isExtra() == true )
            {
                 MovieNodeExtra * extra = dynamic_cast<MovieNodeExtra *>(node);

                 extra->movieForwardUpdate( _time, _beginFrame, _endFrame, layer );
            }
        }
    }
    //////////////////////////////////////////////////////////////////////////
    void Movie::updateForwardFrameNode_( float _time, size_t _beginFrame, size_t _endFrame, const MovieLayer & _layer, Node * _node )
    {
        (void)_time;

        float layerIn = _layer.in;
        float layerOut = _layer.out;

        float frameDuration = m_resourceMovie->getFrameDuration();

        size_t indexIn = (size_t)((layerIn / frameDuration) + 0.5f);
        size_t indexOut = (size_t)((layerOut / frameDuration) + 0.5f);

        if( indexIn > _endFrame || indexOut < _beginFrame )
        {
            return;
        }

        if( _beginFrame < indexIn && _endFrame >= indexIn && _endFrame < indexOut )
        {
            MovieFrameSource frame;
            if( m_resourceMovie->getFrame( _layer, _endFrame - indexIn, frame ) == false )
            {
                return;
            }

            this->updateFrameNode_( _layer, _node, frame );

            _node->localHide( false );

            if( _layer.isAnimatable() == true )
            {
                Animatable * animatable = dynamic_cast<Animatable *>(_node);

                if( animatable->isPlay() == true )
                {
                    animatable->stop();
                }
                //else
                //{

                //}

                size_t frame = _endFrame - indexIn;
                float timing = frame * frameDuration;

                animatable->setTiming( timing );

                //float playTime = this->getPlayTime();



                //float animatablePlayTime = playTime + movieTiming;

                //float movieTiming = _endFrame * frameDuration;

                //animatable->play( playTime + movieTiming );

                float movieTiming = this->getTiming();
                animatable->play( movieTiming );

                //printf("Movie play %f %s\n"
                //    , _time
                //    , node->getName().c_str()
                //    );
            }
        }
        else if( _endFrame >= indexOut && _beginFrame >= indexIn && _beginFrame < indexOut )
        {
            //printf("Movie %s enable %f %d\n", m_name.c_str(), m_timing, layer.index);
            _node->localHide( true );

            if( _layer.isAnimatable() == true )
            {
                Animatable * animatable = dynamic_cast<Animatable *>(_node);

                if( animatable->isPlay() == true )
                {
                    animatable->stop();

                    ///Test TestTestTestTest
                    float timing = (indexOut - indexIn) * frameDuration;                        
                    animatable->setTiming( timing );
                }
            }
        }
        else if( _beginFrame > indexIn && _endFrame >= indexIn && _endFrame < indexOut )
        {
            MovieFrameSource frame;
            if( m_resourceMovie->getFrame( _layer, _endFrame - indexIn, frame ) == false )
            {
                return;
            }

            this->updateFrameNode_( _layer, _node, frame );
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

            if( layer.isSubMovie() == true )
            {
                continue;
            }

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

                if( layer.isAnimatable() == true )
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

                if( layer.isAnimatable() == true )
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
    void Movie::_localHide( bool _value )
    {
        this->hide( _value );
    }
    //////////////////////////////////////////////////////////////////////////
    void Movie::_setPersonalColor( const ColourValue& _color )
    {
        this->setLocalColor( _color );
    }
    //////////////////////////////////////////////////////////////////////////
    void Movie::_setPersonalAlpha( float _alpha )
    {
        this->setLocalAlpha( _alpha );
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

			if( m_currentFrame >= indexIn && m_currentFrame < indexOut )
			{
                MovieFrameSource frame;
				if( m_resourceMovie->getFrame( layer, m_currentFrame - indexIn, frame ) == false )
				{
					continue;
				}
	
				node->localHide(false);

				if( layer.isAnimatable() == true )
				{
					Animatable * animatable = dynamic_cast<Animatable *>(node);

					float timing = (m_currentFrame - indexIn) * frameDuration + m_frameTiming;
					animatable->setTiming( timing );
				}

                this->updateFrameNode_( layer, node, frame );
			}
            else
            {
                node->localHide(true);
            }
		}
	}
    //////////////////////////////////////////////////////////////////////////
    void Movie::updateAnimatablePlay_()
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

            if( layer.isSubMovie() == true )
            {
                continue;
            }

            float layerIn = layer.in;
            float layerOut = layer.out;

            size_t indexIn = (size_t)((layerIn / frameDuration) + 0.5f);
            size_t indexOut = (size_t)((layerOut / frameDuration) + 0.5f);

            if( indexIn > m_currentFrame || indexOut < m_currentFrame )
            {
                continue;
            }

            Node * node = this->getMovieNode_( layer );			

            if( m_currentFrame >= indexIn && m_currentFrame < indexOut )
            {                
                size_t layerFrame = m_currentFrame - indexIn;

                MovieFrameSource frame;
                if( m_resourceMovie->getFrame( layer, layerFrame, frame ) == false )
                {
                    return;
                }

                this->updateFrameNode_( layer, node, frame );

                node->localHide( false );

                if( layer.isAnimatable() == true )
                {
                    Animatable * animatable = dynamic_cast<Animatable *>(node);

                    if( animatable->isPlay() == true )
                    {
                        animatable->stop();
                    }

                    float timing = layerFrame * frameDuration;

                    animatable->setTiming( timing );

                    float movieTiming = this->getTiming();
                    animatable->play( movieTiming );
                }
            }
            else
            {
                //printf("Movie %s enable %f %d\n", m_name.c_str(), m_timing, layer.index);
                node->localHide( true );

                if( layer.isAnimatable() == true )
                {
                    Animatable * animatable = dynamic_cast<Animatable *>(node);

                    if( animatable->isPlay() == true )
                    {
                        animatable->stop();

                        ///Test TestTestTestTest
                        float timing = (indexOut - indexIn) * frameDuration;                        
                        animatable->setTiming( timing );
                    }
                }
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

        m_renderCamera3D = NODE_SERVICE(m_serviceProvider)
            ->createNodeT<Camera3D>( CONST_STRING(m_serviceProvider, Camera3D) );

        const ConstString & name = this->getName();
        m_renderCamera3D->setName( name );

		const MovieLayerCamera3D & camera3D = m_resourceMovie->getCamera3D();

		m_renderCamera3D->setLocalPosition( camera3D.cameraPosition );
		m_renderCamera3D->setCameraInterest( camera3D.cameraInterest );
		m_renderCamera3D->setCameraFOV( camera3D.cameraFOV );
		m_renderCamera3D->setCameraAspect( camera3D.cameraAspect );

        Viewport vp;
		vp.begin.x = 0.f;
		vp.begin.y = 0.f;

        vp.end.x = camera3D.width;
        vp.end.y = camera3D.height;

		m_renderCamera3D->setViewport( vp );

		this->addChildren( m_renderCamera3D );
        //m_renderCamera3D->setRelationTransformation( NULL );
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
			LOGGER_ERROR(m_serviceProvider)( "Warning: Movie::_setSpeedFactor not compile '%s'"
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

			if( layer.isAnimatable() == false )
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

			if( layer.isAnimatable() == false )
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
        (void)_enumerator;

		return false;
	}
}