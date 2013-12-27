#	include "Movie.h"

#	include "Interface/ResourceInterface.h"

#	include "ResourceMovie.h"

#	include "ResourceImageDefault.h"
#	include "ResourceAnimation.h"
#	include "ResourceImageSolid.h"
#   include "ResourceHIT.h"
#   include "ResourceVideo.h"
#   include "ResourceSound.h"
#   include "ResourceShape.h"
#   include "ResourceEmitterContainer.h"
#	include "ResourceEmitter.h"

#   include "Interface/ApplicationInterface.h"

#	include "Kernel/Layer.h"
#	include "Camera3D.h"
#	include "RenderViewport.h"

#	include "Sprite.h"
#	include "Mesh2D.h"
#	include "Animation.h"
#	include "Video.h"
#   include "TextField.h"
#	include "ParticleEmitter.h"
#   include "MovieSlot.h"
#   include "MovieNodeExtra.h"
#   include "MovieEvent.h"
#   include "HotSpotImage.h"
#   include "HotSpotShape.h"
#   include "MovieSceneEffect.h"
#	include "MovieInternalObject.h"

#	include "Kernel/Soundable.h"

#	include "SoundEmitter.h"

#   include "Interface/NodeInterface.h"

#	include "Logger/Logger.h"

#	include "Consts.h"

#	include "Math/angle.h"

#	include "pybind/system.hpp"
#	include "pybind/extract.hpp"

namespace Menge
{
	//////////////////////////////////////////////////////////////////////////
	Movie::Movie()
		: m_frameTiming(0.f)
		, m_currentFrame(0)        
		, m_renderCamera3D(nullptr)
		, m_renderViewport(nullptr)
        , m_parentMovie(false)
	{	
    }
    //////////////////////////////////////////////////////////////////////////
    void Movie::setResourceMovie( ResourceMovie * _resourceMovie )
    {
        if( m_resourceMovie == _resourceMovie )
        {
            return;
        }

        m_resourceMovie = _resourceMovie;

        this->releaseLayers_();

        if( m_resourceMovie != nullptr )
        {
            if( this->createLayers_() == false )
            {
                LOGGER_ERROR(m_serviceProvider)("Movie.setResourceMovie: '%s' can't create layer '%s'"
                    , m_name.c_str()
                    , m_resourceMovie->getName().c_str()
                    );

                m_resourceMovie = nullptr;
                return;
            }            
        }  

        this->recompile();      
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
        //return false;
        (void)_time;

		if( this->isActivate() == false )
		{
			return false;
		}

		//printf("Movie::_play %s %f\n"
		//	, this->getName().c_str()
		//	, _time
		//	);

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

		EVENTABLE_CALL(m_serviceProvider, this, EVENT_MOVIE_END)( "(OiO)", this->getEmbed(), _enumerator, pybind::get_bool(false) );
	}
	//////////////////////////////////////////////////////////////////////////
	void Movie::_end( size_t _enumerator )
	{
		this->stopAnimation_();

		EVENTABLE_CALL(m_serviceProvider, this, EVENT_MOVIE_END)( "(OiO)", this->getEmbed(), _enumerator, pybind::get_bool(true) );
	}
	//////////////////////////////////////////////////////////////////////////
	bool Movie::updateFrameNode_( const MovieLayer & _layer, Node * _node, size_t _frameId, bool _interpolate )
	{	
		const MovieFramePackInterfacePtr & framePack = m_resourceMovie->getFramePack();

		MovieFrameSource frame;
		if( _interpolate == true && _layer.immutable == false )
		{
			float frameDuration = m_resourceMovie->getFrameDuration();

			float t = m_frameTiming / frameDuration;

			if( framePack->getLayerFrameInterpolate( _layer.index, _frameId, t, frame ) == false )
			{
				return false;
			}
		}
		else
		{
			if( framePack->getLayerFrame( _layer.index, _frameId, frame ) == false )
			{
				return false;
			}
		}

        const mt::vec3f coordinate(0.f, 0.f, 0.f);

		_node->setTransformation( 
			frame.position, 
			frame.anchorPoint, 
            coordinate,
			frame.scale, 
			frame.rotation 
			);
		
		if( _layer.isMovie() == false )
		{
			_node->setPersonalAlpha( frame.opacity );
		}
		else
		{
			_node->setLocalAlpha( frame.opacity );
		}

		if( _layer.isAudio() == true )
		{
#   ifdef _DEBUG
            if( dynamic_cast<Soundable *>( _node ) == nullptr )
            {
                LOGGER_ERROR(m_serviceProvider)("Movie::updateFrameNode_ %s layer %s is Audio but node is not Soundable %s:%s"
                    , this->getName().c_str()
                    , _layer.name.c_str()
                    , _node->getName().c_str()
                    , _node->getType().c_str()
                    );

                return false;
            }
#   endif

			SoundEmitter * sounding = static_cast<SoundEmitter *>( _node );

			sounding->setVolume( frame.volume );
		}

		if( _layer.isMesh2D() == true )
		{
#   ifdef _DEBUG
			if( dynamic_cast<Mesh2D *>( _node ) == nullptr )
			{
				LOGGER_ERROR(m_serviceProvider)("Movie::updateFrameNode_ %s layer %s is Mesh2D but node is not Mesh2D %s:%s"
					, this->getName().c_str()
					, _layer.name.c_str()
					, _node->getName().c_str()
					, _node->getType().c_str()
					);

				return false;
			}
#   endif

			Mesh2D * mesh2D = static_cast<Mesh2D *>( _node );

			const MovieFrameShape * shape;
			framePack->getLayerShape( _layer.index, _frameId, &shape );

			mesh2D->setFrameShape( shape );
		}

		return true;
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

        m_playIterator = this->getPlayCount();

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
		if( _layer.in < 0.0001f )
		{
			_node->localHide( false );
		}
		else
		{
			_node->localHide( true );
		}

        Nodies ns;
        ns.node = _node;
        ns.child = (_layer.parent != movie_layer_parent_none);

		m_nodies[_layer.index] = ns;
	}
	//////////////////////////////////////////////////////////////////////////
	Node * Movie::getMovieNode_( const MovieLayer & _layer ) const
	{	
        size_t size = m_nodies.size();
        if( _layer.index >= size )
        {
            return nullptr;
        }

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
		if( m_resourceMovie == nullptr )
		{
			LOGGER_ERROR(m_serviceProvider)("Movie.getMovieSlot %s invalid not compile"
				, m_name.c_str()
				);

			return nullptr;
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

            if( node == nullptr )
            {
                continue;
            }

			Movie * movie = dynamic_cast<Movie *>(node);

			Node * slot = movie->getMovieSlot( _name );

			if( slot == nullptr )
			{
				continue;
			}

			return slot;
		}

		LOGGER_ERROR(m_serviceProvider)("Movie::getMovieSlot: %s not found slot %s"
			, m_name.c_str()
			, _name.c_str()
			);

		return nullptr;
	}
	//////////////////////////////////////////////////////////////////////////
	bool Movie::hasMovieSlot( const ConstString & _name ) const
	{
		if( m_resourceMovie == nullptr )
		{
			LOGGER_ERROR(m_serviceProvider)("Movie.hasMovieSlot %s invalid not compile"
				, m_name.c_str()
				);

			return nullptr;
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

            if( node == nullptr )
            {
                continue;
            }

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
        if( m_resourceMovie == nullptr )
        {
            LOGGER_ERROR(m_serviceProvider)("Movie::getSubMovie %s invalid not compile"
                , m_name.c_str()
                );

            return nullptr;
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

            if( node == nullptr )
            {
                continue;
            }

            Movie * movie = dynamic_cast<Movie *>(node);

            Movie * submovie = movie->getSubMovie( _name );

            if( submovie == nullptr )
            {
                continue;
            }

            return submovie;
        }

        LOGGER_ERROR(m_serviceProvider)("Movie::getSubMovie: %s not found submovie %s"
            , m_name.c_str()
            , _name.c_str()
            );

        return nullptr;
    }
    //////////////////////////////////////////////////////////////////////////
    bool Movie::hasSubMovie( const ConstString & _name ) const
    {
        if( m_resourceMovie == nullptr )
        {
            LOGGER_ERROR(m_serviceProvider)("Movie.hasSubMovie %s invalid not compile"
                , m_name.c_str()
                );

            return nullptr;
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

            if( node == nullptr )
            {
                continue;
            }

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
    bool Movie::visitSockets( VisitorMovieSocket * _visitor ) const
    {
        if( m_resourceMovie == nullptr )
        {
            LOGGER_ERROR(m_serviceProvider)("Movie.visitSockets %s invalid not compile"
                , m_name.c_str()
                );

            return false;
        }

        for( TMapSockets::const_iterator
            it = m_sockets.begin(),
            it_end = m_sockets.end();
        it != it_end;
        ++it )
        {
            const ConstString & name = it->first;
            HotSpot * hotspot = it->second;

            _visitor->visitSocket( name, hotspot );
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

            if( node == nullptr )
            {
                continue;
            }

            Movie * movie = dynamic_cast<Movie *>(node);

            if( movie->visitSockets( _visitor ) == false )
            {
                continue;
            }
        }

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    HotSpot * Movie::getSocket( const ConstString & _name ) const
    {
        if( m_resourceMovie == nullptr )
        {
            LOGGER_ERROR(m_serviceProvider)("Movie.getSocket %s invalid not compile %s"
                , m_name.c_str()
                , _name.c_str()
                );

            return nullptr;
        }

        TMapSockets::const_iterator it_found = m_sockets.find( _name );

        if( it_found != m_sockets.end() )
        {	
            HotSpot * slot = it_found->second;

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

            if( node == nullptr )
            {
                continue;
            }

            Movie * movie = dynamic_cast<Movie *>(node);

            HotSpot * socket = movie->getSocket( _name );

            if( socket == nullptr )
            {
                continue;
            }

            return socket;
        }

        LOGGER_ERROR(m_serviceProvider)("Movie::getSocket: %s not found slot %s"
            , m_name.c_str()
            , _name.c_str()
            );

        return nullptr;
    }
    //////////////////////////////////////////////////////////////////////////
    bool Movie::hasSocket( const ConstString & _name ) const
    {
        if( m_resourceMovie == nullptr )
        {
            LOGGER_ERROR(m_serviceProvider)("Movie.hasSocket %s invalid not compile"
                , m_name.c_str()
                );

            return nullptr;
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

            if( node == nullptr )
            {
                continue;
            }

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
        if( m_resourceMovie == nullptr )
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

            if( node == nullptr )
            {
                continue;
            }

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
        if( m_resourceMovie == nullptr )
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

            if( node == nullptr )
            {
                continue;
            }

            Movie * movie = dynamic_cast<Movie *>(node);

            if( movie->hasMovieEvent( _name ) == true )
            {
                return true;
            }
        }

        return false;
    }
    //////////////////////////////////////////////////////////////////////////
    bool Movie::createLayers_()
    {
        size_t maxLayerIndex = m_resourceMovie->getMaxLayerIndex();

        Nodies ns;
        ns.child = false;
        ns.node = 0;
        m_nodies.resize( maxLayerIndex + 1, ns );

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
			else if ( layer.layerType == CONST_STRING(m_serviceProvider, MovieTextCenter) )
			{
				if( this->createMovieTextCenter_( layer ) == false )
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
				if( layer.shape == false )
				{
					if( this->createMovieImage_( layer ) == false )
					{
						return false;
					}
				}
				else
				{
					if( this->createMovieMesh2D_( layer ) == false )
					{
						return false;
					}
				}
            }
            else if( layer.layerType == CONST_STRING(m_serviceProvider, SolidSprite) )
            {
                if( this->createMovieImageSolid_( layer ) == false )
                {
                    return false;
                }
            }
            else if( layer.layerType == CONST_STRING(m_serviceProvider, MovieSocketImage) )
            {
                if( this->createMovieSocketImage_( layer ) == false )
                {
                    return false;
                }
            }
            else if( layer.layerType == CONST_STRING(m_serviceProvider, MovieSocketShape) )
            {
                if( this->createMovieSocketShape_( layer ) == false )
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
                if( this->createMovieParticleEmitter_( layer ) == false )
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

        for( TVectorNodies::reverse_iterator
            it = m_nodies.rbegin(),
            it_end = m_nodies.rend();
        it != it_end;
        ++it )
        {
            const Nodies & ns = *it;

            if( ns.node == nullptr )
            {
                continue;
            }

            if( ns.child == false )
            {
                continue;
            }

            this->addChildren( ns.node );
        }

        this->createCamera3D_();

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    void Movie::releaseLayers_()
    {
        for( TVectorNodies::const_iterator 
            it = m_nodies.begin(),
            it_end = m_nodies.end();
        it != it_end;
        ++it )
        {
            const Nodies & ns = *it;

            if( ns.node == nullptr )
            {
                continue;
            }

            ns.node->destroy();
        }

        m_nodies.clear();

        m_slots.clear();
        m_subMovies.clear();
        m_sockets.clear();
        m_events.clear();
        m_sceneEffects.clear();
        m_internals.clear();

        this->destroyCamera3D_();
    }
	//////////////////////////////////////////////////////////////////////////
	bool Movie::createMovieSlot_( const MovieLayer & _layer )
	{
		MovieSlot * layer_slot = NODE_SERVICE(m_serviceProvider)
			->createNodeT<MovieSlot>( CONST_STRING(m_serviceProvider, MovieSlot) );

        layer_slot->setName( _layer.name );

        layer_slot->setMovieName( m_name );

		m_slots.insert( _layer.name, layer_slot );

		this->addMovieNode_( _layer, layer_slot );
		
		return true;
	}
    //////////////////////////////////////////////////////////////////////////
    bool Movie::createMovieSceneEffect_( const MovieLayer & _layer )
    {
        MovieSceneEffect * sceneeffect_slot = NODE_SERVICE(m_serviceProvider)
            ->createNodeT<MovieSceneEffect>( CONST_STRING(m_serviceProvider, MovieSceneEffect) );

        sceneeffect_slot->setName( _layer.name );

        m_sceneEffects.insert( _layer.name, sceneeffect_slot );

        this->addMovieNode_( _layer, sceneeffect_slot );

        return true;
    }
	//////////////////////////////////////////////////////////////////////////
	bool Movie::createMovieNullObject_( const MovieLayer & _layer )
	{
		Node * layer_slot = NODE_SERVICE(m_serviceProvider)
			->createNodeT<Node>( CONST_STRING(m_serviceProvider, Node) );

        layer_slot->setName( _layer.name );

		this->addMovieNode_( _layer, layer_slot );

		return true;
	}
	//////////////////////////////////////////////////////////////////////////
	bool Movie::createMovieImage_( const MovieLayer & _layer )
	{
		Sprite * layer_sprite = NODE_SERVICE(m_serviceProvider)
			->createNodeT<Sprite>( CONST_STRING(m_serviceProvider, Sprite) );

        ResourceImage * resourceImage = RESOURCE_SERVICE(m_serviceProvider)
            ->getResourceReferenceT<ResourceImage>( _layer.source );

		layer_sprite->setResourceImage( resourceImage );

        layer_sprite->setName( _layer.name );

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

		this->addMovieNode_( _layer, layer_sprite );

		return true;
	}
	//////////////////////////////////////////////////////////////////////////
	bool Movie::createMovieMesh2D_( const MovieLayer & _layer )
	{
		Mesh2D * layer_mesh = NODE_SERVICE(m_serviceProvider)
			->createNodeT<Mesh2D>( CONST_STRING(m_serviceProvider, Mesh2D) );

		ResourceImage * resourceImage = RESOURCE_SERVICE(m_serviceProvider)
			->getResourceReferenceT<ResourceImage>( _layer.source );

		layer_mesh->setResourceImage( resourceImage );

		layer_mesh->setName( _layer.name );

		if( _layer.blendingMode == CONST_STRING(m_serviceProvider, BlendingModeAdd) )
		{
			layer_mesh->setBlendAdd( true );
		}
		else if( _layer.blendingMode != CONST_STRING(m_serviceProvider, BlendingModeNormal) )
		{
			LOGGER_ERROR(m_serviceProvider)("Movie: '%s'  sprite  blending mode not supported '%s'"
				, m_name.c_str()
				, _layer.name.c_str()
				);
		}

		this->addMovieNode_( _layer, layer_mesh );

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

        ResourceImage * resourceImage = RESOURCE_SERVICE(m_serviceProvider)
            ->getResourceReferenceT<ResourceImage>( CONST_STRING(m_serviceProvider, WhitePixel) );

		layer_sprite->setResourceImage( resourceImage );

        layer_sprite->setName( _layer.name );

		const ColourValue& color = resource->getColor();
		layer_sprite->setLocalColor( color );

		const mt::vec2f& size = resource->getSize();
		layer_sprite->setCustomSize( size );

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

		this->addMovieNode_( _layer, layer_sprite );

		return true;
	}
    //////////////////////////////////////////////////////////////////////////
    bool Movie::createMovieSocketImage_( const MovieLayer & _layer )
    {
        HotSpotImage * layer_hotspotimage = NODE_SERVICE(m_serviceProvider)
            ->createNodeT<HotSpotImage>( CONST_STRING(m_serviceProvider, HotSpotImage) );

        ResourceHIT * resourceHIT = RESOURCE_SERVICE(m_serviceProvider)
            ->getResourceReferenceT<ResourceHIT>( _layer.source );

        layer_hotspotimage->setResourceHIT( resourceHIT );

        layer_hotspotimage->setName( _layer.name );

        m_sockets.insert( _layer.name, layer_hotspotimage );
		
        this->addMovieNode_( _layer, layer_hotspotimage );

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    bool Movie::createMovieSocketShape_( const MovieLayer & _layer )
    {
        HotSpotShape * layer_hotspotshape = NODE_SERVICE(m_serviceProvider)
            ->createNodeT<HotSpotShape>( CONST_STRING(m_serviceProvider, HotSpotShape) );

        ResourceShape * resourceShape = RESOURCE_SERVICE(m_serviceProvider)
            ->getResourceReferenceT<ResourceShape>( _layer.source );

        layer_hotspotshape->setResourceShape( resourceShape );

        layer_hotspotshape->setName( _layer.name );

        m_sockets.insert( _layer.name, layer_hotspotshape );

        this->addMovieNode_( _layer, layer_hotspotshape );

        return true;
    }
	//////////////////////////////////////////////////////////////////////////
	bool Movie::createMovieAnimation_( const MovieLayer & _layer )
	{
		Animation * layer_animation = NODE_SERVICE(m_serviceProvider)
			->createNodeT<Animation>( CONST_STRING(m_serviceProvider, Animation) );

        ResourceAnimation * resourceAnimation = RESOURCE_SERVICE(m_serviceProvider)
            ->getResourceReferenceT<ResourceAnimation>( _layer.source );

		layer_animation->setResourceAnimation( resourceAnimation );
        layer_animation->setName( _layer.name );

		layer_animation->setIntervalStart( _layer.startInterval );
        layer_animation->setPlayCount( _layer.playCount );
        layer_animation->setScretch( _layer.scretch );
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

		this->addMovieNode_( _layer, layer_animation );

		return true;
	}
	//////////////////////////////////////////////////////////////////////////
	bool Movie::createMovieMovie_( const MovieLayer & _layer )
	{
		Movie * layer_movie = NODE_SERVICE(m_serviceProvider)
			->createNodeT<Movie>( CONST_STRING(m_serviceProvider, Movie) );

        layer_movie->setName( _layer.name );

        ResourceMovie * resourceMovie = RESOURCE_SERVICE(m_serviceProvider)
            ->getResourceReferenceT<ResourceMovie>( _layer.source );

		layer_movie->setResourceMovie( resourceMovie );
		
		layer_movie->setIntervalStart( _layer.startInterval );
        
        layer_movie->setPlayCount( _layer.playCount );
        layer_movie->setScretch( _layer.scretch );
        //layer_movie->setLoop( _layer.loop );

		layer_movie->setParentMovie( true );

		this->addMovieNode_( _layer, layer_movie );

		return true;
	}
    //////////////////////////////////////////////////////////////////////////
    bool Movie::createMovieSubMovie_( const MovieLayer & _layer )
    {
        Movie * layer_movie = NODE_SERVICE(m_serviceProvider)
            ->createNodeT<Movie>( CONST_STRING(m_serviceProvider, Movie) );

        layer_movie->setName( _layer.name );
        
        ResourceMovie * resourceMovie = RESOURCE_SERVICE(m_serviceProvider)
            ->getResourceReferenceT<ResourceMovie>( _layer.source );

        layer_movie->setResourceMovie( resourceMovie );

        layer_movie->setIntervalStart( _layer.startInterval );

        layer_movie->setPlayCount( _layer.playCount );
        layer_movie->setScretch( _layer.scretch );
        //layer_movie->setLoop( _layer.loop );

        layer_movie->setParentMovie( true );

        m_subMovies.insert( _layer.name, layer_movie );

        this->addMovieNode_( _layer, layer_movie );

        return true;
    }
	//////////////////////////////////////////////////////////////////////////
	bool Movie::createMovieInternalObject_( const MovieLayer & _layer )
	{
		MovieInternalObject * movie_internal = NODE_SERVICE(m_serviceProvider)
			->createNodeT<MovieInternalObject>( CONST_STRING(m_serviceProvider, MovieInternalObject) );

        movie_internal->setName( _layer.name );

        ResourceInternalObject * resourceInternalObject = RESOURCE_SERVICE(m_serviceProvider)
            ->getResourceT<ResourceInternalObject>( _layer.source );

		movie_internal->setMovie( this );
        movie_internal->setResourceInternalObject( resourceInternalObject );

        m_internals.insert( _layer.name, movie_internal );

		this->addMovieNode_( _layer, movie_internal );

		return true;
	}
	//////////////////////////////////////////////////////////////////////////
	bool Movie::createMovieVideo_( const MovieLayer & _layer )
	{
		Video * layer_video = NODE_SERVICE(m_serviceProvider)
			->createNodeT<Video>( CONST_STRING(m_serviceProvider, Video) );

        ResourceVideo * resourceVideo = RESOURCE_SERVICE(m_serviceProvider)
            ->getResourceReferenceT<ResourceVideo>( _layer.source );

		layer_video->setResourceVideo( resourceVideo );
        layer_video->setName( _layer.name );

		layer_video->setIntervalStart( _layer.startInterval );
        layer_video->setPlayCount( _layer.playCount );
        layer_video->setScretch( _layer.scretch );
        //layer_video->setLoop( _layer.loop );

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

		this->addMovieNode_( _layer, layer_video );

		return true;
	}
	//////////////////////////////////////////////////////////////////////////
	bool Movie::createMovieSound_( const MovieLayer & _layer )
	{
		SoundEmitter * layer_sound = NODE_SERVICE(m_serviceProvider)
			->createNodeT<SoundEmitter>( CONST_STRING(m_serviceProvider, SoundEmitter) );

        ResourceSound * resourceSound = RESOURCE_SERVICE(m_serviceProvider)
            ->getResourceReferenceT<ResourceSound>( _layer.source );

		layer_sound->setResourceSound( resourceSound );
        layer_sound->setName( _layer.name );
		layer_sound->setIntervalStart( _layer.startInterval );

        layer_sound->setPlayCount( _layer.playCount );
        layer_sound->setScretch( _layer.scretch );
        //layer_sound->setLoop( _layer.loop );

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

        this->addMovieNode_( _layer, layer_text );

        return true;
    }
	//////////////////////////////////////////////////////////////////////////
	bool Movie::createMovieTextCenter_( const MovieLayer & _layer )
	{
		TextField * layer_text = NODE_SERVICE(m_serviceProvider)
			->createNodeT<TextField>( CONST_STRING(m_serviceProvider, TextField) );

		layer_text->setName( _layer.name );
		layer_text->setTextByKey( _layer.name ); //Name = TextID
		layer_text->setCenterAlign();

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

        m_events.insert( _layer.name, layer_event );

        this->addMovieNode_( _layer, layer_event );

        return true;
    }
	//////////////////////////////////////////////////////////////////////////
	bool Movie::createMovieParticleEmitter_( const MovieLayer & _layer )
	{
		ParticleEmitter * layer_particles = NODE_SERVICE(m_serviceProvider)
			->createNodeT<ParticleEmitter>( CONST_STRING(m_serviceProvider, ParticleEmitter) );
        
        ResourceEmitter * resourceEmitter = RESOURCE_SERVICE(m_serviceProvider)
            ->getResourceReferenceT<ResourceEmitter>( _layer.source );

		const ConstString & container = resourceEmitter->getContainer();

		ResourceEmitterContainer * resourceEmitterContainer = RESOURCE_SERVICE(m_serviceProvider)
			->getResourceReferenceT<ResourceEmitterContainer>( container );

		layer_particles->setResourceEmitterContainer( resourceEmitterContainer );
        layer_particles->setName( _layer.name );

        //layer_particles->setIntervalStart( _layer.startInterval );        
        layer_particles->setPlayCount( _layer.playCount );
        layer_particles->setScretch( _layer.scretch );
        //layer_particles->setLoop( _layer.loop );

		layer_particles->setLoop( true );

		if( resourceEmitter->getEmitterRelative() == true )
		{
			//layer_particles->setEmitterRelative( true );
			layer_particles->setEmitterTranslateWithParticle( true );
		}

		const ConstString & emitterName = resourceEmitter->getEmitterName();
		layer_particles->setEmitter( emitterName );

		const mt::vec2f & offset = resourceEmitter->getOffset();

		mt::vec3f position;
		position.x = offset.x;
		position.y = offset.y;
		position.z = 0.f;

		layer_particles->setEmitterPosition( position );

		this->addMovieNode_( _layer, layer_particles );

		return true;
	}
	//////////////////////////////////////////////////////////////////////////
	bool Movie::_compile()
	{
		if( m_resourceMovie == nullptr )
		{
			LOGGER_ERROR(m_serviceProvider)("Movie::_compile: '%s' can't setup resource"
				, this->getName().c_str()
				);

			return false;
		}

        if( m_resourceMovie.compile() == false )
        {
            LOGGER_ERROR(m_serviceProvider)( "Movie::_compile '%s' movie resource %s not compile"
                , m_name.c_str() 
                , m_resourceMovie->getName().c_str()
                );

            return false;
        }
        
		bool reverse = this->getReverse();
		this->updateReverse_( reverse );

        Layer * layer = this->getLayer();

        for( TMapSceneEffects::iterator
            it = m_sceneEffects.begin(),
            it_end = m_sceneEffects.end();
        it != it_end;
        ++it )
        {
            MovieSceneEffect * sceneEffect = it->second;

            sceneEffect->setPropagateNode( layer );
        }

		if( this->setupParent_() == false )
        {
            LOGGER_ERROR(m_serviceProvider)("Movie: '%s' can't setup layer parents"
                , m_name.c_str()
                );

            return false;
        }

		return true;
	}
    //////////////////////////////////////////////////////////////////////////
    void Movie::_release()
    {	
        this->removeParent_();

        //const TVectorMovieLayers & layers = m_resourceMovie->getLayers();
        //this->releaseLayers_();

        m_resourceMovie.release();
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

            if( node == nullptr )
            {
                continue;
            }

			if( layer.isThreeD() == true )
			{
				node->setRenderCamera( m_renderCamera3D );
				node->setRenderViewport( m_renderViewport );
			}
		}
	}
	//////////////////////////////////////////////////////////////////////////
	bool Movie::setupParent_()
	{
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

            if( node == nullptr )
            {
                continue;
            }
             
            this->updateFrameNode_( layer, node, 0, false );
                 
            if( layer.parent != 0 && layer.parent != movie_layer_parent_none )
			{
				Node * node_parent = this->getMovieParent_( layer );

				if( node_parent == nullptr )
				{
					LOGGER_ERROR(m_serviceProvider)("Movie::updateParent_ %s invalid parent node %d"
						, this->getName().c_str()
						, layer.parent
						);

					return false;
				}

				node->setRelationTransformation( node_parent );	
			}
		}

        return true;
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

            if( node == nullptr )
            {
                continue;
            }

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

			Node * node = this->getMovieNode_( layer );

            if( node == nullptr )
            {
                continue;
            }

			if( layer.switcher == true )
			{
				node->localHide( true );
			}

			if( layer.isAnimatable() == false )
			{
				continue;
			}

			Animatable * animatable = dynamic_cast<Animatable *>(node);

            if( layer.timeRemap == false )
            {
                if( animatable->isPlay() == true )
                {
                    animatable->stop();
                }
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

            if( node == nullptr )
            {
                continue;
            }

			Animatable * animatable = dynamic_cast<Animatable *>(node);
			
			animatable->setTiming( 0.f );
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

        float speedFactor = this->getSpeedFactor();
		float realTiming = _timing * speedFactor;

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

        const mt::vec2f & loopSegment = m_resourceMovie->getLoopSegment();

        if( m_intervalEnd > 0.f )
        {
            frameCount = (size_t)((m_intervalStart + m_intervalEnd) / frameDuration + 0.5f) - 1;
        }
        else
        {   
            if( loopSegment.y > 0.f )
            {
                frameCount = (size_t)(loopSegment.y / frameDuration + 0.5f) - 1;
            }
        }
        
        size_t lastFrame = m_currentFrame;

        //bool needUpdate = true;

        if( m_currentFrame != frameCount )
        {
            while( m_frameTiming >= frameDuration )
            {
                m_frameTiming -= frameDuration;

                ++m_currentFrame;

                //needUpdate = true;

                if ( m_currentFrame == frameCount )
                {
                    this->updateForwardFrame_( _time, lastFrame, frameCount );

                    //needUpdate = false;

                    if( this->getLoop() == false && --m_playIterator == 0 )
                    {
                        lastFrame = frameCount;
                        m_currentFrame = frameCount;

                        this->end();

                        return;
                    }
                    else
                    {   
                        //this->setTiming( 0.f );
                        
                        float beginTiming = m_frameTiming + loopSegment.x;

                        this->setTiming( beginTiming );

                        lastFrame = m_currentFrame;

                        this->updateAnimatablePlay_();

                        //needUpdate = false;
                    }
                }	
            }
        }
        else
        {
			this->updateForwardFrame_( _time, lastFrame, frameCount );

            this->end();

			return;
        }

        //if( needUpdate == true )
        //{
            //printf("Movie::update %s %d:%d\n"
            //    , this->getName().c_str()
            //    , lastFrame
            //    , m_currentFrame
            //    );

			this->updateForwardFrame_( _time, lastFrame, m_currentFrame );
        //}
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
            
            if( node == nullptr )
            {
                continue;
            }

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
			size_t frameId = _endFrame - indexIn;

            this->updateFrameNode_( _layer, _node, frameId, (_endFrame + 1) < indexOut );

            _node->localHide( false );

            if( _layer.isAnimatable() == true )
            {
                Animatable * animatable = dynamic_cast<Animatable *>(_node);

                if( _layer.timeRemap == false )
                {
                    if( animatable->isPlay() == true )
                    {
                        animatable->stop();
                    }

                    size_t frame = _endFrame - indexIn;
                    float timing = frame * frameDuration;

                    animatable->setTiming( timing );


                    float movieTiming = this->getTiming();
                    animatable->play( movieTiming );
                }
                else
                {
					const MovieFramePackInterfacePtr & framePack = m_resourceMovie->getFramePack();

                    float timing;
                    if( framePack->getLayerTimeRemap( _layer.index, _endFrame - indexIn, timing ) == false )
                    {
                        return;
                    }

                    animatable->setTiming( timing );
                }
            }
        }
        else if( _endFrame >= indexOut && _beginFrame >= indexIn && _beginFrame < indexOut )
        {
            //printf("Movie %s enable %f %d\n", m_name.c_str(), m_timing, layer.index);
            _node->localHide( true );

            if( _layer.isAnimatable() == true )
            {
                Animatable * animatable = dynamic_cast<Animatable *>(_node);

                if( _layer.timeRemap == false )
                {
                    if( animatable->isPlay() == true )
                    {
                        animatable->stop();

                        ///Test TestTestTestTest
                        float timing = (indexOut - indexIn) * frameDuration;                        
                        animatable->setTiming( timing );
                    }
                }
                else
                {
					const MovieFramePackInterfacePtr & framePack = m_resourceMovie->getFramePack();

                    float timing;
                    if( framePack->getLayerTimeRemap( _layer.index, indexOut - indexIn, timing ) == false )
                    {
                        return;
                    }

                    animatable->setTiming( timing );
                }
            }
        }
        else if( _beginFrame >= indexIn && _endFrame >= indexIn && _endFrame < indexOut )
        {
			size_t frameId = _endFrame - indexIn;
		
            this->updateFrameNode_( _layer, _node, frameId, (_endFrame + 1) < indexOut );

            if( _layer.isAnimatable() == true )
            {
                Animatable * animatable = dynamic_cast<Animatable *>(_node);

                if( _layer.timeRemap == false )
                {
                }
                else
                {
					const MovieFramePackInterfacePtr & framePack = m_resourceMovie->getFramePack();

                    float timing;
                    if( framePack->getLayerTimeRemap( _layer.index, _endFrame - indexIn, timing ) == false )
                    {
                        return;
                    }

                    animatable->setTiming( timing );
                }
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
				size_t frameId = _endFrame - indexIn;
                this->updateFrameNode_( layer, node, frameId, false );

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
				size_t frameId = _endFrame - indexIn;

                this->updateFrameNode_( layer, node, frameId, false );
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

            if( node == nullptr )
            {
                continue;
            }

			if( m_currentFrame >= indexIn && m_currentFrame < indexOut )
			{            
				size_t frameId = m_currentFrame - indexIn;

                this->updateFrameNode_( layer, node, frameId, (m_currentFrame + 1) < indexOut );

				if( layer.switcher == true && m_currentFrame + 1 == indexOut )
				{
					node->localHide(true);
				}
				else
				{
					node->localHide(false);
				}

                if( layer.isAnimatable() == true )
                {
                    Animatable * animatable = dynamic_cast<Animatable *>(node);
                
                    if( layer.timeRemap == false )
                    {
                        float timing = (m_currentFrame - indexIn) * frameDuration + m_frameTiming;
                        animatable->setTiming( timing );
                    }
                    else
                    {
						const MovieFramePackInterfacePtr & framePack = m_resourceMovie->getFramePack();

                        float timing;
                        if( framePack->getLayerTimeRemap( layer.index, m_currentFrame - indexIn, timing ) == false )
                        {
                            continue;
                        }

                        animatable->setTiming( timing );
                    }
                }
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
            
            if( node == nullptr )
            {
                continue;
            }

            if( m_currentFrame >= indexIn && m_currentFrame < indexOut )
            {                
                size_t frameId = m_currentFrame - indexIn;

                this->updateFrameNode_( layer, node, frameId, (m_currentFrame + 1) < indexOut );

                node->localHide( false );

                if( layer.isAnimatable() == true )
                {
                    Animatable * animatable = dynamic_cast<Animatable *>(node);

                    if( layer.timeRemap == false )
                    {
                        if( animatable->isPlay() == true )
                        {
                            animatable->stop();
                        }

                        float timing = frameId * frameDuration;

                        animatable->setTiming( timing );

                        float movieTiming = this->getTiming();
                        animatable->play( movieTiming );
                    }
                    else
                    {
						const MovieFramePackInterfacePtr & framePack = m_resourceMovie->getFramePack();

                        float timing;
                        if( framePack->getLayerTimeRemap( layer.index, frameId, timing ) == false )
                        {
                            continue;
                        }

                        animatable->setTiming( timing );
                    }
                }
            }
            else
            {
				size_t frameId = indexOut - indexIn;

                //printf("Movie %s enable %f %d\n", m_name.c_str(), m_timing, layer.index);
                node->localHide( true );

                if( layer.isAnimatable() == true )
                {
                    Animatable * animatable = dynamic_cast<Animatable *>(node);

                    if( layer.timeRemap == false )
                    {
                        if( animatable->isPlay() == true )
                        {
                            animatable->stop();

                            ///Test TestTestTestTest
                            float timing = frameId * frameDuration;
                            animatable->setTiming( timing );
                        }
                    }
                    else
                    {
						const MovieFramePackInterfacePtr & framePack = m_resourceMovie->getFramePack();

                        float timing;
                        if( framePack->getLayerTimeRemap( layer.index, frameId, timing ) == false )
                        {
                            continue;
                        }

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

            if( node == nullptr )
            {
                continue;
            }

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

		m_renderCamera3D->setCameraPosition( camera3D.cameraPosition );
		m_renderCamera3D->setCameraDir( camera3D.cameraInterest - camera3D.cameraPosition );
		m_renderCamera3D->setCameraFOV( camera3D.cameraFOV );
		m_renderCamera3D->setCameraAspect( camera3D.cameraAspect );

		Viewport rp;
		rp.begin.x = 0.f;
		rp.begin.y = 0.f;

		rp.end.x = camera3D.width;
		rp.end.y = camera3D.height;

		m_renderCamera3D->setRenderport( rp );

		this->addChildren( m_renderCamera3D );

		m_renderViewport = NODE_SERVICE(m_serviceProvider)
			->createNodeT<RenderViewport>( CONST_STRING(m_serviceProvider, RenderViewport) );

		m_renderViewport->setName( name );

		Viewport vp;
		vp.begin.x = 0.f;
		vp.begin.y = 0.f;

		vp.end.x = camera3D.width;
		vp.end.y = camera3D.height;

		m_renderViewport->setViewport( vp );
        
		this->addChildren( m_renderViewport );
		//m_renderCamera3D->setRelationTransformation( NULL );
	}
	//////////////////////////////////////////////////////////////////////////
	void Movie::destroyCamera3D_()
	{
		if( m_renderCamera3D != nullptr )
		{
			m_renderCamera3D->destroy();
			m_renderCamera3D = nullptr;
		}

		if( m_renderViewport != nullptr )
		{
			m_renderViewport->destroy();
			m_renderViewport = nullptr;
		}
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

            if( node == nullptr )
            {
                continue;
            }
			
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
            
            if( node == nullptr )
            {
                continue;
            }

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