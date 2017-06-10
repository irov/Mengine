#	include "Movie.h"

#	include "Interface/ResourceInterface.h"
#	include "Interface/PrototypeManagerInterface.h"
#   include "Interface/TimelineInterface.h"

#	include "ResourceMovie.h"

#	include "ResourceImageDefault.h"
#	include "ResourceAnimation.h"
#	include "ResourceImageSolid.h"
#   include "ResourceHIT.h"
#   include "ResourceVideo.h"
#   include "ResourceSound.h"
#   include "ResourceShape.h"
#   include "ResourceParticle.h"


#	include "Kernel/Layer.h"
#	include "Kernel/Scene.h"

#	include "Kernel/RenderCameraProjection.h"
#	include "Kernel/RenderViewport.h"
#	include "Kernel/RenderClipplane.h"

#   include "ShapeQuadFixed.h"
#	include "Mesh2D.h"
#   include "TextField.h"
#	include "ParticleEmitter2.h"
#   include "MovieSlot.h"
#   include "MovieNodeExtra.h"
#   include "MovieEvent.h"
#   include "HotSpotImage.h"
#   include "HotSpotShape.h"
#   include "MovieSceneEffect.h"
#	include "MovieInternalObject.h"

#   include "SurfaceImage.h"
#   include "SurfaceImageSequence.h"
#   include "SurfaceVideo.h"
#   include "SurfaceSound.h"

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
		, m_renderCameraProjection(nullptr)
		, m_renderViewport(nullptr)
		, m_parentMovie(false)
		, m_interruptEnd(false)
	{	
	}
	//////////////////////////////////////////////////////////////////////////
	void Movie::setResourceMovie( const ResourceMoviePtr & _resourceMovie )
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
	const ResourceMoviePtr & Movie::getResourceMovie() const
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
	void Movie::getFrameTiming_( float _time, uint32_t & _frame, float & _timing ) const
	{
		float duration = m_resourceMovie->getDuration();

		float crop_timing = mt::crop( _time, 0.f, duration );

		bool loop = this->getLoop();

		float frameDuration = m_resourceMovie->getFrameDuration();

		uint32_t frame = (uint32_t)(crop_timing / frameDuration + 0.5f);

		uint32_t frameCount = m_resourceMovie->getFrameCount();

		if( frameCount == 0 )
		{
			_frame = 0;
			_timing = 0.f;
		}
		else if( loop == true )
		{
			_frame = frame % frameCount;
			_timing = crop_timing - _frame * frameDuration;
		}
		else
		{
			if( frame >= frameCount )
			{
				_frame = frameCount - 1;
				_timing = 0.f;
			}
			else
			{
				_frame = frame;
				_timing = crop_timing - _frame * frameDuration;
			}
		}
	}
	//////////////////////////////////////////////////////////////////////////
	void Movie::_setTiming( float _timing )
	{
		if( m_resourceMovie.empty() == true )
		{
			LOGGER_ERROR( m_serviceProvider )("Movie._setTiming: '%s' not activate"
				, this->getName().c_str()
				);

			return;
		}

		uint32_t frame;
		float timing;
		this->getFrameTiming_( _timing, frame, timing );

		m_currentFrame = frame;
		m_frameTiming = timing;

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
			if( this->isHomeless() == true )
			{
				LOGGER_ERROR( m_serviceProvider )("Movie::_play: '%s' is homeless"
					, this->getName().c_str()
					);
			}
			else
			{
				LOGGER_ERROR( m_serviceProvider )("Movie::_play: '%s' play not activate"
					, this->getName().c_str()
					);
			}		

			return false;
		}

		this->updateAnimatablePlay_();
		
		return true;
	}
	//////////////////////////////////////////////////////////////////////////
	bool Movie::_restart( uint32_t _enumerator, float _time )
	{
		(void)_time;
		(void)_enumerator;

		if( this->isActivate() == false )
		{
			return false;
		}

		this->updateAnimatablePlay_();

		return true;
	}
	//////////////////////////////////////////////////////////////////////////
	void Movie::_pause( uint32_t _enumerator )
	{
		(void)_enumerator;

		this->pauseAnimation_();
	}
	//////////////////////////////////////////////////////////////////////////
	void Movie::_resume( uint32_t _enumerator, float _time )
	{
		(void)_time;
		(void)_enumerator;

		this->resumeAnimation_();
	}
	//////////////////////////////////////////////////////////////////////////
	void Movie::_stop( uint32_t _enumerator )
	{
		this->stopAnimation_();
        
        EVENTABLE_METHOD( this, EVENT_ANIMATABLE_STOP )
            ->onAnimatableStop( _enumerator );
		//EVENTABLE_CALL( m_serviceProvider, this, EVENT_MOVIE_END )(this, _enumerator, false);
		//EVENTABLE_CALL( m_serviceProvider, this, EVENT_ANIMATABLE_END )(this, _enumerator, false);
	}
	//////////////////////////////////////////////////////////////////////////
	void Movie::_end( uint32_t _enumerator )
	{
		this->stopAnimation_();

        EVENTABLE_METHOD( this, EVENT_ANIMATABLE_END )
            ->onAnimatableEnd( _enumerator );
		//EVENTABLE_CALL( m_serviceProvider, this, EVENT_MOVIE_END )(this, _enumerator, true);
		//EVENTABLE_CALL( m_serviceProvider, this, EVENT_ANIMATABLE_END )(this, _enumerator, true);
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
			if( framePack->getLayerShape( _layer.index, _frameId, &shape ) == false )
			{
				return false;
			}

			mesh2D->setFrameShape( shape );
		}

		const MovieLayerFrame & layerFrame = framePack->getLayer( _layer.index );

		bool immutable = layerFrame.immutable;

		if( immutable == true && _first == false )
		{
			return true;
		}

		_interpolate = true;

		MovieFrameSource frame;
		if( _interpolate == true && immutable == false )
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

		mt::vec2f skew( 0.f, 0.f );

		_node->setTransformation(
			frame.position,
			frame.anchorPoint,
			frame.scale,
			skew,
			frame.rotation
			);

		Soundable * soundable = this->getLayerSoundable_( _layer );

		if( soundable == nullptr )
		{
			if( _layer.isMovie() == false )
			{
				_node->setPersonalAlpha( frame.opacity );
			}
			else
			{
				_node->setLocalAlpha( frame.opacity );
			}
		}
		else		
		{
			soundable->setVolume( frame.volume );
		}

		return true;
	}
	//////////////////////////////////////////////////////////////////////////
	void Movie::_setFirstFrame()
	{
		if( m_resourceMovie.empty() == true )
		{
			LOGGER_ERROR( m_serviceProvider )("Movie._setFirstFrame: '%s' not activate"
				, this->getName().c_str()
				);

			return;
		}

		this->setTiming( 0.f );
	}
	//////////////////////////////////////////////////////////////////////////
	void Movie::_setLastFrame()
	{
		if( m_resourceMovie.empty() == true )
		{
			LOGGER_ERROR( m_serviceProvider )("Movie._setLastFrame: '%s' not activate"
				, this->getName().c_str()
				);

			return;
		}

		float duration = m_resourceMovie->getDuration();
		float frameDuration = m_resourceMovie->getFrameDuration();

		this->setTiming( duration - frameDuration );
	}
	//////////////////////////////////////////////////////////////////////////
	bool Movie::addMovieNode_( const MovieLayer & _layer, Node * _node, Animatable * _animatable, Soundable * _soundable, Movie * _movie )
	{		
		if( _layer.hasViewport == true )
		{
			RenderClipplane * clippane = NODE_SERVICE( m_serviceProvider )
				->createNodeT<RenderClipplane *>( CONST_STRING( m_serviceProvider, RenderClipplane ) );

			if( clippane == nullptr )
			{
				return false;
			}

			clippane->setName( _layer.name );
			clippane->setViewport( _layer.viewport );

			_node->setRenderClipplane( clippane );

			m_clipplanes.push_back( clippane );

			this->addChild( clippane );
		}

		Nodies nd;
		nd.node = _node;

		if( _layer.in <= 0.0001f )
		{
			nd.visible = true;
			nd.node->localHide( false );
		}
		else
		{
			nd.visible = false;
			nd.node->localHide( true );
		}

		nd.animatable = _animatable;
		nd.soundable = _soundable;
		nd.movie = _movie;

		nd.child = (_layer.parent != movie_layer_parent_none);

		_node->setName( _layer.name );

		mt::vec2f skew( 0.f, 0.f );

		if( _layer.scale.x < 1.f )
		{
			skew.x = 0.f;
		}
		
		_node->setTransformation(
			_layer.position,
			_layer.anchorPoint,
			_layer.scale,
			skew,
			_layer.rotation
			);

		m_nodies[_layer.index - 1] = nd;

		return true;
	}
	//////////////////////////////////////////////////////////////////////////
	bool Movie::visitMovieLayer( const ConstString & _type, VisitorMovieNode * _visitor )
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

			Movie * movie = this->getLayerMovie_( layer );
			
			if( movie->visitMovieLayer( _type, _visitor ) == false )
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
			LOGGER_ERROR(m_serviceProvider)("Movie.getMovieNode %s resource %s invalid not compile '%s' type '%s'"
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

			if( layer.name != _name || (layer.type != _type && _type.empty() == false) )
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

			Movie * movie = this->getLayerMovie_( layer );

			if( movie->getMovieNode( _name, _type, _node, _movie ) == true )
			{
				return true;
			}
		}

		LOGGER_ERROR(m_serviceProvider)("Movie::getMovieNode: '%s' resource '%s' not found node '%s' type '%s'"
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

			if( layer.name != _name || (layer.type != _type && _type.empty() == false) )
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

			Movie * movie = this->getLayerMovie_( layer );

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

		const MovieLayer * exist_layer;
		if( m_resourceMovie->hasMovieLayer( _name, &exist_layer) == true )
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

			Movie * movie = this->getLayerMovie_( layer );

			if( movie->hasMovieLayer( _name ) == false )
			{
				continue;
			}

			return true;
		}

		return false;
	}
	//////////////////////////////////////////////////////////////////////////
	bool Movie::getMovieClipplane( const ConstString & _name, RenderClipplane ** _clipplane )
	{
		for( TVectorClipplane::iterator
			it = m_clipplanes.begin(),
			it_end = m_clipplanes.end();
			it != it_end;
			++it )
		{
			RenderClipplane * clipplane = *it;

			const ConstString & name = clipplane->getName();

			if( name != _name )
			{
				continue;
			}

			*_clipplane = clipplane;

			return true;
		}

		return false;
	}
	//////////////////////////////////////////////////////////////////////////
	bool Movie::hasMovieClipplane( const ConstString & _name )
	{
		for( TVectorClipplane::const_iterator
			it = m_clipplanes.begin(),
			it_end = m_clipplanes.end();
			it != it_end;
			++it )
		{
			const RenderClipplane * clipplane = *it;

			const ConstString & name = clipplane->getName();

			if( name != _name )
			{
				continue;				
			}

			return true;
		}

		return false;
	}
	//////////////////////////////////////////////////////////////////////////
	void Movie::setEnableMovieLayers( const ConstString & _name, bool _enable )
	{
		if( m_resourceMovie == nullptr )
		{
			LOGGER_ERROR( m_serviceProvider )("Movie::setEnableMovieLayers %s resource %s invalid get layer %s not compile"
				, this->getName().c_str()
				, this->getResourceMovieName().c_str()
				, _name.c_str()
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

			if( layer.name != _name )
			{
				continue;
			}
			
			this->setEnableLayer_( layer, _enable );
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

			Movie * movie = this->getLayerMovie_( layer );

			movie->setEnableMovieLayers( _name, _enable );
		}
	}
	//////////////////////////////////////////////////////////////////////////
	bool Movie::setEnableMovieLayer( const ConstString & _name, bool _enable )
	{
		if( m_resourceMovie == nullptr )
		{
			LOGGER_ERROR( m_serviceProvider )("Movie::setEnableMovieLayer %s resource %s invalid get layer %s not compile"
				, this->getName().c_str()
				, this->getResourceMovieName().c_str()
				, _name.c_str()
				);

			return false;
		}

		const MovieLayer * exist_layer;
		if( m_resourceMovie->hasMovieLayer( _name, &exist_layer) == true )
		{
			this->setEnableLayer_( *exist_layer, _enable );

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

			Movie * movie = this->getLayerMovie_( layer );

			if( movie->setEnableMovieLayer( _name, _enable ) == false )
			{
				continue;
			}

			return true;
		}

		LOGGER_ERROR( m_serviceProvider )("Movie::setEnableMovieLayer %s resource %s not found layer %s"
			, this->getName().c_str()
			, this->getResourceMovieName().c_str()
			, _name.c_str()
			);

		return false;
	}
	//////////////////////////////////////////////////////////////////////////
	bool Movie::getEnableMovieLayer( const ConstString & _name, bool & _enable )
	{
		if( m_resourceMovie == nullptr )
		{
			LOGGER_ERROR( m_serviceProvider )("Movie::getEnableMovieLayer %s resource %s invalid get layer %s not compile"
				, this->getName().c_str()
				, this->getResourceMovieName().c_str()
				, _name.c_str()
				);

			return false;
		}

		const MovieLayer * exist_layer;
		if( m_resourceMovie->hasMovieLayer( _name, &exist_layer) == true )
		{
			_enable = this->getEnableLayer_( *exist_layer);

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

			Movie * movie = this->getLayerMovie_( layer );

			if( movie->getEnableMovieLayer( _name, _enable ) == false )
			{
				continue;
			}

			return true;
		}

		LOGGER_ERROR( m_serviceProvider )("Movie::getEnableMovieLayer %s resource %s not found layer %s"
			, this->getName().c_str()
			, this->getResourceMovieName().c_str()
			, _name.c_str()
			);

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

			return false;
		}

		const MovieLayer * exist_layer;
		if( m_resourceMovie->hasMovieLayer( _name, &exist_layer) == true )
		{	
			*_layer = exist_layer;
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

			Movie * movie = this->getLayerMovie_( layer );

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
				if( layer.shape == false )
				{
					if( this->createMovieImageSolid_( layer ) == false )
					{
						return false;
					}
				}
				else
				{
					if( this->createMovieMesh2DSolid_( layer ) == false )
					{
						return false;
					}
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
			else if( layer.type == CONST_STRING(m_serviceProvider, ParticleEmitter2) )
			{
				if( this->createMovieParticleEmitter2_( layer ) == false )
				{
					return false;
				}
			}
			else if( layer.type == CONST_STRING( m_serviceProvider, MovieParticle ) )
			{
				//Empty
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

		if( m_resourceMovie->hasOffsetPoint() == false )
		{
			if( m_resourceMovie->hasAnchorPoint() == true )
			{
				const mt::vec3f & anchorPoint = m_resourceMovie->getAnchorPoint();

				this->setOrigin( anchorPoint );
			}
		}
		else
		{
			if( m_resourceMovie->hasAnchorPoint() == true )
			{
				const mt::vec3f & offsetPoint = m_resourceMovie->getOffsetPoint();

				const mt::vec3f & anchorPoint = m_resourceMovie->getAnchorPoint();

				this->setOrigin( offsetPoint );
							
				mt::vec3f offset = offsetPoint - anchorPoint;

				this->setLocalPosition( offset );
			}
			else
			{
				const mt::vec3f & offsetPoint = m_resourceMovie->getOffsetPoint();

				this->setLocalPosition( offsetPoint );
			}			
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
			->createNodeT<MovieSlot *>( CONST_STRING( m_serviceProvider, MovieSlot ) );

		if( layer_slot == nullptr )
		{
			return false;
		}

		layer_slot->setMovieName( m_name );		

		if( this->addMovieNode_( _layer, layer_slot, nullptr, nullptr, nullptr ) == false )
		{
			return false;
		}

		return true;
	}
	//////////////////////////////////////////////////////////////////////////
	bool Movie::createMovieSceneEffect_( const MovieLayer & _layer )
	{
		MovieSceneEffect * sceneeffect_slot = NODE_SERVICE(m_serviceProvider)
			->createNodeT<MovieSceneEffect *>( CONST_STRING( m_serviceProvider, MovieSceneEffect ) );

		if( sceneeffect_slot == nullptr )
		{
			return false;
		}

		if( this->addMovieNode_( _layer, sceneeffect_slot, nullptr, nullptr, nullptr ) == false )
		{
			return false;
		}

		return true;
	}
	//////////////////////////////////////////////////////////////////////////
	bool Movie::createMovieNullObject_( const MovieLayer & _layer )
	{
		Node * layer_slot = NODE_SERVICE(m_serviceProvider)
			->createNodeT<Node *>( CONST_STRING( m_serviceProvider, Node ) );

		if( layer_slot == nullptr )
		{
			return false;
		}

		if( this->addMovieNode_( _layer, layer_slot, nullptr, nullptr, nullptr ) == false )
		{
			return false;
		}

		return true;
	}
	//////////////////////////////////////////////////////////////////////////
	bool Movie::createMovieImage_( const MovieLayer & _layer )
	{
		ResourceImagePtr resourceImage = RESOURCE_SERVICE(m_serviceProvider)
			->getResourceReferenceT<ResourceImagePtr>( _layer.source );

		if( resourceImage == nullptr )
		{
			return false;
		}

        SurfaceImagePtr surface = PROTOTYPE_SERVICE( m_serviceProvider )
            ->generatePrototype( CONST_STRING( m_serviceProvider, Surface ), CONST_STRING( m_serviceProvider, SurfaceImage ) );

        if( surface == nullptr )
        {
            return false;
        }

        surface->setResourceImage( resourceImage );

		if( this->setupBlendingMode_( _layer, surface.get() ) == false )
		{
			return false;
		}

        ShapeQuadFixed * layer_sprite = NODE_SERVICE( m_serviceProvider )
            ->createNodeT<ShapeQuadFixed *>( CONST_STRING( m_serviceProvider, ShapeQuadFixed ) );

        if( layer_sprite == nullptr )
        {
            return false;
        }

        layer_sprite->setSurface( surface );

		if( this->addMovieNode_( _layer, layer_sprite, nullptr, nullptr, nullptr ) == false )
		{
			return false;
		}

		return true;
	}
	//////////////////////////////////////////////////////////////////////////
	bool Movie::createMovieMesh2D_( const MovieLayer & _layer )
	{
		ResourceImagePtr resourceImage = RESOURCE_SERVICE( m_serviceProvider )
			->getResourceReferenceT<ResourceImagePtr>( _layer.source );

		if( resourceImage == nullptr )
		{
			return false;
		}

		Mesh2D * layer_mesh = NODE_SERVICE(m_serviceProvider)
			->createNodeT<Mesh2D *>( CONST_STRING( m_serviceProvider, Mesh2D ) );

		if( layer_mesh == nullptr )
		{
			return false;
		}

		layer_mesh->setResourceImage( resourceImage );

		if( this->setupBlendingMode_( _layer, layer_mesh ) == false )
		{
			return false;
		}

		if( this->addMovieNode_( _layer, layer_mesh, nullptr, nullptr, nullptr ) == false )
		{
			return false;
		}

		return true;
	}
	//////////////////////////////////////////////////////////////////////////
	bool Movie::createMovieImageSolid_( const MovieLayer & _layer )
	{
		ResourceImageSolidPtr resource = RESOURCE_SERVICE( m_serviceProvider )
			->getResourceT<ResourceImageSolidPtr>( _layer.source );

		if( resource == nullptr )
		{
			LOGGER_ERROR( m_serviceProvider )("Movie.createMovieImageSolid_: %s resource %s can't compile sprite '%s' imageSolid resource = NULL"
				, this->getName().c_str()
				, this->getResourceMovieName().c_str()
				, _layer.name.c_str()
				);

			return false;
		}

        SurfaceImagePtr surface = PROTOTYPE_SERVICE( m_serviceProvider )
            ->generatePrototype( CONST_STRING( m_serviceProvider, Surface ), CONST_STRING( m_serviceProvider, SurfaceImage ) );

        if( surface == nullptr )
        {
            return false;
        }

        surface->setResourceImage( resource );

        if( this->setupBlendingMode_( _layer, surface.get() ) == false )
        {
            return false;
        }

		ShapeQuadFixed * layer_sprite = NODE_SERVICE( m_serviceProvider )
			->createNodeT<ShapeQuadFixed *>( CONST_STRING( m_serviceProvider, ShapeQuadFixed ) );

		if( layer_sprite == nullptr )
		{
			return false;
		}

        layer_sprite->setSurface( surface );

		if( this->addMovieNode_( _layer, layer_sprite, nullptr, nullptr, nullptr ) == false )
		{
			return false;
		}

		return true;
	}
	//////////////////////////////////////////////////////////////////////////
	bool Movie::createMovieMesh2DSolid_( const MovieLayer & _layer )
	{
		ResourceImageSolidPtr resourceImage = RESOURCE_SERVICE( m_serviceProvider )
			->getResourceReferenceT<ResourceImageSolidPtr>( _layer.source );

		if( resourceImage == nullptr )
		{
			return false;
		}

		Mesh2D * layer_mesh = NODE_SERVICE( m_serviceProvider )
			->createNodeT<Mesh2D *>( CONST_STRING( m_serviceProvider, Mesh2D ) );

		if( layer_mesh == nullptr )
		{
			return false;
		}

		layer_mesh->setResourceImage( resourceImage );

		if( this->setupBlendingMode_( _layer, layer_mesh ) == false )
		{
			return false;
		}

		if( this->addMovieNode_( _layer, layer_mesh, nullptr, nullptr, nullptr ) == false )
		{
			return false;
		}

		return true;
	}
	//////////////////////////////////////////////////////////////////////////
	bool Movie::createMovieSocketImage_( const MovieLayer & _layer )
	{
		ResourceHITPtr resourceHIT = RESOURCE_SERVICE( m_serviceProvider )
			->getResourceReferenceT<ResourceHITPtr>( _layer.source );

		if( resourceHIT == nullptr )
		{
			return false;
		}

        HotSpotImage * layer_hotspotimage = NODE_SERVICE( m_serviceProvider )
            ->createNodeT<HotSpotImage *>( CONST_STRING( m_serviceProvider, HotSpotImage ) );

        if( layer_hotspotimage == nullptr )
        {
            return false;
        }
		
		layer_hotspotimage->setResourceHIT( resourceHIT );

		if( this->addMovieNode_( _layer, layer_hotspotimage, nullptr, nullptr, nullptr ) == false )
		{
			return false;
		}

		return true;
	}
	//////////////////////////////////////////////////////////////////////////
	bool Movie::createMovieSocketShape_( const MovieLayer & _layer )
	{
		HotSpotShape * layer_hotspotshape = NODE_SERVICE(m_serviceProvider)
			->createNodeT<HotSpotShape *>( CONST_STRING( m_serviceProvider, HotSpotShape ) );

		if( layer_hotspotshape == nullptr )
		{
			return false;
		}

		ResourceShapePtr resourceShape = RESOURCE_SERVICE( m_serviceProvider )
			->getResourceReferenceT<ResourceShapePtr>( _layer.source );

		if( resourceShape == nullptr )
		{
			return false;
		}

		layer_hotspotshape->setResourceShape( resourceShape );
		
		if( this->addMovieNode_( _layer, layer_hotspotshape, nullptr, nullptr, nullptr ) == false )
		{
			return false;
		}

		return true;
	}
	//////////////////////////////////////////////////////////////////////////
	bool Movie::createMovieAnimation_( const MovieLayer & _layer )
	{
        ResourceAnimationPtr resourceAnimation = RESOURCE_SERVICE( m_serviceProvider )
			->getResourceReferenceT<ResourceAnimationPtr>( _layer.source );

		if( resourceAnimation == nullptr )
		{
			return false;
		}

        SurfaceImageSequencePtr surface = PROTOTYPE_SERVICE( m_serviceProvider )
            ->generatePrototype( CONST_STRING( m_serviceProvider, Surface ), CONST_STRING( m_serviceProvider, SurfaceImageSequence ) );

        if( surface == nullptr )
        {
            return false;
        }

		surface->setName(_layer.name);
        surface->setResourceAnimation( resourceAnimation );

        surface->setIntervalStart( _layer.startInterval );
        surface->setPlayCount( _layer.playCount );
        surface->setStretch( _layer.stretch );
        surface->setLoop( _layer.loop );

		if( this->setupBlendingMode_( _layer, surface.get() ) == false )
		{
			return false;
		}

        ShapeQuadFixed * layer_animation = NODE_SERVICE( m_serviceProvider )
            ->createNodeT<ShapeQuadFixed *>( CONST_STRING( m_serviceProvider, ShapeQuadFixed ) );

        if( layer_animation == nullptr )
        {
            return false;
        }

		layer_animation->setName(_layer.name);
        layer_animation->setSurface( surface );

		if( this->addMovieNode_( _layer, layer_animation, surface.get(), nullptr, nullptr ) == false )
		{
			return false;
		}

		return true;
	}
	//////////////////////////////////////////////////////////////////////////
	bool Movie::createMovieMovie_( const MovieLayer & _layer )
	{
		Movie * layer_movie = NODE_SERVICE(m_serviceProvider)
			->createNodeT<Movie *>( CONST_STRING( m_serviceProvider, Movie ) );

		if( layer_movie == nullptr )
		{
			return false;
		}

		ResourceMoviePtr resourceMovie = RESOURCE_SERVICE( m_serviceProvider )
			->getResourceReferenceT<ResourceMoviePtr>( _layer.source );

		if( resourceMovie == nullptr )
		{
			return false;
		}

		layer_movie->setResourceMovie( resourceMovie );

		layer_movie->setIntervalStart( _layer.startInterval );

		layer_movie->setPlayCount( _layer.playCount );
		layer_movie->setStretch( _layer.stretch );
		layer_movie->setLoop( _layer.loop );

		layer_movie->setParentMovie( true );

		if( this->addMovieNode_( _layer, layer_movie, layer_movie, nullptr, layer_movie ) == false )
		{
			return false;
		}

		return true;
	}
	//////////////////////////////////////////////////////////////////////////
	bool Movie::createMovieSubMovie_( const MovieLayer & _layer )
	{
		Movie * layer_movie = NODE_SERVICE(m_serviceProvider)
			->createNodeT<Movie *>( CONST_STRING( m_serviceProvider, Movie ) );

		if( layer_movie == nullptr )
		{
			return false;
		}

		ResourceMoviePtr resourceMovie = RESOURCE_SERVICE( m_serviceProvider )
			->getResourceReferenceT<ResourceMoviePtr>( _layer.source );

		if( resourceMovie == nullptr )
		{
			return false;
		}

		layer_movie->setResourceMovie( resourceMovie );

		layer_movie->setIntervalStart( _layer.startInterval );

		layer_movie->setPlayCount( _layer.playCount );
		layer_movie->setStretch( _layer.stretch );
		layer_movie->setLoop( _layer.loop );

		layer_movie->setParentMovie( true );

		if( this->addMovieNode_( _layer, layer_movie, layer_movie, nullptr, layer_movie ) == false )
		{
			return false;
		}

		return true;
	}
	//////////////////////////////////////////////////////////////////////////
	bool Movie::createMovieInternalObject_( const MovieLayer & _layer )
	{
		MovieInternalObject * movie_internal = NODE_SERVICE(m_serviceProvider)
			->createNodeT<MovieInternalObject *>( CONST_STRING( m_serviceProvider, MovieInternalObject ) );

		if( movie_internal == nullptr )
		{
			return false;
		}

		ResourceInternalObjectPtr resourceInternalObject = RESOURCE_SERVICE( m_serviceProvider )
			->getResourceT<ResourceInternalObjectPtr>( _layer.source );

		if( resourceInternalObject == nullptr )
		{
			return false;
		}

		movie_internal->setMovie( this );
		movie_internal->setResourceInternalObject( resourceInternalObject );

		if( this->addMovieNode_( _layer, movie_internal, nullptr, nullptr, nullptr ) == false )
		{
			return false;
		}

		return true;
	}
	//////////////////////////////////////////////////////////////////////////
	bool Movie::createMovieVideo_( const MovieLayer & _layer )
	{
		ResourceVideoPtr resourceVideo = RESOURCE_SERVICE( m_serviceProvider )
			->getResourceReferenceT<ResourceVideoPtr>( _layer.source );

		if( resourceVideo == nullptr )
		{
			return false;
		}

		SurfaceVideoPtr surface = PROTOTYPE_SERVICE( m_serviceProvider )
			->generatePrototype( CONST_STRING( m_serviceProvider, Surface ), CONST_STRING( m_serviceProvider, SurfaceVideo ) );

		if( surface == nullptr )
		{
			return false;
		}

        surface->setResourceVideo( resourceVideo );

        surface->setIntervalStart( _layer.startInterval );
        surface->setPlayCount( _layer.playCount );
        surface->setStretch( _layer.stretch );
        surface->setLoop( _layer.loop );

		if( this->setupBlendingMode_( _layer, surface.get() ) == false )
		{
			return false;
		}

        ShapeQuadFixed * layer_video = NODE_SERVICE( m_serviceProvider )
            ->createNodeT<ShapeQuadFixed *>( CONST_STRING( m_serviceProvider, ShapeQuadFixed ) );

        if( layer_video == nullptr )
        {
            return false;
        }

		layer_video->setSurface( surface );

		if( this->addMovieNode_( _layer, layer_video, surface.get(), nullptr, nullptr ) == false )
		{
			return false;
		}

		return true;
	}
	//////////////////////////////////////////////////////////////////////////
	bool Movie::createMovieSound_( const MovieLayer & _layer )
	{
		SoundEmitter * layer_sound = NODE_SERVICE(m_serviceProvider)
			->createNodeT<SoundEmitter *>( CONST_STRING( m_serviceProvider, SoundEmitter ) );

		if( layer_sound == nullptr )
		{
			return false;
		}

		ResourceSoundPtr resourceSound = RESOURCE_SERVICE( m_serviceProvider )
			->getResourceReferenceT<ResourceSoundPtr>( _layer.source );

		if( resourceSound == nullptr )
		{
			return false;
		}

		SurfaceSoundPtr surfaceSound = PROTOTYPE_SERVICE( m_serviceProvider )
			->generatePrototype( CONST_STRING( m_serviceProvider, Surface ), CONST_STRING( m_serviceProvider, SurfaceSound ) );

		if( surfaceSound == nullptr )
		{
			return false;
		}

		surfaceSound->setResourceSound( resourceSound );

		surfaceSound->setIntervalStart( _layer.startInterval );

		surfaceSound->setPlayCount( _layer.playCount );
		surfaceSound->setStretch( _layer.stretch );
		surfaceSound->setLoop( _layer.loop );

		layer_sound->setSurfaceSound( surfaceSound );

		if( this->addMovieNode_( _layer, layer_sound, surfaceSound.get(), surfaceSound.get(), nullptr ) == false )
		{
			return false;
		}

		return true;
	}
	//////////////////////////////////////////////////////////////////////////
	bool Movie::createMovieSoundId_( const MovieLayer & _layer )
	{
		SoundEmitter * layer_sound = NODE_SERVICE(m_serviceProvider)
			->createNodeT<SoundEmitter *>( CONST_STRING( m_serviceProvider, SoundEmitter ) );

		if( layer_sound == nullptr )
		{
			return false;
		}

		ResourceSoundPtr resourceSound = RESOURCE_SERVICE( m_serviceProvider )
			->getResourceReferenceT<ResourceSoundPtr>( _layer.name );

		if( resourceSound == nullptr )
		{
			return false;
		}

		SurfaceSoundPtr surfaceSound = PROTOTYPE_SERVICE( m_serviceProvider )
			->generatePrototype( CONST_STRING( m_serviceProvider, Surface ), CONST_STRING( m_serviceProvider, SurfaceSound ) );

		if( surfaceSound == nullptr )
		{
			return false;
		}

		surfaceSound->setResourceSound( resourceSound );
		surfaceSound->setIntervalStart( _layer.startInterval );

		surfaceSound->setPlayCount( 1 );
		surfaceSound->setStretch( _layer.stretch );
		surfaceSound->setLoop( _layer.loop );

		layer_sound->setSurfaceSound( surfaceSound );

		if( this->addMovieNode_( _layer, layer_sound, surfaceSound.get(), surfaceSound.get(), nullptr ) == false )
		{
			return false;
		}

		return true;
	}
	//////////////////////////////////////////////////////////////////////////
	bool Movie::createMovieText_( const MovieLayer & _layer )
	{
		TextField * layer_text = NODE_SERVICE(m_serviceProvider)
			->createNodeT<TextField *>( CONST_STRING( m_serviceProvider, TextField ) );

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

		if( _layer.hasParam( MOVIE_LAYER_PARAM_PARTICLE_TRANSLATE ) == true )
		{
			layer_text->setPixelsnap( false );
		}

		if( this->addMovieNode_( _layer, layer_text, nullptr, nullptr, nullptr ) == false )
		{
			return false;
		}

		return true;
	}
	//////////////////////////////////////////////////////////////////////////
	bool Movie::createMovieTextCenter_( const MovieLayer & _layer )
	{
		TextField * layer_text = NODE_SERVICE(m_serviceProvider)
			->createNodeT<TextField *>( CONST_STRING( m_serviceProvider, TextField ) );

		if( layer_text == nullptr )
		{
			return false;
		}

		layer_text->setTextID( _layer.name ); //Name = TextID
		layer_text->setHorizontalCenterAlign();

		if( _layer.hasParam( MOVIE_LAYER_PARAM_PARTICLE_TRANSLATE ) == true )
		{
			layer_text->setPixelsnap( false );
		}

		if( this->addMovieNode_( _layer, layer_text, nullptr, nullptr, nullptr ) == false )
		{
			return false;
		}

		return true;
	}
	//////////////////////////////////////////////////////////////////////////
	bool Movie::createMovieExtraSprite_( const MovieLayer & _layer )
	{
		ResourceImagePtr resourceImage = RESOURCE_SERVICE( m_serviceProvider )
			->getResourceReferenceT<ResourceImagePtr>( _layer.name );

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

        SurfaceImagePtr surface = PROTOTYPE_SERVICE( m_serviceProvider )
            ->generatePrototype( CONST_STRING( m_serviceProvider, Surface ), CONST_STRING( m_serviceProvider, SurfaceImage ) );

        if( surface == nullptr )
        {
            return false;
        }

        surface->setResourceImage( resourceImage );

		if( this->setupBlendingMode_( _layer, surface.get() ) == false )
		{
			return false;
		}

        ShapeQuadFixed * layer_sprite = NODE_SERVICE( m_serviceProvider )
            ->createNodeT<ShapeQuadFixed *>( CONST_STRING( m_serviceProvider, ShapeQuadFixed ) );

        if( layer_sprite == nullptr )
        {
            LOGGER_ERROR( m_serviceProvider )("Movie::createMovieSprite_ %s resource %s layer %s invalid create 'Sprite'"
                , this->getName().c_str()
                , this->getResourceMovieName().c_str()
                , _layer.name.c_str()
                );

            return false;
        }

        layer_sprite->setSurface( surface );

		if( this->addMovieNode_( _layer, layer_sprite, nullptr, nullptr, nullptr ) == false )
		{
			return false;
		}

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
			->createNodeT<MovieEvent *>( CONST_STRING( m_serviceProvider, MovieEvent ) );

		layer_event->setResourceMovie( m_resourceMovie );

		if( this->addMovieNode_( _layer, layer_event, nullptr, nullptr, nullptr ) == false )
		{
			return false;
		}

		return true;
	}
	//////////////////////////////////////////////////////////////////////////
	bool Movie::createMovieParticleEmitter2_( const MovieLayer & _layer )
	{
		ParticleEmitter2 * layer_particles = NODE_SERVICE(m_serviceProvider)
			->createNodeT<ParticleEmitter2 *>( CONST_STRING( m_serviceProvider, ParticleEmitter2 ) );

		if( layer_particles == nullptr )
		{
			return false;
		}

		ResourceParticlePtr resourceParticle = RESOURCE_SERVICE( m_serviceProvider )
			->getResourceReferenceT<ResourceParticlePtr>( _layer.source );

		if( resourceParticle == nullptr )
		{
			return false;
		}

		layer_particles->setResourceParticle( resourceParticle );

		layer_particles->setIntervalStart( _layer.startInterval );
		layer_particles->setPlayCount( _layer.playCount );
		layer_particles->setStretch( _layer.stretch );
		layer_particles->setLoop( _layer.loop );

		layer_particles->setEmitterPositionProviderOriginOffset( -mt::vec3f( 1024.f, 1024.f, 0.f ) );

		if( _layer.hasParam( MOVIE_LAYER_PARAM_PARTICLE_TRANSLATE ) == true )
		{
			layer_particles->setEmitterPositionRelative( true );
			layer_particles->setEmitterCameraRelative( false );
			layer_particles->setEmitterTranslateWithParticle( false );
		}
		else
		{
			layer_particles->setEmitterPositionRelative( false );
			layer_particles->setEmitterCameraRelative( false );
			layer_particles->setEmitterTranslateWithParticle( true );
		}

		if( this->addMovieNode_( _layer, layer_particles, layer_particles, nullptr, nullptr ) == false )
		{
			return false;
		}

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

		bool loop = this->getLoop();
		this->updateLoop_( loop );

		float animationSpeedFactor = this->getAnimationSpeedFactor();
		this->updateAnimationSpeedFactor_( animationSpeedFactor );

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
				node->setRenderCamera( m_renderCameraProjection );
				node->setRenderViewport( m_renderViewport );
			}
		}
	}
	//////////////////////////////////////////////////////////////////////////
	static Node * s_findNodeScene( Node * _node )
	{
		Node * parent = _node->getParent();

		while( parent != nullptr )
		{
			if( dynamic_cast<Layer *>(parent) != nullptr )
			{
				return parent;
			}

			parent = parent->getParent();
		}

		return nullptr;
	}
	//////////////////////////////////////////////////////////////////////////
	bool Movie::setupSceneEffect_()
	{
		const TVectorMovieLayers & layers = m_resourceMovie->getLayers();

		Node * parent = s_findNodeScene( this );

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
				LOGGER_ERROR( m_serviceProvider )("Movie::compileMovieText_ %s resource %s layer %s must be 'MovieSceneEffect' but node is %s type %s"
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

			sceneEffect->setPropagateNode( parent );
		}

		return true;
	}
	//////////////////////////////////////////////////////////////////////////
	void Movie::enableSceneEffect_( bool _value )
	{
		const TVectorMovieLayers & layers = m_resourceMovie->getLayers();
		
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

			MovieSceneEffect * sceneEffect = static_cast<MovieSceneEffect *>(node);

			sceneEffect->enablePropagate( _value );
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

			Node * node = this->getLayerNode_( layer );

			if( node == nullptr )
			{
				continue;
			}

			if( this->updateFrameNode_( layer, node, 0, false, true ) == false )
			{
				return false;
			}

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
				this->setVisibleLayer_( layer, false );
			}

			if( layer.isAnimatable() == false )
			{
				continue;
			}

			if( layer.isSubMovie() == true )
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

			if( layer.isSubMovie() == true )
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

			if( layer.isSubMovie() == true )
			{
				continue;
			}

			if( layer.timeRemap == false )
			{
				Animatable * animatable = this->getLayerAnimatable_( layer );

				float time = TIMELINE_SERVICE( m_serviceProvider )
					->getTime();

				animatable->resume( time );
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

			if( layer.isSubMovie() == true )
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
		this->enableSceneEffect_( true );

		return true;
	}
	//////////////////////////////////////////////////////////////////////////
	void Movie::_deactivate()
	{
		this->stopAnimation_();
		this->enableSceneEffect_( false );

		Node::_deactivate();
	}
	//////////////////////////////////////////////////////////////////////////
	void Movie::_afterActivate()
	{ 
		Node::_afterActivate();

		bool autoPlay = this->getAutoPlay();

		if( autoPlay == true )
		{
			float time = TIMELINE_SERVICE( m_serviceProvider )
				->getTime();

			if( this->play( time ) == 0 )
			{
				LOGGER_ERROR( m_serviceProvider )("Movie::_afterActivate '%s' resource '%s' auto play return 0"
					, this->getName().c_str()
					, this->m_resourceMovie->getName().c_str()
					);

				return;
			}
		}
		else
		{
			this->updateTiming_();
		}
	}
    //////////////////////////////////////////////////////////////////////////
    void Movie::_destroy()
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

            if( ns.child == true )
            {
                continue;
            }

            ns.node->destroy();
        }

        Node::_destroy();
    }
	//////////////////////////////////////////////////////////////////////////
	void Movie::_update( float _current, float _timing )
	{
		if( this->isPlay() == false )
		{
			return;
		}

		//if( this->getName() == "Movie_Effect"
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

		float speedFactor = this->getAnimationSpeedFactor();
		float scretch = this->getStretch();
		float realTiming = _timing * speedFactor / scretch;

		m_frameTiming += realTiming;

		this->updateForward_( _current );
	}
	//////////////////////////////////////////////////////////////////////////
	void Movie::updateForward_( float _time )
	{
		if( m_interruptEnd == true )
		{
			for( TVectorNodies::const_iterator
				it = m_nodies.begin(),
				it_end = m_nodies.end();
			it != it_end;
			++it )
			{
				const Nodies & node = *it;

				if( node.animatable == nullptr )
				{
					continue;
				}

				if( node.animatable->isInterrupt() == true )
				{
					return;
				}
			}

			m_interruptEnd = false;

			this->end();

			return;
		}

		float frameDuration = m_resourceMovie->getFrameDuration();
		uint32_t frameCount = m_resourceMovie->getFrameCount();

		const mt::vec2f & loopSegment = m_resourceMovie->getLoopSegment();

		float intervalStart = this->getIntervalStart();
		float intervalBegin = this->getIntervalBegin();
		float intervalEnd = this->getIntervalEnd();

		if( intervalEnd > 0.f )
		{
			frameCount = (uint32_t)((intervalEnd - intervalBegin - intervalStart) / frameDuration + 0.5f);
		}
		else
		{   
			if( loopSegment.y > 0.f )
			{
				frameCount = (uint32_t)(loopSegment.y / frameDuration + 0.5f);
			}
		}

		uint32_t lastFrame = m_currentFrame;

		while( m_frameTiming >= frameDuration )
		{
			m_frameTiming -= frameDuration;

			++m_currentFrame;

			if( m_currentFrame == frameCount - 1 )
			{
				this->updateForwardFrame_( _time, lastFrame, frameCount - 1 );

				bool loop = this->getLoop();
				bool interrupt = this->isInterrupt();

				if( (loop == false && --m_playIterator == 0) || interrupt == true )
				{
					lastFrame = frameCount - 1;
					m_currentFrame = frameCount - 1;

					if( interrupt == false )
					{
						this->end();
					}
					else
					{
						m_interruptEnd = true;
					}

					return;
				}
				else
				{
					float beginTiming = m_frameTiming + loopSegment.x;

					this->setTiming( beginTiming );

					lastFrame = m_currentFrame;

					this->updateAnimatablePlay_();
				}
			}
		}

		this->updateForwardFrame_( _time, lastFrame, m_currentFrame );
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

		if( this->isLastCicle() == false && _layer.loop == true )
		{
			uint32_t frameId = _endFrame - indexIn;

			this->updateFrameNode_( _layer, _node, frameId, true, false );

			if( _layer.isAnimatable() == true )
			{
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

					Animatable * animatable = this->getLayerAnimatable_( _layer );

					animatable->setTiming( timing );
				}
			}
		}
		else
		{
			if( _beginFrame < indexIn && _endFrame >= indexIn && _endFrame < indexOut )
			{
				uint32_t frameId = _endFrame - indexIn;

				this->updateFrameNode_( _layer, _node, frameId, (_endFrame + 1) < indexOut, true );

				this->setVisibleLayer_( _layer, true );

				if( _layer.isAnimatable() == true && _layer.isSubMovie() == false )
				{
					Animatable * animatable = this->getLayerAnimatable_( _layer );

					if( _layer.timeRemap == false )
					{
						if( animatable->isPlay() == true )
						{
							animatable->stop();
						}

						uint32_t frame = _endFrame - indexIn;
						float timing = float( frame ) * frameDuration;

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
				this->setVisibleLayer_( _layer, false );

				if( _layer.isAnimatable() == true )
				{
					Animatable * animatable = this->getLayerAnimatable_( _layer );

					if( _layer.timeRemap == false )
					{
						if( animatable->isPlay() == true && _layer.isUnstoppable() == false )
						{
							animatable->stop();

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

				if( layer.switcher == true && m_currentFrame + 1 == indexOut )
				{
					this->setVisibleLayer_( layer, false );
				}
				else
				{
					this->updateFrameNode_( layer, node, frameId, (m_currentFrame + 1) < indexOut, true );

					this->setVisibleLayer_( layer, true );
				}

				if( layer.isAnimatable() == true && layer.isSubMovie() == false )
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
				this->setVisibleLayer_( layer, false );
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
				
				if( layer.switcher == true && m_currentFrame + 1 == indexOut )
				{
					this->setVisibleLayer_( layer, false );
				}
				else
				{
					this->updateFrameNode_( layer, node, frameId, (m_currentFrame + 1) < indexOut, true );

					this->setVisibleLayer_( layer, true );
				}

				if( layer.isAnimatable() == true && layer.isSubMovie() == false )
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

						//float movieTiming = this->getTiming();

						if( animatable->isPlay() == false )
						{
							float playTime = TIMELINE_SERVICE( m_serviceProvider )
								->getTime();

							animatable->play( playTime );
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

				this->setVisibleLayer_( layer, false );

				if( layer.isAnimatable() == true && layer.isSubMovie() == false )
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
				this->setVisibleLayer_( layer, false );
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

		m_renderCameraProjection = NODE_SERVICE(m_serviceProvider)
			->createNodeT<RenderCameraProjection *>( CONST_STRING( m_serviceProvider, RenderCameraProjection ) );

		const ConstString & name = this->getName();
		m_renderCameraProjection->setName( name );

		const MovieLayerCamera3D & camera3D = m_resourceMovie->getCamera3D();

		m_renderCameraProjection->setCameraPosition( camera3D.cameraPosition );
		m_renderCameraProjection->setCameraDirection( camera3D.cameraInterest - camera3D.cameraPosition );
		m_renderCameraProjection->setCameraFOV( camera3D.cameraFOV );
		m_renderCameraProjection->setCameraAspect( camera3D.cameraAspect );

		//Viewport rp;
		//rp.begin.x = 0.f;
		//rp.begin.y = 0.f;

		//rp.end.x = camera3D.width;
		//rp.end.y = camera3D.height;

		//m_renderCameraProjection->setRenderport( rp );

		this->addChild( m_renderCameraProjection );

		m_renderViewport = NODE_SERVICE(m_serviceProvider)
			->createNodeT<RenderViewport *>( CONST_STRING( m_serviceProvider, RenderViewport ) );

		m_renderViewport->setName( name );

		Viewport vp;
		vp.begin.x = 0.f;
		vp.begin.y = 0.f;

		vp.end.x = camera3D.width;
		vp.end.y = camera3D.height;

		m_renderViewport->setViewport( vp );

		this->addChild( m_renderViewport );
	}
	//////////////////////////////////////////////////////////////////////////
	void Movie::destroyCamera3D_()
	{
		if( m_renderCameraProjection != nullptr )
		{
			m_renderCameraProjection->destroy();
			m_renderCameraProjection = nullptr;
		}

		if( m_renderViewport != nullptr )
		{
			m_renderViewport->destroy();
			m_renderViewport = nullptr;
		}

		for( TVectorClipplane::iterator
			it = m_clipplanes.begin(),
			it_end = m_clipplanes.end();
		it != it_end;
		++it )
		{
			RenderClipplane * clipplane = *it;

			clipplane->destroy();
		}

		m_clipplanes.clear();
	}
	//////////////////////////////////////////////////////////////////////////
	void Movie::setEnableLayer_( const MovieLayer & _layer, bool _enable )
	{
		Nodies & nd = m_nodies[_layer.index - 1];

		nd.enable = _enable;

		if( nd.visible == true && nd.enable == true )
		{
			nd.node->localHide( false );
		}
		else
		{
			nd.node->localHide( true );
		}
	}
	//////////////////////////////////////////////////////////////////////////
	void Movie::setVisibleLayer_( const MovieLayer & _layer, bool _visible )
	{ 
		Nodies & nd = m_nodies[_layer.index - 1];

		nd.visible = _visible;

		if( nd.visible == true && nd.enable == true )
		{
			nd.node->localHide( false );
		}
		else
		{
			nd.node->localHide( true );
		}
	}
	//////////////////////////////////////////////////////////////////////////
	bool Movie::setupBlendingMode_( const MovieLayer & _layer, Materialable * _materiable )
	{ 
		if( _layer.blendingMode.empty() == true )
		{
			//Empty
		}
		else if( _layer.blendingMode == CONST_STRING( m_serviceProvider, BlendingModeScreen ) )
		{
			_materiable->setBlendMode( EMB_SCREEN );
		}
		else if( _layer.blendingMode == CONST_STRING( m_serviceProvider, BlendingModeMultiply ) )
		{
			_materiable->setBlendMode( EMB_MULTIPLY );
		}
		else if( _layer.blendingMode == CONST_STRING( m_serviceProvider, BlendingModeNormal ) )
		{
			_materiable->setBlendMode( EMB_NORMAL );
		}
		else if( _layer.blendingMode == CONST_STRING( m_serviceProvider, BlendingModeAdd ) )
		{
			_materiable->setBlendMode( EMB_ADD );
		}
		else
		{
			LOGGER_ERROR( m_serviceProvider )("Movie.setupBlendingMode_: %s resource %s layer '%s' blending mode not supported '%s'"
				, this->getName().c_str()
				, this->getResourceMovieName().c_str()
				, _layer.name.c_str()
				, _layer.blendingMode.c_str()
				);

			return false;
		}

		return true;
	}
	//////////////////////////////////////////////////////////////////////////
	void Movie::_setLoop( bool _value )
	{
		if( this->isCompile() == false )
		{
			return;
		}

		this->updateLoop_( _value );
	}
	//////////////////////////////////////////////////////////////////////////
	void Movie::updateLoop_( bool _value )
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

			if( layer.isAnimatable() == false )
			{
				continue;
			}

			if( layer.loop == false )
			{
				continue;
			}

			Animatable * animatable = this->getLayerAnimatable_( layer );

			animatable->setLoop( _value );
		}
	}
	//////////////////////////////////////////////////////////////////////////
	void Movie::_setAnimationSpeedFactor( float _factor )
	{
		if( this->isCompile() == false )
		{
			return;
		}

		this->updateAnimationSpeedFactor_( _factor );
	}
	//////////////////////////////////////////////////////////////////////////
	void Movie::updateAnimationSpeedFactor_( float _factor )
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

			if( layer.isAnimatable() == false )
			{
				continue;
			}

			Animatable * animatable = this->getLayerAnimatable_( layer );

			animatable->setAnimationSpeedFactor( _factor );
		}
	}
	//////////////////////////////////////////////////////////////////////////
	bool Movie::_interrupt( uint32_t _enumerator )
	{
		(void)_enumerator;

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

			if( layer.isAnimatable() == false )
			{
				continue;
			}

			Animatable * animatable = this->getLayerAnimatable_( layer );

			if( animatable->interrupt() == false )
			{
				return false;
			}
		}

		return true;
	}
}