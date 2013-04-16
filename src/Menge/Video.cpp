#	include "Video.h"

#	include "ResourceVideo.h"

#	include "Interface/ResourceInterface.h"
#	include "Interface/SoundSystemInterface.h"
#	include "Interface/NodeInterface.h"
#	include "Interface/RenderSystemInterface.h"
#	include "Interface/CodecInterface.h"
#	include "Interface/VideoCodecInterface.h"
#	include "Interface/FileSystemInterface.h"

#   include "SoundEmitter.h"

#	include "Logger/Logger.h"

#	include "pybind/system.hpp"
#	include "Consts.h"

namespace Menge
{
	//////////////////////////////////////////////////////////////////////////
	Video::Video()
		: m_autoStart(false)
		, m_needUpdate(false)
        , m_needUpdate2(false)
		, m_material(NULL)
		, m_frameSize(0.f, 0.f)
		, m_uv(0.f, 0.f, 1.f, 1.f)
		, m_videoDecoder(NULL)
		, m_timing(0.f)
        , m_playIterator(0)
		, m_blendAdd(false)
        , m_soundEmitter(NULL)
	{
		m_textures[0] = NULL;
	}
	//////////////////////////////////////////////////////////////////////////
	void Video::_setEventListener( PyObject * _listener )
	{
		Node::_setEventListener( _listener );

		this->registerEvent( EVENT_VIDEO_END, ("onVideoEnd"), _listener );
	}
	//////////////////////////////////////////////////////////////////////////
	void Video::setVideoResource( const ConstString& _resource )
	{
		if( m_resourceVideoName == _resource )
		{
			return;
		}

		m_resourceVideoName = _resource;

		recompile();
	}
	//////////////////////////////////////////////////////////////////////////
	const ConstString & Video::getVideoResource() const
	{
		return m_resourceVideoName;
	}
	//////////////////////////////////////////////////////////////////////////
	void Video::setSoundResource( const ConstString& _resource )
	{
		if( m_resourceSoundName == _resource )
		{
			return;
		}

		m_resourceSoundName = _resource;

		recompile();
	}
	//////////////////////////////////////////////////////////////////////////
	const ConstString & Video::getSoundResource() const
	{
		return m_resourceSoundName;
	}
	//////////////////////////////////////////////////////////////////////////
	void Video::_update( float _current, float _timing )
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

		float speedFactor = this->getSpeedFactor();
		float timing = _timing * speedFactor;

		//Node::_update( _current, timing );
		//localHide(false);
		//printf("%f %s\n",_timing,m_name.c_str());

		//printf("%f %s\n",_timing,m_name.c_str());
		m_needUpdate = this->sync_( timing );

        //printf("Video._update %f %s %d %d\n"
        //    , timing
        //    , m_name.c_str()
        //    , m_needUpdate
        //    , m_needUpdate2
        //    );
	}
	//////////////////////////////////////////////////////////////////////////
	bool Video::_activate()
	{
		if( Node::_activate() == false )
		{
			return false;
		}
		//printf("%s  onActivate \n",m_name.c_str());

		return true;
	}
	//////////////////////////////////////////////////////////////////////////
	void Video::_deactivate()
	{
		Node::_deactivate();
	}
	//////////////////////////////////////////////////////////////////////////
	bool Video::_compile()
	{
		m_resourceVideo = RESOURCE_SERVICE(m_serviceProvider)
            ->getResourceT<ResourceVideo>( m_resourceVideoName );

		if( m_resourceVideo == 0 )
		{
			LOGGER_ERROR(m_serviceProvider)( "Video::_compile '%s' resource not found '%s'"
				, this->getName().c_str()
				, m_resourceVideoName.c_str() 
				);	

			return false;
		}

		this->updateMaterial_();

		if ( this->compileDecoder_() == false )
		{
			LOGGER_ERROR(m_serviceProvider)( "Video::_compile %s can`t create video decoder '%s'"
				, this->getName().c_str()
				, m_resourceVideoName.c_str() 
				);	

			return false;
		}

        size_t channels;

		if ( m_resourceVideo->isAlpha() == true )
		{
			channels = 4;
		}
		else
		{
			channels = 3;
		}

        size_t width = (size_t)(m_frameSize.x + 0.5f);
        size_t height = (size_t)(m_frameSize.y + 0.5f);

		m_textures[0] = RENDER_SERVICE(m_serviceProvider)
            ->createDynamicTexture( width, height, channels, PF_UNKNOWN );

		//m_material->textureStage[0].texture = m_resourceImage;

		if( m_resourceSoundName.empty() == false )
		{
			m_soundEmitter = NODE_SERVICE(m_serviceProvider)
                ->createNodeT<SoundEmitter>( CONST_STRING(m_serviceProvider, SoundEmitter) );
			
			m_soundEmitter->setSoundResource( m_resourceSoundName );

			this->addChildren( m_soundEmitter );

			if( m_soundEmitter->compile() == false )
			{
				LOGGER_ERROR(m_serviceProvider)( "Video::_compile '%s' failed to compile sound resource '%s'"
					, this->getName().c_str()
					, m_resourceSoundName.c_str() 
					);

				return false;
			}
		}

		this->updateUV_();
		this->invalidateVertices();
		this->invalidateBoundingBox();

        m_timing = 0.f;
        m_needUpdate = false;
        m_needUpdate2 = false;

		return true;
	}
	//////////////////////////////////////////////////////////////////////////
	bool Video::compileDecoder_()
	{
        m_videoDecoder = m_resourceVideo->createVideoDecoder();

        if( m_videoDecoder == 0 )
        {
            LOGGER_ERROR(m_serviceProvider)( "Video::_compileDecoder '%s' can't create video decoder"
                , this->getName().c_str()
                );

            return false;
        }
        		
		const VideoCodecDataInfo * dataInfo = m_videoDecoder->getCodecDataInfo();

		m_frameSize.x = (float)dataInfo->frameWidth;
		m_frameSize.y = (float)dataInfo->frameHeight;

		return true;
	}
	//////////////////////////////////////////////////////////////////////////
	void Video::_release()
	{
		RENDER_SERVICE(m_serviceProvider)
			->releaseTexture( m_textures[0] );

		if( m_videoDecoder != NULL )
		{
            InputStreamInterface * stream = m_videoDecoder->getStream();
            stream->destroy();

			m_videoDecoder->destroy();
			m_videoDecoder = NULL;
		}

		if( m_resourceVideo != 0 )
		{
			m_resourceVideo->decrementReference();
			m_resourceVideo = 0;
		}

        if( m_soundEmitter != NULL )
        {
            m_soundEmitter->destroy();
            m_soundEmitter = NULL;
        }
	}
	//////////////////////////////////////////////////////////////////////////
	void Video::_stop( size_t _enumerator )
	{
		if( m_soundEmitter && m_soundEmitter->isCompile() )
		{
			m_soundEmitter->stop();
		}

		m_needUpdate = false;
        m_needUpdate2 = false;

		EVENTABLE_CALL(this, EVENT_VIDEO_END)( "(OiO)", this->getEmbed() ,_enumerator, pybind::get_bool(false) );
	}
	//////////////////////////////////////////////////////////////////////////
	void Video::_end( size_t _enumerator )
	{
		if( m_soundEmitter && m_soundEmitter->isCompile() )
		{
			m_soundEmitter->stop();
		}

		EVENTABLE_CALL(this, EVENT_VIDEO_END)( "(OiO)", this->getEmbed() ,_enumerator, pybind::get_bool(false));	
	}
	//////////////////////////////////////////////////////////////////////////
	void Video::pause()
	{
		m_play = false;
	}
	//////////////////////////////////////////////////////////////////////////
	bool Video::_play( float _time )
	{
		if( this->isActivate() == false )
		{
			LOGGER_ERROR(m_serviceProvider)( "Video::_play: '%s' play not activate"
				, this->getName().c_str()
				);

			return false;
		}

		if( m_soundEmitter && m_soundEmitter->isCompile() )
		{
			m_soundEmitter->play( _time );
		}

        m_playIterator = this->getPlayCount();

        m_timing = 0.f;

		return true;
	}
	//////////////////////////////////////////////////////////////////////////
	bool Video::_restart( float _time, size_t _enumerator )
	{
        (void)_time;
        (void)_enumerator;

		if( isActivate() == false )
		{
			return false;
		}

		return true;
	}
	//////////////////////////////////////////////////////////////////////////
	void Video::_render( RenderCameraInterface * _camera )
	{
		Node::_render( _camera );

		if( m_needUpdate == true || m_needUpdate2 == true )
		{
            m_needUpdate = false;
            m_needUpdate2 = false;

            if( this->fillVideoBuffer_() == false )
            {
                LOGGER_ERROR(m_serviceProvider)("Video::_render %s invalid fill video buffer (%s)"
                    , this->getName().c_str()
                    , m_resourceVideoName.c_str()
                    );
            }                
		}
        
		const Vertex2D * vertices = this->getVertices();

		RENDER_SERVICE(m_serviceProvider)
            ->addRenderObject2D( _camera, m_material, m_textures, 1, vertices, 4, LPT_QUAD );
	}
	//////////////////////////////////////////////////////////////////////////
	void Video::_updateVertices( Vertex2D * _vertices, unsigned char _invalidateVertices )
	{
        (void)_invalidateVertices;

		const mt::mat4f & wm = this->getWorldMatrix();

		//mt::mul_v2_m3( m_vertices[0], m_offset, wm );
		//mt::mul_v2_m3( m_vertices[1], m_offset + mt::vec2f( m_size.x, 0.0f ), wm );
		//mt::mul_v2_m3( m_vertices[2], m_offset + m_size, wm );
		//mt::mul_v2_m3( m_vertices[3], m_offset + mt::vec2f( 0.0f, m_size.y ), wm );

		mt::vec3f transformX;
		mt::vec3f transformY;

		mt::mul_v3_m4_r( transformX, mt::vec3f( m_frameSize.x, 0.f, 0.f ), wm );
		mt::mul_v3_m4_r( transformY, mt::vec3f( 0.f, m_frameSize.y, 0.f ), wm );

		_vertices[0].pos = wm.v3.to_vec3f();
		_vertices[1].pos = _vertices[0].pos + transformX;
		_vertices[2].pos = _vertices[1].pos + transformY;
		_vertices[3].pos = _vertices[0].pos + transformY;

		_vertices[0].uv.x = m_uv.x;
		_vertices[0].uv.y = m_uv.y;
		_vertices[1].uv.x = m_uv.z;
		_vertices[1].uv.y = m_uv.y;
		_vertices[2].uv.x = m_uv.z;
		_vertices[2].uv.y = m_uv.w;
		_vertices[3].uv.x = m_uv.x;
		_vertices[3].uv.y = m_uv.w;

        _vertices[0].uv2.x = 0.f;
        _vertices[0].uv2.y = 0.f;
        _vertices[1].uv2.x = 0.f;
        _vertices[1].uv2.y = 0.f;
        _vertices[2].uv2.x = 0.f;
        _vertices[2].uv2.y = 0.f;
        _vertices[3].uv2.x = 0.f;
        _vertices[3].uv2.y = 0.f;

		ColourValue color;
		this->calcTotalColor(color);

		uint32 argb = color.getAsARGB();
		ApplyColor2D applyColor( argb );
		std::for_each( _vertices, _vertices + 4, applyColor );
	}
	//////////////////////////////////////////////////////////////////////////
	void Video::_invalidateWorldMatrix()
	{
		Node::_invalidateWorldMatrix();

		this->invalidateVertices();
	}
	//////////////////////////////////////////////////////////////////////////
	void Video::_updateBoundingBox( mt::box2f & _boundingBox )
	{
		const Vertex2D * vertcies = this->getVertices();

		mt::vec2f v( vertcies[0].pos[0], vertcies[0].pos[1] );
		mt::reset( _boundingBox, v );

		for( int i = 1; i != 4; ++i )
		{
			v.x = vertcies[i].pos[0];
			v.y = vertcies[i].pos[1];
			mt::add_internal_point( _boundingBox, v );
		}
	}
	//////////////////////////////////////////////////////////////////////////
	void Video::_invalidateColor()
	{
		Node::_invalidateColor();

		this->invalidateVertices();
	}
	////////////////////////////////////////////////////////////////////
	bool Video::sync_( float _timing )
	{
		m_timing += _timing;

		const VideoCodecDataInfo * dataInfo = m_videoDecoder->getCodecDataInfo();
	
        bool needUpdate = false;
		
		while( m_timing >= dataInfo->frameTiming )
		{            
			EVideoDecoderReadState state = m_videoDecoder->readNextFrame( 0.f );
            
            needUpdate = true;

			if( state == VDRS_END_STREAM )
			{	
				if( this->getLoop() == false && --m_playIterator == 0 )
                {
                    this->stop();

                    break;
                }
                else
				{
                    float time = m_intervalStart + m_intervalBegin;

                    if( m_videoDecoder->seek( time ) == false )
                    {
                        LOGGER_ERROR(m_serviceProvider)("Video::sync_ %s:%s invalid seek to %f"
                            , this->getName().c_str()
                            , m_resourceVideoName.c_str()
                            , time
                            );
                    }
				}
			}
			else if( state == VDRS_FAILURE )
			{
				LOGGER_ERROR(m_serviceProvider)( "Video::_sync: '%s' error reading frame timing %4.2f total timing %4.2f"
					, this->getName().c_str()
					, _timing
					, m_timing
					);

				break;	
			}
			else if( state == VDRS_SKIP )
			{
				continue;	
			}

            m_timing -= dataInfo->frameTiming;
		}
		
		return needUpdate;
	}
	////////////////////////////////////////////////////////////////////
	void Video::_setTiming( float _timing )
	{
		//float current_timing = m_videoDecoder->getTiming();
		const VideoCodecDataInfo * dataInfo = m_videoDecoder->getCodecDataInfo();

		float seek_timing = _timing;

		if( seek_timing >= dataInfo->frameTiming )
		{		
			seek_timing -= dataInfo->frameTiming;
		}
		
		if( m_videoDecoder->seek( seek_timing ) == false )
        {
            LOGGER_ERROR(m_serviceProvider)("Video::_setTiming %s:%s invalid seek to %f"
                , this->getName().c_str()
                , m_resourceVideoName.c_str()
                , seek_timing
                );
        }

        //m_timing = fmod( seek_timing, dataInfo->frameTiming );

        m_videoDecoder->readNextFrame( 0.f );

        m_timing = 0.f;

        m_needUpdate2 = true;

        if( m_videoDecoder->seek( seek_timing ) == false )
        {
            LOGGER_ERROR(m_serviceProvider)("Video::_setTiming %s:%s invalid seek to %f (double)"
                , this->getName().c_str()
                , m_resourceVideoName.c_str()
                , seek_timing
                );
        }	
    }
	////////////////////////////////////////////////////////////////////
	float Video::_getTiming() const
	{
		float timing = m_videoDecoder->getTiming();

		return timing;
	}
	////////////////////////////////////////////////////////////////////
	void Video::_setFirstFrame()
	{
		this->setTiming( 0.f );
	}
	////////////////////////////////////////////////////////////////////
	void Video::_setLastFrame()
	{
		const VideoCodecDataInfo * dataInfo = m_videoDecoder->getCodecDataInfo(); 
		this->_setTiming( dataInfo->duration );
	}
	////////////////////////////////////////////////////////////////////
	bool Video::fillVideoBuffer_()
	{
		Rect rect;
		rect.left = 0;
		rect.top = 0;
		rect.right = (size_t)m_frameSize.x;
		rect.bottom = (size_t)m_frameSize.y;

        int pitch = 0;
		unsigned char* lockRect = m_textures[0]->lock( &pitch, rect, false );

		unsigned int decode = m_videoDecoder->decode( lockRect, pitch );
        
		m_textures[0]->unlock();

        if( decode == 0 )
        {
            return false;
        }

		return true;
	}
	////////////////////////////////////////////////////////////////////
	bool Video::_interrupt( size_t _enumerator )
	{
        (void)_enumerator;

		return false;
	}
	////////////////////////////////////////////////////////////////////
	void Video::updateUV_()
	{
		RenderTextureInterface * texture = m_textures[0];

		const Rect & rect = texture->getRect();

		size_t hwWidth = texture->getHWWidth();
		size_t hwHeight = texture->getHWHeight();

		float scaleLeft = float(rect.left) / float(hwWidth);
		float scaleTop = float(rect.top) / float(hwHeight);
		float scaleRight = float(rect.right) / float(hwWidth);
		float scaleBottom = float(rect.bottom) / float(hwHeight);

		m_uv.x = scaleLeft;
		m_uv.y = scaleTop;
		m_uv.z = scaleLeft + (scaleRight - scaleLeft);
		m_uv.w = scaleTop + (scaleBottom - scaleTop);
	}
	////////////////////////////////////////////////////////////////////
	void Video::updateMaterial_()
	{
		if ( this->isBlendAdd() == true )
		{
			m_materialGroup = RENDER_SERVICE(m_serviceProvider)->getMaterialGroup( CONST_STRING(m_serviceProvider, ParticleIntensive) );
		}
		else
		{
			m_materialGroup = RENDER_SERVICE(m_serviceProvider)->getMaterialGroup( CONST_STRING(m_serviceProvider, BlendSprite) );
		}

		m_material = m_materialGroup->getMaterial( TAM_CLAMP, TAM_CLAMP );
	}
	////////////////////////////////////////////////////////////////////
	void Video::setBlendAdd( bool _blendAdd )
	{
		if ( m_blendAdd == _blendAdd )
		{
			return;
		}

		m_blendAdd = _blendAdd;

		this->updateMaterial_();
	}
	////////////////////////////////////////////////////////////////////
	bool Video::isBlendAdd() const
	{
		return m_blendAdd;
	}
	////////////////////////////////////////////////////////////////////
}
