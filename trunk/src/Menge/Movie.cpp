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
#   include "ResourceParticle.h"

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
#	include "ParticleEmitter2.h"
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

		this->destroyLayers_();

		if( m_resourceMovie != nullptr )
		{
			if( this->createLayers_() == false )
			{
				LOGGER_ERROR(m_serviceProvider)("Movie.setResourceMovie: %s resource %s can't create layers"
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
	const ConstString & Movie::getResourceMovieName() const
	{
		if( m_resourceMovie == nullptr )
		{
			return ConstString::none();
		}

		const ConstString & name = m_resourceMovie->getName();

		return name;
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
		if( this->isCompile() == false )
		{
			LOGGER_ERROR(m_serviceProvider)("Movie.setTiming: '%s' not activate"
				, m_name.c_str()
				);

			return;
		}

		float frameDuration = m_resourceMovie->getFrameDuration();
		m_currentFrame = (uint32_t)(_timing / frameDuration + 0.5f);

		uint32_t frameCount = m_resourceMovie->getFrameCount();
		if( m_currentFrame > frameCount )
		{
			m_currentFrame = frameCount;
			m_frameTiming = 0.f;
		}
		else
		{
			m_frameTiming = _timing - m_currentFrame * frameDuration;
		}		

		if( this->isActivate() == true )
		{
			this->updateTiming_();
		}
	}
	//////////////////////////////////////////////////////////////////////////
	float Movie::_getTiming() const
	{
		if( this->isCompile() == false )
		{
			LOGGER_ERROR(m_serviceProvider)("Movie.getTiming: '%s' not compile"
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

		//if( this->getName() == "Bus_Zombie_In"
		    //&& m_resourceMovieName != "Movie103_Courtyard_veronika_01"
		    //&& m_resourceMovieName != "Movie401_StreetToChurch_WallZOOM_otv"
		    //)
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
			uint32_t frameCount = m_resourceMovie->getFrameCount();

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
	bool Movie::_restart( float _time, uint32_t _enumerator )
	{
		(void)_enumerator;

		if( this->isActivate() == false )
		{
			return false;
		}

		if( m_reverse == true )
		{
			uint32_t frameCount = m_resourceMovie->getFrameCount();

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
	void Movie::_pause( uint32_t _enumerator )
	{
		(void)_enumerator;

		this->pauseAnimation_();
	}
	//////////////////////////////////////////////////////////////////////////
	void Movie::_resume( uint32_t _enumerator )
	{
		(void)_enumerator;

		this->resumeAnimation_();
	}
	//////////////////////////////////////////////////////////////////////////
	void Movie::_stop( uint32_t _enumerator )
	{
		this->stopAnimation_();

		EVENTABLE_CALL(m_serviceProvider, this, EVENT_MOVIE_END)( this, _enumerator, false );
	}
	//////////////////////////////////////////////////////////////////////////
	void Movie::_end( uint32_t _enumerator )
	{
		this->stopAnimation_();

		EVENTABLE_CALL(m_serviceProvider, this, EVENT_MOVIE_END)( this, _enumerator, true );
	}
	//////////////////////////////////////////////////////////////////////////
	bool Movie::updateFrameNode_( const MovieLayer & _layer, Node * _node, uint32_t _frameId, bool _interpolate, bool _first )
	{	
		const MovieFramePackInterfacePtr & framePack = m_resourceMovie->getFramePack();

		if( _layer.isMesh2D() == true )
		{
#   ifdef _DEBUG
			if( dynamic_cast<Mesh2D *>( _node ) == nullptr )
			{
				LOGGER_ERROR(m_serviceProvider)("Movie::updateFrameNode_ %s resource %s layer %s is Mesh2D but node is not Mesh2D %s:%s"
					, this->getName().c_str()
					, this->getResourceMovieName().c_str()
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

		if( _layer.immutable == true && _first == false )
		{
			return true;
		}

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
			_node->setLocalColorAlpha( frame.opacity );
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

		if( _layer.isAnimatable() == true )
		{
			ns.animatable = dynamic_cast<Animatable *>(_node);
		}
		
		ns.child = (_layer.parent != movie_layer_parent_none);

		_node->setName( _layer.name );
		_node->setLocalPosition( _layer.position );

		m_nodies[_layer.index - 1] = ns;
	}
	//////////////////////////////////////////////////////////////////////////
	bool Movie::visitMovieNode( const ConstString & _type, VisitorMovieNode * _visitor )
	{
		if( m_resourceMovie == nullptr )
		{
			LOGGER_ERROR(m_serviceProvider)("Movie::filterLayer %s invalid not compile"
				, m_name.c_str()
				);

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

			if( _type.empty() == false && layer.type != _type )
			{
				continue;
			}

			Node * node = this->getLayerNode_( layer );

			_visitor->visitMovieNode( this, node );
		}

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

			Node * node = this->getLayerNode_( layer );

			if( node == nullptr )
			{
				continue;
			}

#   ifdef _DEBUG
			if( dynamic_cast<Movie *>( node ) == nullptr )
			{
				LOGGER_ERROR(m_serviceProvider)("Movie::visitMovieNode %s resource %s layer %s must be 'Movie' but node is %s type %s"
					, this->getName().c_str()
					, this->getResourceMovieName().c_str()
					, layer.name.c_str()
					, node->getName().c_str()
					, node->getType().c_str()
					);

				return false;
			}
#   endif

			Movie * movie = static_cast<Movie *>(node);

			if( movie->visitMovieNode( _type, _visitor ) == false )
			{
				continue;
			}
		}

		return true;
	}
	//////////////////////////////////////////////////////////////////////////
	bool Movie::getMovieNode( const ConstString & _name, const ConstString & _type, Node ** _node, Movie ** _movie )
	{
		if( m_resourceMovie == nullptr )
		{
			LOGGER_ERROR(m_serviceProvider)("Movie.getMovieNode %s resource %s invalid not compile %s:%s"
				, this->getName().c_str()
				, this->getResourceMovieName().c_str()
				, _name.c_str()
				, _type.c_str()
				);

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

			if( layer.name != _name || layer.type != _type )
			{
				continue;
			}

			Node * node = this->getLayerNode_( layer );

			*_node = node;
			*_movie = this;

			return true;
		}

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

			Node * node = this->getLayerNode_( layer );

			if( node == nullptr )
			{
				continue;
			}

#   ifdef _DEBUG
			if( dynamic_cast<Movie *>( node ) == nullptr )
			{
				LOGGER_ERROR(m_serviceProvider)("Movie::getMovieNode %s resource %s layer %s must be 'Movie' but node is %s type %s"
					, this->getName().c_str()
					, this->getResourceMovieName().c_str()
					, layer.name.c_str()
					, node->getName().c_str()
					, node->getType().c_str()
					);

				return false;
			}
#   endif

			Movie * movie = static_cast<Movie *>(node);

			if( movie->getMovieNode( _name, _type, _node, _movie ) == true )
			{
				return true;
			}
		}

		LOGGER_ERROR(m_serviceProvider)("Movie::getMovieNode: %s resource %s not found node %s type %s"
			, this->getName().c_str()
			, this->getResourceMovieName().c_str()
			, _name.c_str()
			, _type.c_str()
			);

		return false;
	}
	//////////////////////////////////////////////////////////////////////////
	bool Movie::hasMovieNode( const ConstString & _name, const ConstString & _type, Node ** _node, Movie ** _movie )
	{
		if( m_resourceMovie == nullptr )
		{
			LOGGER_ERROR(m_serviceProvider)("Movie.hasMovieNode %s resource %s invalid not compile (%s:%s)"
				, this->getName().c_str()
				, this->getResourceMovieName().c_str()
				, _name.c_str()
				, _type.c_str()
				);

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

			if( layer.name != _name || layer.type != _type )
			{
				continue;
			}

			Node * node = this->getLayerNode_( layer );

			*_node = node;
			*_movie = this;

			return true;
		}

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

			Node * node = this->getLayerNode_( layer );

			if( node == nullptr )
			{
				continue;
			}

#   ifdef _DEBUG
			if( dynamic_cast<Movie *>( node ) == nullptr )
			{
				LOGGER_ERROR(m_serviceProvider)("Movie::hasMovieNode %s resource %s layer %s must be 'Movie' but node is %s type %s"
					, this->getName().c_str()
					, this->getResourceMovieName().c_str()
					, layer.name.c_str()
					, node->getName().c_str()
					, node->getType().c_str()
					);

				return false;
			}
#   endif

			Movie * movie = static_cast<Movie *>(node);

			if( movie->hasMovieNode( _name, _type, _node, _movie ) == true )
			{
				return true;
			}
		}

		return false;
	}
	//////////////////////////////////////////////////////////////////////////
	bool Movie::hasMovieLayer( const ConstString & _name ) const
	{
		if( m_resourceMovie == nullptr )
		{
			LOGGER_ERROR(m_serviceProvider)("Movie::hasLayer %s resource %s invalid get layer %s not compile"
				, this->getName().c_str()
				, this->getResourceMovieName().c_str()
				, _name.c_str()
				);

			return false;
		}

		const MovieLayer * layer;
		if( m_resourceMovie->hasMovieLayer( _name, &layer ) == true )
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

			Node * node = this->getLayerNode_( layer );

			if( node == nullptr )
			{
				continue;
			}

#   ifdef _DEBUG
			if( dynamic_cast<Movie *>( node ) == nullptr )
			{
				LOGGER_ERROR(m_serviceProvider)("Movie::hasMovieLayer %s resource %s layer %s must be 'Movie' but node is %s type %s"
					, this->getName().c_str()
					, this->getResourceMovieName().c_str()
					, layer.name.c_str()
					, node->getName().c_str()
					, node->getType().c_str()
					);

				return false;
			}
#   endif

			Movie * movie = static_cast<Movie *>(node);

			if( movie->hasMovieLayer( _name ) == false )
			{
				continue;
			}

			return true;
		}

		return false;
	}
	//////////////////////////////////////////////////////////////////////////
	bool Movie::getMovieLayer( const ConstString & _name, const MovieLayer ** _layer, Movie ** _movie )
	{
		if( m_resourceMovie == nullptr )
		{
			LOGGER_ERROR(m_serviceProvider)("Movie::getLayer %s resource %s invalid get layer %s not compile"
				, this->getName().c_str()
				, this->getResourceMovieName().c_str()
				, _name.c_str()
				);

			return nullptr;
		}

		const MovieLayer * layer;
		if( m_resourceMovie->hasMovieLayer( _name, &layer ) == true )
		{	
			*_layer = layer;
			*_movie = this;

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

			Node * node = this->getLayerNode_( layer );

			if( node == nullptr )
			{
				continue;
			}

#   ifdef _DEBUG
			if( dynamic_cast<Movie *>( node ) == nullptr )
			{
				LOGGER_ERROR(m_serviceProvider)("Movie::getMovieLayer %s resource %s layer %s must be 'Movie' but node is %s type %s"
					, this->getName().c_str()
					, this->getResourceMovieName().c_str()
					, layer.name.c_str()
					, node->getName().c_str()
					, node->getType().c_str()
					);

				return false;
			}
#   endif

			Movie * movie = static_cast<Movie *>(node);

			if( movie->getMovieLayer( _name, _layer, _movie ) == false )
			{
				continue;
			}

			return true;
		}

		LOGGER_ERROR(m_serviceProvider)("Movie::getLayer %s resource %s not found layer %s"
			, this->getName().c_str()
			, this->getResourceMovieName().c_str()
			, _name.c_str()
			);

		return false;
	}
	//////////////////////////////////////////////////////////////////////////
	bool Movie::createLayers_()
	{
		uint32_t maxLayerIndex = m_resourceMovie->getMaxLayerIndex();

		m_nodies.resize( maxLayerIndex );

		const TVectorMovieLayers & layers = m_resourceMovie->getLayers();

		for( TVectorMovieLayers::const_iterator
			it = layers.begin(),
			it_end = layers.end();
		it != it_end;
		++it )
		{
			const MovieLayer & layer = *it;

			if ( layer.type == CONST_STRING(m_serviceProvider, MovieSlot) )
			{
				if( this->createMovieSlot_( layer ) == false )
				{
					return false;
				}
			}
			else if( layer.type == CONST_STRING(m_serviceProvider, MovieSceneEffect) )
			{
				if( this->createMovieSceneEffect_( layer ) == false )
				{
					return false;
				}
			}
			else if( layer.type == CONST_STRING(m_serviceProvider, MovieText) )
			{
				if( this->createMovieText_( layer ) == false )
				{
					return false;
				}
			}
			else if( layer.type == CONST_STRING(m_serviceProvider, MovieTextCenter) )
			{
				if( this->createMovieTextCenter_( layer ) == false )
				{
					return false;
				}
			}
			else if( layer.type == CONST_STRING(m_serviceProvider, MovieSprite) )
			{
				if( this->createMovieExtraSprite_( layer ) == false )
				{
					return false;
				}
			}
			else if( layer.type == CONST_STRING(m_serviceProvider, MovieNullObject) )
			{
				if( this->createMovieNullObject_( layer ) == false )
				{
					return false;
				}
			}
			else if( layer.type == CONST_STRING(m_serviceProvider, Image) )
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
			else if( layer.type == CONST_STRING(m_serviceProvider, SolidSprite) )
			{
				if( this->createMovieImageSolid_( layer ) == false )
				{
					return false;
				}
			}
			else if( layer.type == CONST_STRING(m_serviceProvider, MovieSocketImage) )
			{
				if( this->createMovieSocketImage_( layer ) == false )
				{
					return false;
				}
			}
			else if( layer.type == CONST_STRING(m_serviceProvider, MovieSocketShape) )
			{
				if( this->createMovieSocketShape_( layer ) == false )
				{
					return false;
				}
			}
			else if( layer.type == CONST_STRING(m_serviceProvider, Animation) )
			{
				if( this->createMovieAnimation_( layer ) == false )
				{
					return false;
				}
			}
			else if( layer.type == CONST_STRING(m_serviceProvider, Movie) )
			{
				if( this->createMovieMovie_( layer ) == false )
				{
					return false;
				}
			}
			else if( layer.type == CONST_STRING(m_serviceProvider, SubMovie) )
			{
				if( this->createMovieSubMovie_( layer ) == false )
				{
					return false;
				}
			}
			else if( layer.type == CONST_STRING(m_serviceProvider, MovieInternalObject) )
			{				
				if( this->createMovieInternalObject_( layer ) == false )
				{
					return false;
				}
			}
			else if( layer.type == CONST_STRING(m_serviceProvider, Video) )
			{
				if( this->createMovieVideo_( layer ) == false )
				{
					return false;
				}
			}
			else if( layer.type == CONST_STRING(m_serviceProvider, Sound) )
			{
				if( this->createMovieSound_( layer ) == false )
				{
					return false;
				}
			}
			else if( layer.type == CONST_STRING(m_serviceProvider, SoundId) )
			{
				if( this->createMovieSoundId_( layer ) == false )
				{
					return false;
				}
			}
			else if( layer.type == CONST_STRING(m_serviceProvider, ParticleEmitter) )
			{
				if( this->createMovieParticleEmitter_( layer ) == false )
				{
					return false;
				}
			}
			else if( layer.type == CONST_STRING(m_serviceProvider, ParticleEmitter2) )
			{
				if( this->createMovieParticleEmitter2_( layer ) == false )
				{
					return false;
				}
			}
			else if( layer.type == CONST_STRING(m_serviceProvider, MovieEvent) )
			{
				if( this->createMovieEvent_( layer ) == false )
				{
					return false;
				}
			}
			else
			{
				LOGGER_ERROR(m_serviceProvider)("Movie.createLayers_: %s resource %s can't create layer_node '%s' type '%s'"
					, this->getName().c_str()
					, this->getResourceMovieName().c_str()
					, layer.source.c_str()
					, layer.type.c_str()
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

			this->addChild( ns.node );
		}

		this->createCamera3D_();

		if( m_resourceMovie->hasAnchorPoint() == true )
		{
			const mt::vec3f & anchorPoint = m_resourceMovie->getAnchorPoint();
			this->setOrigin( anchorPoint );
		}

		return true;
	}
	//////////////////////////////////////////////////////////////////////////
	bool Movie::compileLayers_()
	{
		const TVectorMovieLayers & layers = m_resourceMovie->getLayers();

		for( TVectorMovieLayers::const_iterator
			it = layers.begin(),
			it_end = layers.end();
		it != it_end;
		++it )
		{
			const MovieLayer & layer = *it;

			if ( layer.type == CONST_STRING(m_serviceProvider, MovieText) )
			{
				if( this->compileMovieText_( layer ) == false )
				{
					return false;
				}
			}
			else if ( layer.type == CONST_STRING(m_serviceProvider, MovieTextCenter) )
			{
				if( this->compileMovieText_( layer ) == false )
				{
					return false;
				}
			}
		}

		return true;
	}
	//////////////////////////////////////////////////////////////////////////
	void Movie::destroyLayers_()
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

		this->destroyCamera3D_();
	}
	//////////////////////////////////////////////////////////////////////////
	bool Movie::createMovieSlot_( const MovieLayer & _layer )
	{
		MovieSlot * layer_slot = NODE_SERVICE(m_serviceProvider)
			->createNodeT<MovieSlot>( CONST_STRING(m_serviceProvider, MovieSlot) );

		if( layer_slot == nullptr )
		{
			return false;
		}

		layer_slot->setMovieName( m_name );		

		this->addMovieNode_( _layer, layer_slot );

		return true;
	}
	//////////////////////////////////////////////////////////////////////////
	bool Movie::createMovieSceneEffect_( const MovieLayer & _layer )
	{
		MovieSceneEffect * sceneeffect_slot = NODE_SERVICE(m_serviceProvider)
			->createNodeT<MovieSceneEffect>( CONST_STRING(m_serviceProvider, MovieSceneEffect) );

		if( sceneeffect_slot == nullptr )
		{
			return false;
		}

		this->addMovieNode_( _layer, sceneeffect_slot );

		return true;
	}
	//////////////////////////////////////////////////////////////////////////
	bool Movie::createMovieNullObject_( const MovieLayer & _layer )
	{
		Node * layer_slot = NODE_SERVICE(m_serviceProvider)
			->createNodeT<Node>( CONST_STRING(m_serviceProvider, Node) );

		if( layer_slot == nullptr )
		{
			return false;
		}

		this->addMovieNode_( _layer, layer_slot );

		return true;
	}
	//////////////////////////////////////////////////////////////////////////
	bool Movie::createMovieImage_( const MovieLayer & _layer )
	{
		Sprite * layer_sprite = NODE_SERVICE(m_serviceProvider)
			->createNodeT<Sprite>( CONST_STRING(m_serviceProvider, Sprite) );

		if( layer_sprite == nullptr )
		{
			return false;
		}

		ResourceImage * resourceImage = RESOURCE_SERVICE(m_serviceProvider)
			->getResourceReferenceT<ResourceImage>( _layer.source );

		if( resourceImage == nullptr )
		{
			return false;
		}

		layer_sprite->setResourceImage( resourceImage );

		if( _layer.blendingMode.empty() == true )
		{
			//Empty
		}
		else if( _layer.blendingMode == CONST_STRING(m_serviceProvider, BlendingModeNormal) )
		{
			//Empty
		}
		else if( _layer.blendingMode == CONST_STRING(m_serviceProvider, BlendingModeAdd) )
		{
			layer_sprite->setBlendAdd( true );
		}
		else
		{
			LOGGER_ERROR(m_serviceProvider)("Movie.createMovieImage_: %s resource %s layer '%s' blending mode not supported '%s'"
				, this->getName().c_str()
				, this->getResourceMovieName().c_str()
				, _layer.name.c_str()
				, _layer.blendingMode.c_str()
				);

			return false;
		}

		this->addMovieNode_( _layer, layer_sprite );

		return true;
	}
	//////////////////////////////////////////////////////////////////////////
	bool Movie::createMovieMesh2D_( const MovieLayer & _layer )
	{
		Mesh2D * layer_mesh = NODE_SERVICE(m_serviceProvider)
			->createNodeT<Mesh2D>( CONST_STRING(m_serviceProvider, Mesh2D) );

		if( layer_mesh == nullptr )
		{
			return false;
		}

		ResourceImage * resourceImage = RESOURCE_SERVICE(m_serviceProvider)
			->getResourceReferenceT<ResourceImage>( _layer.source );

		if( resourceImage == nullptr )
		{
			return false;
		}

		layer_mesh->setResourceImage( resourceImage );

		if( _layer.blendingMode.empty() == true )
		{
			//Empty
		}
		else if( _layer.blendingMode == CONST_STRING(m_serviceProvider, BlendingModeNormal) )
		{
			//Empty
		}
		else if( _layer.blendingMode == CONST_STRING(m_serviceProvider, BlendingModeAdd) )
		{
			layer_mesh->setBlendAdd( true );
		}
		else
		{
			LOGGER_ERROR(m_serviceProvider)("Movie.createMovieMesh2D_: %s resource %s layer '%s' blending mode not supported '%s'"
				, this->getName().c_str()
				, this->getResourceMovieName().c_str()
				, _layer.name.c_str()
				, _layer.blendingMode.c_str()
				);

			return false;
		}

		this->addMovieNode_( _layer, layer_mesh );

		return true;
	}
	//////////////////////////////////////////////////////////////////////////
	bool Movie::createMovieImageSolid_( const MovieLayer & _layer )
	{
		ResourceImageSolid * resource = RESOURCE_SERVICE(m_serviceProvider)
			->getResourceT<ResourceImageSolid>( _layer.source );

		if( resource == nullptr )
		{
			LOGGER_ERROR(m_serviceProvider)("Movie.createMovieImageSolid_: %s resource %s can't compile sprite '%s' imageSolid resource = NULL"
				, this->getName().c_str()
				, this->getResourceMovieName().c_str()
				, _layer.name.c_str()
				);

			return false;
		}

		Sprite * layer_sprite = NODE_SERVICE(m_serviceProvider)
			->createNodeT<Sprite>( CONST_STRING(m_serviceProvider, Sprite) );

		if( layer_sprite == nullptr )
		{
			return false;
		}

		layer_sprite->setResourceImage( resource );

		if( _layer.blendingMode.empty() == true )
		{
			//Empty
		}
		else if( _layer.blendingMode == CONST_STRING(m_serviceProvider, BlendingModeNormal) )
		{
			//Empty
		}
		else if( _layer.blendingMode == CONST_STRING(m_serviceProvider, BlendingModeAdd) )
		{
			layer_sprite->setBlendAdd( true );
		}
		else
		{
			LOGGER_ERROR(m_serviceProvider)("Movie.createMovieImageSolid_: %s resource %s layer '%s' blending mode not supported '%s'"
				, this->getName().c_str()
				, this->getResourceMovieName().c_str()
				, _layer.name.c_str()
				, _layer.blendingMode.c_str()
				);

			return false;
		}

		this->addMovieNode_( _layer, layer_sprite );

		return true;
	}
	//////////////////////////////////////////////////////////////////////////
	bool Movie::createMovieSocketImage_( const MovieLayer & _layer )
	{
		HotSpotImage * layer_hotspotimage = NODE_SERVICE(m_serviceProvider)
			->createNodeT<HotSpotImage>( CONST_STRING(m_serviceProvider, HotSpotImage) );

		if( layer_hotspotimage == nullptr )
		{
			return false;
		}

		ResourceHIT * resourceHIT = RESOURCE_SERVICE(m_serviceProvider)
			->getResourceReferenceT<ResourceHIT>( _layer.source );

		if( resourceHIT == nullptr )
		{
			return false;
		}
		
		layer_hotspotimage->setResourceHIT( resourceHIT );

		this->addMovieNode_( _layer, layer_hotspotimage );

		return true;
	}
	//////////////////////////////////////////////////////////////////////////
	bool Movie::createMovieSocketShape_( const MovieLayer & _layer )
	{
		HotSpotShape * layer_hotspotshape = NODE_SERVICE(m_serviceProvider)
			->createNodeT<HotSpotShape>( CONST_STRING(m_serviceProvider, HotSpotShape) );

		if( layer_hotspotshape == nullptr )
		{
			return false;
		}

		ResourceShape * resourceShape = RESOURCE_SERVICE(m_serviceProvider)
			->getResourceReferenceT<ResourceShape>( _layer.source );

		if( resourceShape == nullptr )
		{
			return false;
		}

		layer_hotspotshape->setResourceShape( resourceShape );
		
		this->addMovieNode_( _layer, layer_hotspotshape );

		return true;
	}
	//////////////////////////////////////////////////////////////////////////
	bool Movie::createMovieAnimation_( const MovieLayer & _layer )
	{
		Animation * layer_animation = NODE_SERVICE(m_serviceProvider)
			->createNodeT<Animation>( CONST_STRING(m_serviceProvider, Animation) );

		if( layer_animation == nullptr )
		{
			return false;
		}

		ResourceAnimation * resourceAnimation = RESOURCE_SERVICE(m_serviceProvider)
			->getResourceReferenceT<ResourceAnimation>( _layer.source );

		if( resourceAnimation == nullptr )
		{
			return false;
		}

		layer_animation->setResourceAnimation( resourceAnimation );

		layer_animation->setIntervalStart( _layer.startInterval );
		layer_animation->setPlayCount( _layer.playCount );
		layer_animation->setScretch( _layer.scretch );
		//layer_animation->setLoop( _layer.loop );

		if( _layer.blendingMode.empty() == true )
		{
			//Empty
		}
		else if( _layer.blendingMode == CONST_STRING(m_serviceProvider, BlendingModeNormal) )
		{
			//Empty
		}
		else if( _layer.blendingMode == CONST_STRING(m_serviceProvider, BlendingModeAdd) )
		{
			layer_animation->setBlendAdd( true );
		}
		else
		{
			LOGGER_ERROR(m_serviceProvider)("Movie.createMovieAnimation_: %s resource %s layer '%s' blending mode not supported '%s'"
				, this->getName().c_str()
				, this->getResourceMovieName().c_str()
				, _layer.name.c_str()
				, _layer.blendingMode.c_str()
				);

			return false;
		}

		this->addMovieNode_( _layer, layer_animation );

		return true;
	}
	//////////////////////////////////////////////////////////////////////////
	bool Movie::createMovieMovie_( const MovieLayer & _layer )
	{
		Movie * layer_movie = NODE_SERVICE(m_serviceProvider)
			->createNodeT<Movie>( CONST_STRING(m_serviceProvider, Movie) );

		if( layer_movie == nullptr )
		{
			return false;
		}

		ResourceMovie * resourceMovie = RESOURCE_SERVICE(m_serviceProvider)
			->getResourceReferenceT<ResourceMovie>( _layer.source );

		if( resourceMovie == nullptr )
		{
			return false;
		}

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

		if( layer_movie == nullptr )
		{
			return false;
		}

		ResourceMovie * resourceMovie = RESOURCE_SERVICE(m_serviceProvider)
			->getResourceReferenceT<ResourceMovie>( _layer.source );

		if( resourceMovie == nullptr )
		{
			return false;
		}

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
	bool Movie::createMovieInternalObject_( const MovieLayer & _layer )
	{
		MovieInternalObject * movie_internal = NODE_SERVICE(m_serviceProvider)
			->createNodeT<MovieInternalObject>( CONST_STRING(m_serviceProvider, MovieInternalObject) );

		if( movie_internal == nullptr )
		{
			return false;
		}

		ResourceInternalObject * resourceInternalObject = RESOURCE_SERVICE(m_serviceProvider)
			->getResourceT<ResourceInternalObject>( _layer.source );

		if( resourceInternalObject == nullptr )
		{
			return false;
		}

		movie_internal->setMovie( this );
		movie_internal->setResourceInternalObject( resourceInternalObject );

		this->addMovieNode_( _layer, movie_internal );

		return true;
	}
	//////////////////////////////////////////////////////////////////////////
	bool Movie::createMovieVideo_( const MovieLayer & _layer )
	{
		Video * layer_video = NODE_SERVICE(m_serviceProvider)
			->createNodeT<Video>( CONST_STRING(m_serviceProvider, Video) );

		if( layer_video == nullptr )
		{
			return false;
		}

		ResourceVideo * resourceVideo = RESOURCE_SERVICE(m_serviceProvider)
			->getResourceReferenceT<ResourceVideo>( _layer.source );

		if( resourceVideo == nullptr )
		{
			return false;
		}

		layer_video->setResourceVideo( resourceVideo );

		layer_video->setIntervalStart( _layer.startInterval );
		layer_video->setPlayCount( _layer.playCount );
		layer_video->setScretch( _layer.scretch );
		//layer_video->setLoop( _layer.loop );

		if( _layer.blendingMode.empty() == true )
		{
			//Empty
		}
		else if( _layer.blendingMode == CONST_STRING(m_serviceProvider, BlendingModeNormal) )
		{
			//Empty
		}
		else if( _layer.blendingMode == CONST_STRING(m_serviceProvider, BlendingModeAdd) )
		{
			layer_video->setBlendAdd( true );
		}
		else
		{
			LOGGER_ERROR(m_serviceProvider)("Movie.createMovieVideo_: %s resource %s layer '%s' blending mode not supported '%s'"
				, this->getName().c_str()
				, this->getResourceMovieName().c_str()
				, _layer.name.c_str()
				, _layer.blendingMode.c_str()
				);

			return false;
		}

		this->addMovieNode_( _layer, layer_video );

		return true;
	}
	//////////////////////////////////////////////////////////////////////////
	bool Movie::createMovieSound_( const MovieLayer & _layer )
	{
		SoundEmitter * layer_sound = NODE_SERVICE(m_serviceProvider)
			->createNodeT<SoundEmitter>( CONST_STRING(m_serviceProvider, SoundEmitter) );

		if( layer_sound == nullptr )
		{
			return false;
		}

		ResourceSound * resourceSound = RESOURCE_SERVICE(m_serviceProvider)
			->getResourceReferenceT<ResourceSound>( _layer.source );

		if( resourceSound == nullptr )
		{
			return false;
		}

		layer_sound->setResourceSound( resourceSound );

		layer_sound->setIntervalStart( _layer.startInterval );

		layer_sound->setPlayCount( _layer.playCount );
		layer_sound->setScretch( _layer.scretch );
		//layer_sound->setLoop( _layer.loop );

		this->addMovieNode_( _layer, layer_sound );

		return true;
	}
	//////////////////////////////////////////////////////////////////////////
	bool Movie::createMovieSoundId_( const MovieLayer & _layer )
	{
		SoundEmitter * layer_sound = NODE_SERVICE(m_serviceProvider)
			->createNodeT<SoundEmitter>( CONST_STRING(m_serviceProvider, SoundEmitter) );

		if( layer_sound == nullptr )
		{
			return false;
		}

		ResourceSound * resourceSound = RESOURCE_SERVICE(m_serviceProvider)
			->getResourceReferenceT<ResourceSound>( _layer.name );

		if( resourceSound == nullptr )
		{
			return false;
		}

		layer_sound->setResourceSound( resourceSound );
		//layer_sound->setIntervalStart( _layer.startInterval );

		layer_sound->setPlayCount( 1 );
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

		if( layer_text == nullptr )
		{
			return false;
		}

		layer_text->setTextID( _layer.name ); //Name = TextID

		if( _layer.hasParam( MOVIE_LAYER_PARAM_TEXT_HORIZONTAL_CENTER ) == true )
		{
			layer_text->setHorizontalCenterAlign();
		}

		if( _layer.hasParam( MOVIE_LAYER_PARAM_TEXT_VERTICAL_CENTER ) == true )
		{
			layer_text->setVerticalCenterAlign();
		}

		this->addMovieNode_( _layer, layer_text );

		return true;
	}
	//////////////////////////////////////////////////////////////////////////
	bool Movie::createMovieTextCenter_( const MovieLayer & _layer )
	{
		TextField * layer_text = NODE_SERVICE(m_serviceProvider)
			->createNodeT<TextField>( CONST_STRING(m_serviceProvider, TextField) );

		if( layer_text == nullptr )
		{
			return false;
		}

		layer_text->setTextID( _layer.name ); //Name = TextID
		layer_text->setHorizontalCenterAlign();

		this->addMovieNode_( _layer, layer_text );

		return true;
	}
	//////////////////////////////////////////////////////////////////////////
	bool Movie::createMovieExtraSprite_( const MovieLayer & _layer )
	{
		Sprite * layer_sprite = NODE_SERVICE(m_serviceProvider)
			->createNodeT<Sprite>( CONST_STRING(m_serviceProvider, Sprite) );

		if( layer_sprite == nullptr )
		{
			LOGGER_ERROR(m_serviceProvider)("Movie::createMovieSprite_ %s resource %s layer %s invalid create 'Sprite'"
				, this->getName().c_str()
				, this->getResourceMovieName().c_str()
				, _layer.name.c_str()
				);

			return false;
		}

		ResourceImage * resourceImage = RESOURCE_SERVICE(m_serviceProvider)
			->getResourceReferenceT<ResourceImage>( _layer.name );

		if( resourceImage == nullptr )
		{
			LOGGER_ERROR(m_serviceProvider)("Movie::createMovieSprite_ %s resource %s layer %s invalid get resource for image %s"
				, this->getName().c_str()
				, this->getResourceMovieName().c_str()
				, _layer.name.c_str()
				, _layer.name.c_str()
				);

			return false;
		}

		layer_sprite->setResourceImage( resourceImage );

		if( _layer.blendingMode.empty() == true )
		{
			//Empty
		}
		else if( _layer.blendingMode == CONST_STRING(m_serviceProvider, BlendingModeNormal) )
		{
			//Empty
		}
		else if( _layer.blendingMode == CONST_STRING(m_serviceProvider, BlendingModeAdd) )
		{
			layer_sprite->setBlendAdd( true );
		}
		else
		{
			LOGGER_ERROR(m_serviceProvider)("Movie.createMovieExtraSprite_: %s resource %s layer '%s' blending mode not supported '%s'"
				, this->getName().c_str()
				, this->getResourceMovieName().c_str()
				, _layer.name.c_str()
				, _layer.blendingMode.c_str()
				);

			return false;
		}

		this->addMovieNode_( _layer, layer_sprite );

		return true;
	}
	//////////////////////////////////////////////////////////////////////////
	bool Movie::compileMovieText_( const MovieLayer & _layer )
	{
		if( _layer.polygon == false )
		{
			return true;
		}

		Node * node = this->getLayerNode_( _layer );

		if( node == nullptr )
		{
			return false;
		}

#   ifdef _DEBUG
		if( dynamic_cast<TextField *>( node ) == nullptr )
		{
			LOGGER_ERROR(m_serviceProvider)("Movie::compileMovieText_ %s resource %s layer %s must be 'TextField' but node is %s type %s"
				, this->getName().c_str()
				, this->getResourceMovieName().c_str()
				, _layer.name.c_str()
				, node->getName().c_str()
				, node->getType().c_str()
				);

			return false;
		}
#   endif

		TextField * layer_text = static_cast<TextField *>(node);

		const MovieFramePackInterfacePtr & framePack = m_resourceMovie->getFramePack();

		uint8_t vertexCount; 
		const mt::vec2f * polygon;
		if( framePack->getLayerPolygon( _layer.index, &polygon, vertexCount ) == false )
		{
			return false;
		}

		if( vertexCount != 2 )
		{
			return false;
		}

		const mt::vec2f & begin = polygon[0];
		const mt::vec2f & end = polygon[1];

		float length = mt::length_v2_v2( begin, end );

		layer_text->setMaxLength( length );

		return true;
	}
	//////////////////////////////////////////////////////////////////////////
	bool Movie::createMovieEvent_( const MovieLayer & _layer )
	{
		MovieEvent * layer_event = NODE_SERVICE(m_serviceProvider)
			->createNodeT<MovieEvent>( CONST_STRING(m_serviceProvider, MovieEvent) );

		layer_event->setResourceMovie( m_resourceMovie );

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

		if( resourceEmitter == nullptr )
		{
			return false;
		}

		const ConstString & container = resourceEmitter->getContainer();

		ResourceEmitterContainer * resourceEmitterContainer = RESOURCE_SERVICE(m_serviceProvider)
			->getResourceReferenceT<ResourceEmitterContainer>( container );

		if( resourceEmitterContainer == nullptr )
		{
			return false;
		}

		layer_particles->setResourceEmitterContainer( resourceEmitterContainer );

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
	bool Movie::createMovieParticleEmitter2_( const MovieLayer & _layer )
	{
		ParticleEmitter2 * layer_particles = NODE_SERVICE(m_serviceProvider)
			->createNodeT<ParticleEmitter2>( CONST_STRING(m_serviceProvider, ParticleEmitter2) );

		ResourceParticle * resourceParticle = RESOURCE_SERVICE(m_serviceProvider)
			->getResourceReferenceT<ResourceParticle>( _layer.source );

		if( resourceParticle == nullptr )
		{
			return false;
		}

		layer_particles->setResourceParticle( resourceParticle );

		//layer_particles->setIntervalStart( _layer.startInterval );        
		layer_particles->setPlayCount( _layer.playCount );
		layer_particles->setScretch( _layer.scretch );
		layer_particles->setLoop( true );

		layer_particles->setEmitterTranslateWithParticle( true );

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
			LOGGER_ERROR(m_serviceProvider)("Movie::_compile '%s' resource %s not compile"
				, m_name.c_str() 
				, m_resourceMovie->getName().c_str()
				);

			return false;
		}

		bool reverse = this->getReverse();
		this->updateReverse_( reverse );

		if( this->setupSceneEffect_() == false )
		{
			LOGGER_ERROR(m_serviceProvider)("Movie::_compile: %s resource %s can't setup scene effect"
				, m_name.c_str()
				, m_resourceMovie->getName().c_str()
				);

			return false;
		}

		if( this->setupParent_() == false )
		{
			LOGGER_ERROR(m_serviceProvider)("Movie::_compile: %s resource %s can't setup layer parents"
				, m_name.c_str()
				, m_resourceMovie->getName().c_str()
				);

			return false;
		}

		if( this->compileLayers_() == false )
		{
			LOGGER_ERROR(m_serviceProvider)("Movie::_compile: %s resource %s can't compile layers"
				, m_name.c_str()
				, m_resourceMovie->getName().c_str()
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

			Node * node = this->getLayerNode_( layer );

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
	bool Movie::setupSceneEffect_()
	{
		const TVectorMovieLayers & layers = m_resourceMovie->getLayers();

		Layer * layer = this->getLayer();

		for( TVectorMovieLayers::const_iterator
			it = layers.begin(),
			it_end = layers.end();
		it != it_end;
		++it )
		{
			const MovieLayer & l = *it;

			if( l.isSceneEffect() == false )
			{
				continue;
			}

			Node * node = this->getLayerNode_( l );

#   ifdef _DEBUG
			if( dynamic_cast<MovieSceneEffect *>(node) == nullptr )
			{
				LOGGER_ERROR(m_serviceProvider)("Movie::compileMovieText_ %s resource %s layer %s must be 'MovieSceneEffect' but node is %s type %s"
					, this->getName().c_str()
					, this->getResourceMovieName().c_str()
					, l.name.c_str()
					, node->getName().c_str()
					, node->getType().c_str()
					);

				return false;
			}
#   endif

			MovieSceneEffect * sceneEffect = static_cast<MovieSceneEffect *>(node);

			sceneEffect->setPropagateNode( layer );
		}

		return true;
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

			Node * node = this->getLayerNode_( layer );

			if( node == nullptr )
			{
				continue;
			}

			this->updateFrameNode_( layer, node, 0, false, true );

			if( layer.parent != 0 && layer.parent != movie_layer_parent_none )
			{
				Node * node_parent = this->getLayerParent_( layer );

				if( node_parent == nullptr )
				{
					LOGGER_ERROR(m_serviceProvider)("Movie::updateParent_ %s resource %s invalid parent node %d"						
						, this->getName().c_str()
						, this->getResourceMovieName().c_str()
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

			Node * node = this->getLayerNode_( layer );

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

			Node * node = this->getLayerNode_( layer );

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

			Animatable * animatable = this->getLayerAnimatable_( layer );

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
	void Movie::pauseAnimation_()
	{
		if( this->isCompile() == false )
		{
			LOGGER_ERROR(m_serviceProvider)("Movie::pauseAnimation_ %s is not compile"
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

			Node * node = this->getLayerNode_( layer );

			if( node == nullptr )
			{
				continue;
			}

			if( layer.isAnimatable() == false )
			{
				continue;
			}

			if( layer.timeRemap == false )
			{
				Animatable * animatable = this->getLayerAnimatable_( layer );

				animatable->pause();
			}
		}
	}
	//////////////////////////////////////////////////////////////////////////
	void Movie::resumeAnimation_()
	{
		if( this->isCompile() == false )
		{
			LOGGER_ERROR(m_serviceProvider)("Movie::resumeAnimation_ %s is not compile"
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

			Node * node = this->getLayerNode_( layer );

			if( node == nullptr )
			{
				continue;
			}

			if( layer.isAnimatable() == false )
			{
				continue;
			}

			if( layer.timeRemap == false )
			{
				Animatable * animatable = this->getLayerAnimatable_( layer );

				animatable->resume();
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

			Node * node = this->getLayerNode_( layer );

			if( node == nullptr )
			{
				continue;
			}

			Animatable * animatable = this->getLayerAnimatable_( layer );

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
		this->stopAnimation_();

		Node::_deactivate();
	}
	//////////////////////////////////////////////////////////////////////////
	void Movie::_setEventListener( const pybind::dict & _embed )
	{
		Node::_setEventListener(_embed);

		this->registerEvent( EVENT_MOVIE_GET_INTERNAL, ("onMovieGetInternal"), _embed );
		//this->registerEvent( EVENT_MOVIE_HIDE_INTERNAL, ("onMovieHideInternal"), _embed );
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

		//if( this->getName() == "Movie_OpenNewspaper"
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

		float speedFactor = this->getAnimationSpeedFactor();
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
		uint32_t frameCount = m_resourceMovie->getFrameCount();

		const mt::vec2f & loopSegment = m_resourceMovie->getLoopSegment();

		if( m_intervalEnd > 0.f )
		{
			frameCount = (uint32_t)((m_intervalStart + m_intervalEnd) / frameDuration + 0.5f) - 1;
		}
		else
		{   
			if( loopSegment.y > 0.f )
			{
				frameCount = (uint32_t)(loopSegment.y / frameDuration + 0.5f) - 1;
			}
		}

		uint32_t lastFrame = m_currentFrame;

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

					bool loop = this->getLoop();
					bool interrupt = this->isInterrupt();

					if( (loop == false && --m_playIterator == 0) || interrupt == true )
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

		uint32_t lastFrame = m_currentFrame;

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
	void Movie::updateForwardFrame_( float _time, uint32_t _beginFrame, uint32_t _endFrame )
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

			Node * node = this->getLayerNode_( layer );

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
#   ifdef _DEBUG
				if( dynamic_cast<MovieNodeExtra *>(node) == nullptr )
				{
					LOGGER_ERROR(m_serviceProvider)("Movie::updateForwardFrame_ %s layer %s must be 'MovieNodeExtra' but node is %s type %s"
						, this->getName().c_str()
						, layer.name.c_str()
						, node->getName().c_str()
						, node->getType().c_str()
						);

					continue;
				}
#   endif

				MovieNodeExtra * extra = static_cast<MovieNodeExtra *>(node);

				extra->movieForwardUpdate( _time, _beginFrame, _endFrame, layer );
			}
		}
	}
	//////////////////////////////////////////////////////////////////////////
	void Movie::updateForwardFrameNode_( float _time, uint32_t _beginFrame, uint32_t _endFrame, const MovieLayer & _layer, Node * _node )
	{
		(void)_time;

		float layerIn = _layer.in;
		float layerOut = _layer.out;

		float frameDuration = m_resourceMovie->getFrameDuration();

		uint32_t indexIn = (uint32_t)((layerIn / frameDuration) + 0.5f);
		uint32_t indexOut = (uint32_t)((layerOut / frameDuration) + 0.5f);

		if( indexIn > _endFrame || indexOut < _beginFrame )
		{
			return;
		}

		if( _beginFrame < indexIn && _endFrame >= indexIn && _endFrame < indexOut )
		{
			uint32_t frameId = _endFrame - indexIn;

			this->updateFrameNode_( _layer, _node, frameId, (_endFrame + 1) < indexOut, true );

			_node->localHide( false );

			if( _layer.isAnimatable() == true )
			{
				Animatable * animatable = this->getLayerAnimatable_( _layer );

				if( _layer.timeRemap == false )
				{
					if( animatable->isPlay() == true )
					{
						animatable->stop();
					}

					uint32_t frame = _endFrame - indexIn;
					float timing = float(frame) * frameDuration;

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
				Animatable * animatable = this->getLayerAnimatable_( _layer );

				if( _layer.timeRemap == false )
				{
					if( animatable->isPlay() == true && _layer.isUnstoppable() == false )
					{
						animatable->stop();

						///Test TestTestTestTest
						uint32_t frame = indexOut - indexIn;
						float timing = frame * frameDuration;                        
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
			uint32_t frameId = _endFrame - indexIn;

			this->updateFrameNode_( _layer, _node, frameId, (_endFrame + 1) < indexOut, false );

			if( _layer.isAnimatable() == true )
			{
				Animatable * animatable = this->getLayerAnimatable_( _layer );

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
	void Movie::updateBackwardFrame_( uint32_t _beginFrame, uint32_t _endFrame )
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

			uint32_t indexIn = (uint32_t)((layerIn / frameDuration) + 0.5f);
			uint32_t indexOut = (uint32_t)((layerOut / frameDuration) + 0.5f);

			if( indexIn > _beginFrame || indexOut < _endFrame )
			{
				continue;
			}

			Node * node = this->getLayerNode_( layer );			

			if( _beginFrame >= indexOut && _endFrame <= indexOut && _endFrame > indexIn )
			{
				uint32_t frameId = _endFrame - indexIn;
				this->updateFrameNode_( layer, node, frameId, false, true );

				node->localHide(false);

				if( layer.isAnimatable() == true )
				{
					Animatable * animatable = this->getLayerAnimatable_( layer );

					if( animatable->isPlay() == true )
					{
						animatable->stop();
					}

					uint32_t frame = _endFrame - indexIn;
					float timing = frame * frameDuration;

					animatable->setTiming( timing );

					float playTime = this->getPlayTime();
					animatable->play( playTime );                                       
				}
			}
			else if( _endFrame <= indexIn && _beginFrame <= indexOut && _beginFrame > indexIn )
			{
				node->localHide(true);

				if( layer.isAnimatable() == true )
				{
					Animatable * animatable = this->getLayerAnimatable_( layer );

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
				uint32_t frameId = _endFrame - indexIn;

				this->updateFrameNode_( layer, node, frameId, false, false );
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
		this->setLocalColorAlpha( _alpha );
	}
	//////////////////////////////////////////////////////////////////////////
	void Movie::updateTiming_()
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

			uint32_t indexIn = (uint32_t)((layerIn / frameDuration) + 0.5f);
			uint32_t indexOut = (uint32_t)((layerOut / frameDuration) + 0.5f);

			Node * node = this->getLayerNode_( layer );

			if( node == nullptr )
			{
				continue;
			}

			if( m_currentFrame >= indexIn && m_currentFrame < indexOut )
			{            
				uint32_t frameId = m_currentFrame - indexIn;

				this->updateFrameNode_( layer, node, frameId, (m_currentFrame + 1) < indexOut, true );

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
					Animatable * animatable = this->getLayerAnimatable_( layer );

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
				node->localHide( true );
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

			uint32_t indexIn = (uint32_t)((layerIn / frameDuration) + 0.5f);
			uint32_t indexOut = (uint32_t)((layerOut / frameDuration) + 0.5f);

			if( indexIn > m_currentFrame || indexOut < m_currentFrame )
			{
				continue;
			}

			Node * node = this->getLayerNode_( layer );			

			if( node == nullptr )
			{
				continue;
			}

			if( m_currentFrame >= indexIn && m_currentFrame < indexOut )
			{                
				uint32_t frameId = m_currentFrame - indexIn;

				this->updateFrameNode_( layer, node, frameId, (m_currentFrame + 1) < indexOut, true );

				node->localHide( false );

				if( layer.isAnimatable() == true )
				{
					Animatable * animatable = this->getLayerAnimatable_( layer );

					if( layer.timeRemap == false )
					{
						if( animatable->isPlay() == true && animatable->getLoop() == false )
						{
							animatable->stop();
						}

						float timing = frameId * frameDuration;

						animatable->setTiming( timing );

						float movieTiming = this->getTiming();

						if( animatable->isPlay() == false )
						{
							animatable->play( movieTiming );
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
			else
			{
				uint32_t frameId = indexOut - indexIn;

				node->localHide( true );

				if( layer.isAnimatable() == true )
				{
					Animatable * animatable = this->getLayerAnimatable_( layer );

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

			uint32_t indexIn = (uint32_t)((layerIn / frameDuration) + 0.5f);
			uint32_t indexOut = (uint32_t)((layerOut / frameDuration) + 0.5f);

			if( indexOut < m_currentFrame )
			{
				continue;
			}

			Node * node = this->getLayerNode_( layer );

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

		this->addChild( m_renderCamera3D );

		m_renderViewport = NODE_SERVICE(m_serviceProvider)
			->createNodeT<RenderViewport>( CONST_STRING(m_serviceProvider, RenderViewport) );

		m_renderViewport->setName( name );

		Viewport vp;
		vp.begin.x = 0.f;
		vp.begin.y = 0.f;

		vp.end.x = camera3D.width;
		vp.end.y = camera3D.height;

		m_renderViewport->setViewport( vp );

		this->addChild( m_renderViewport );
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
			LOGGER_ERROR(m_serviceProvider)("Movie::_setSpeedFactor not compile '%s'"
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

			Animatable * animatable = this->getLayerAnimatable_( layer );

			animatable->setAnimationSpeedFactor( _factor );
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

			Animatable * animatable = this->getLayerAnimatable_( layer );

			animatable->setReverse( _reverse );
		}
	}
	//////////////////////////////////////////////////////////////////////////
	bool Movie::_interrupt( uint32_t _enumerator )
	{
		(void)_enumerator;

		return false;
	}
}