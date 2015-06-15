#	include "Video.h"

#	include "ResourceVideo.h"

#	include "Interface/ResourceInterface.h"
#	include "Interface/NodeInterface.h"
#   include "Interface/PrototypeManagerInterface.h"
#	include "Interface/RenderSystemInterface.h"
#	include "Interface/CodecInterface.h"
#	include "Interface/VideoCodecInterface.h"
#	include "Interface/FileSystemInterface.h"

#	include "Logger/Logger.h"

#	include "pybind/system.hpp"
#	include "Consts.h"

#   include <algorithm>

namespace Menge
{
	//////////////////////////////////////////////////////////////////////////
	Video::Video()
		: m_autoStart(false)
		, m_material(nullptr)
		, m_frameSize(0.f, 0.f)
		, m_uv(0.f, 0.f, 1.f, 1.f)
		, m_videoDecoder(nullptr)
		, m_timing(0.f)
		, m_blendAdd(false)
        , m_needUpdate(false)
		, m_invalidVideoTexture(true)
		, m_invalidFirstFrame(true)
		, m_invalidateMaterial(true)
	{
	}
	//////////////////////////////////////////////////////////////////////////
	void Video::_setEventListener( const pybind::dict & _listener )
	{
		Node::_setEventListener( _listener );

		this->registerEvent( EVENT_VIDEO_END, ("onVideoEnd"), _listener );
	}
	//////////////////////////////////////////////////////////////////////////
	void Video::setResourceVideo( ResourceVideo * _resourceVideo )
	{
		if( m_resourceVideo == _resourceVideo )
		{
			return;
		}

		m_resourceVideo = _resourceVideo;

		this->recompile();
	}
	//////////////////////////////////////////////////////////////////////////
	ResourceVideo * Video::getResourceVideo() const
	{
		return m_resourceVideo;
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

		float speedFactor = this->getAnimationSpeedFactor();
		float timing = _timing * speedFactor;

		float scretch = this->getScretch();

		float totalTiming = timing / scretch;

		if( m_invalidFirstFrame == true )
		{
			m_invalidFirstFrame = false;
			m_needUpdate = true;
			
			if( this->sync_( totalTiming ) == false )
			{
				if( this->syncFirstFrame_() == false )
				{
					return;
				}
			}
		}
		else
		{
			m_needUpdate = this->sync_( totalTiming );
		}		
	}
	//////////////////////////////////////////////////////////////////////////
	bool Video::_activate()
	{
		if( Node::_activate() == false )
		{
			return false;
		}

		m_needUpdate = false;

		m_invalidVideoTexture = true;
		m_invalidFirstFrame = true;

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
		if( m_resourceVideo == nullptr )
		{
			LOGGER_ERROR(m_serviceProvider)("Video::_compile '%s' resource is null"
				, this->getName().c_str()
				);	

			return false;
		}

        if( m_resourceVideo.compile() == false )
        {
            LOGGER_ERROR(m_serviceProvider)("Video::_compile '%s' resource '%s' is not compile"
                , this->getName().c_str()
                , m_resourceVideo->getName().c_str()
                );	

            return false;
        }

		if ( this->compileDecoder_() == false )
		{
			LOGGER_ERROR(m_serviceProvider)("Video::_compile %s can`t create video decoder '%s'"
				, this->getName().c_str()
				, m_resourceVideo->getName().c_str() 
				);	

			return false;
		}

        uint32_t channels;

		if ( m_resourceVideo->isAlpha() == true )
		{
			channels = 4;
		}
		else
		{
			channels = 3;
		}
		        
		const VideoCodecDataInfo * dataInfo = m_videoDecoder->getCodecDataInfo();

		RenderTextureInterfacePtr dynamicTexture = RENDERTEXTURE_SERVICE(m_serviceProvider)
            ->createDynamicTexture( dataInfo->frameWidth, dataInfo->frameHeight, channels, 1, dataInfo->format );

		if( dynamicTexture == nullptr )
		{
			LOGGER_ERROR(m_serviceProvider)("Video::_compile '%s' resource '%s' can`t create dynamic texture"
				, this->getName().c_str()
				, m_resourceVideo->getName().c_str() 
				);	

			return false;
		}

        const ConstString & resourceVideoName = m_resourceVideo->getName();		
        dynamicTexture->setFileName( resourceVideoName );

		m_textures[0] = dynamicTexture;

		this->updateUV_();
		this->invalidateMaterial_();
		this->invalidateVertices();

        m_timing = 0.f;
        m_needUpdate = false;

		m_invalidVideoTexture = true;
		m_invalidFirstFrame = true;

		return true;
	}
	//////////////////////////////////////////////////////////////////////////
	bool Video::compileDecoder_()
	{
        m_videoDecoder = m_resourceVideo->createVideoDecoder();

        if( m_videoDecoder == nullptr )
        {
            LOGGER_ERROR(m_serviceProvider)("Video::_compileDecoder '%s' can't create video decoder"
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
		if( m_videoDecoder != nullptr )
		{
			m_resourceVideo->destroyVideoDecoder( m_videoDecoder );
			m_videoDecoder = nullptr;
		}

        m_resourceVideo.release();

		m_textures[0] = nullptr;

		m_material = nullptr;
    }
	//////////////////////////////////////////////////////////////////////////
	void Video::_stop( uint32_t _enumerator )
	{
		m_needUpdate = false;

		EVENTABLE_CALL(m_serviceProvider, this, EVENT_VIDEO_END)( this, _enumerator, false );
	}
	//////////////////////////////////////////////////////////////////////////
	void Video::_end( uint32_t _enumerator )
	{
		EVENTABLE_CALL(m_serviceProvider, this, EVENT_VIDEO_END)( this, _enumerator, false);	
	}
	//////////////////////////////////////////////////////////////////////////
	void Video::pause()
	{
		m_play = false;
	}
	//////////////////////////////////////////////////////////////////////////
	bool Video::_play( float _time )
	{
        (void)_time;

		if( this->isActivate() == false )
		{
			LOGGER_ERROR(m_serviceProvider)("Video::_play: '%s' play not activate"
				, this->getName().c_str()
				);

			return false;
		}

        m_playIterator = this->getPlayCount();

        m_timing = 0.f;

		return true;
	}
	//////////////////////////////////////////////////////////////////////////
	bool Video::_restart( float _time, uint32_t _enumerator )
	{
        (void)_time;
        (void)_enumerator;

		if( this->isActivate() == false )
		{
			return false;
		}

        m_playIterator = this->getPlayCount();

		return true;
	}
	//////////////////////////////////////////////////////////////////////////
	void Video::_pause( uint32_t _enumerator )
	{
		(void)_enumerator;
	}
	//////////////////////////////////////////////////////////////////////////
	void Video::_resume( uint32_t _enumerator )
	{
		(void)_enumerator;
	}
	//////////////////////////////////////////////////////////////////////////
	void Video::_render( const RenderViewportInterface * _viewport, const RenderCameraInterface * _camera )
	{
		Node::_render( _viewport, _camera );

		if( m_invalidFirstFrame == true )
		{
			if( this->syncFirstFrame_() == false )
			{
				return;
			}

			m_invalidFirstFrame = false;
			m_needUpdate = true;
		}

		if( m_needUpdate == true )
		{
			if( this->fillVideoBuffer_() == false )
			{
				LOGGER_ERROR(m_serviceProvider)("Video::_render %s invalid fill video buffer (%s)"
					, this->getName().c_str()
					, m_resourceVideo->getName().c_str()
					);

				return;
			}

            m_needUpdate = false;
		}

		if( m_invalidVideoTexture == true )
		{
			return;
		}
        
		const RenderVertex2D * vertices = this->getVertices();

		const RenderMaterialInterfacePtr & material = this->getMaterial(); 

		RENDER_SERVICE(m_serviceProvider)
            ->addRenderQuad( _viewport, _camera, material, vertices, 4, nullptr );
	}
	//////////////////////////////////////////////////////////////////////////
	void Video::_updateVertices( RenderVertex2D * _vertices, unsigned char _invalidateVertices ) const
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

		uint32_t argb = color.getAsARGB();

		for( RenderVertex2D
			*it = _vertices,
			*it_end = _vertices + 4;
		it != it_end;
		++it )
		{
			RenderVertex2D & vtx = *it;

			vtx.color = argb;
		}
	}
	//////////////////////////////////////////////////////////////////////////
	void Video::_invalidateWorldMatrix()
	{
		Node::_invalidateWorldMatrix();

		this->invalidateVertices();
	}
	//////////////////////////////////////////////////////////////////////////
	void Video::_updateBoundingBox( mt::box2f & _boundingBox ) const
	{
		const RenderVertex2D * vertcies = this->getVertices();

		mt::reset( _boundingBox, vertcies[0].pos.x, vertcies[0].pos.y );

		mt::add_internal_point( _boundingBox, vertcies[1].pos.x, vertcies[1].pos.y );
		mt::add_internal_point( _boundingBox, vertcies[2].pos.x, vertcies[2].pos.y );
		mt::add_internal_point( _boundingBox, vertcies[3].pos.x, vertcies[3].pos.y );
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

        float frameTiming = dataInfo->getFrameTiming();
		float duration = dataInfo->duration;

		while( m_timing >= frameTiming )
		{            
            float pts;
			EVideoDecoderReadState state = m_videoDecoder->readNextFrame( pts );
                       
            needUpdate = true;

			if( state == VDRS_SUCCESS )
			{
				//Empty
			}
			else if( state == VDRS_END_STREAM )
			{	
				if( this->getLoop() == false && --m_playIterator == 0 )
                {
                    this->stop();

                    break;
                }
                else
				{
                    float time = m_intervalBegin;

					float intervalTime = duration * (m_playCount - m_playIterator);

					if( m_intervalStart > intervalTime )
					{
						time += m_intervalStart - intervalTime;
					}

                    if( time > frameTiming )
                    {
                        time -= frameTiming;
                    }

                    if( m_videoDecoder->seek( time ) == false )
                    {
                        LOGGER_ERROR(m_serviceProvider)("Video::sync_ %s:%s invalid seek to %f"
                            , this->getName().c_str()
                            , m_resourceVideo->getName().c_str()
                            , time
                            );
                    }

                    m_timing += frameTiming;
				}
			}
			else if( state == VDRS_FAILURE )
			{
				LOGGER_ERROR(m_serviceProvider)("Video::_sync: '%s' error reading frame timing %4.2f total timing %4.2f"
					, this->getName().c_str()
					, _timing
					, m_timing
					);
                
                needUpdate = false;

				break;	
			}
			else if( state == VDRS_SKIP )
			{
                needUpdate = false;

				continue;	
			}

            m_timing -= frameTiming;
		}
        	
		return needUpdate;
	}
	//////////////////////////////////////////////////////////////////////////
	bool Video::syncFirstFrame_()
	{
		const VideoCodecDataInfo * dataInfo = m_videoDecoder->getCodecDataInfo();
				
		float frameTiming = dataInfo->getFrameTiming();
		float duration = dataInfo->duration;

		for(;;)
		{
			float pts;
			EVideoDecoderReadState state = m_videoDecoder->readNextFrame( pts );

			if( state == VDRS_SUCCESS )
			{
				//Empty
			}
			else if( state == VDRS_FAILURE )
			{
				LOGGER_ERROR(m_serviceProvider)("Video::syncFirstFrame_: '%s' error reading first frame"
					, this->getName().c_str()
					);

				return false;
			}
			else if( state == VDRS_END_STREAM )
			{
				if( this->getLoop() == false && --m_playIterator == 0 )
				{
					this->stop();

					break;
				}
				else
				{
					float time = m_intervalBegin;

					float intervalTime = duration * (m_playCount - m_playIterator);

					if( m_intervalStart > intervalTime )
					{
						time += m_intervalStart - intervalTime;
					}

					if( time > frameTiming )
					{
						time -= frameTiming;
					}

					if( m_videoDecoder->seek( time ) == false )
					{
						LOGGER_ERROR(m_serviceProvider)("Video::sync_ %s:%s invalid seek to %f"
							, this->getName().c_str()
							, m_resourceVideo->getName().c_str()
							, time
							);

						return false;
					}

					m_timing += frameTiming;
				}
			}
			else if( state == VDRS_SKIP )
			{
				continue;	
			}

			break;
		}

		return true;
	}
	////////////////////////////////////////////////////////////////////
	void Video::_setTiming( float _timing )
	{
        if( this->isCompile() == false )
        {
            LOGGER_ERROR(m_serviceProvider)("Video::_setTiming %s not compile"
                , this->getName().c_str()
                );

            return;
        }

        m_playIterator = this->getPlayCount();
				
		const VideoCodecDataInfo * dataInfo = m_videoDecoder->getCodecDataInfo();

        float frameTiming = dataInfo->getFrameTiming();

        uint32_t frameRate = m_resourceVideo->getFrameRate();

        if( frameRate > 0 )
        {
            frameTiming = 1000.f / float(frameRate);
        }

		float seek_timing = _timing;

		if( seek_timing >= frameTiming )
		{		
			seek_timing -= frameTiming;
		}

		float duration = dataInfo->duration;

		uint32_t skipIterator = (uint32_t)((m_intervalStart / duration) + 0.5f);

		if( skipIterator > 0 )
		{
			m_playIterator -= skipIterator;
		}

		m_timing = 0.f;

		m_invalidVideoTexture = true;
		m_invalidFirstFrame = true;
		
		if( m_videoDecoder->seek( seek_timing ) == false )
        {
            LOGGER_ERROR(m_serviceProvider)("Video::_setTiming %s:%s invalid seek to %f"
                , this->getName().c_str()
                , m_resourceVideo->getName().c_str()
                , seek_timing
                );

			return;
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
		const RenderTextureInterfacePtr & texture = m_textures[0];
				
		Rect rect;

		const VideoCodecDataInfo * dataInfo = m_videoDecoder->getCodecDataInfo();

		if( dataInfo->clamp == true )
		{
			rect.left = 0;
			rect.top = 0;
			rect.right = texture->getWidth();
			rect.bottom = texture->getHeight();
		}
		else
		{
			const RenderImageInterfacePtr & image = texture->getImage();

			rect.left = 0;
			rect.top = 0;
			rect.right = image->getHWWidth();
			rect.bottom = image->getHWHeight();
		}

        size_t pitch = 0;
		void * lockRect = texture->lock( &pitch, 0, rect, false );

		if( lockRect == nullptr )
		{
			LOGGER_ERROR(m_serviceProvider)("Video::fillVideoBuffer_ %s:%s invalid lock texture %f:%f"
				, this->getName().c_str()
				, m_resourceVideo->getName().c_str()
				, m_frameSize.x
				, m_frameSize.y
				);

			return false;
		}

		m_videoDecoder->setPitch( pitch );

		size_t bufferSize = texture->getMemoryUse();
        size_t bytes = m_videoDecoder->decode( lockRect, bufferSize );
       
		texture->unlock( 0 );

		m_invalidVideoTexture = false;
                
		return bytes != 0;
	}
	////////////////////////////////////////////////////////////////////
	bool Video::_interrupt( uint32_t _enumerator )
	{
        (void)_enumerator;

		return false;
	}
	////////////////////////////////////////////////////////////////////
	void Video::updateUV_()
	{
		const RenderTextureInterfacePtr & texture = m_textures[0];

		const Rect & rect = texture->getRect();

		const RenderImageInterfacePtr & image = texture->getImage();

		uint32_t hwWidth = image->getHWWidth();
		uint32_t hwHeight = image->getHWHeight();

		float scaleLeft = float(rect.left) / float(hwWidth);
		float scaleTop = float(rect.top) / float(hwHeight);
		float scaleRight = float(rect.right) / float(hwWidth);
		float scaleBottom = float(rect.bottom) / float(hwHeight);

		m_uv.x = scaleLeft;
		m_uv.y = scaleTop;
		m_uv.z = scaleLeft + (scaleRight - scaleLeft);
		m_uv.w = scaleTop + (scaleBottom - scaleTop);
	}
	//////////////////////////////////////////////////////////////////////////
	void Video::invalidateMaterial_()
	{
		m_invalidateMaterial = true;
	}
	////////////////////////////////////////////////////////////////////
	void Video::updateMaterial_()
	{
		m_invalidateMaterial = false;

		ConstString stageName; 
		if ( this->isBlendAdd() == true )
		{
			stageName = CONST_STRING( m_serviceProvider, Texture_Intensive );
		}
		else
		{
			stageName = CONST_STRING( m_serviceProvider, Texture_Blend );
		}

		m_material = RENDERMATERIAL_SERVICE(m_serviceProvider)
			->getMaterial( stageName, false, false, PT_TRIANGLELIST, 1, m_textures );
	}
	////////////////////////////////////////////////////////////////////
	void Video::setBlendAdd( bool _blendAdd )
	{
		if ( m_blendAdd == _blendAdd )
		{
			return;
		}

		m_blendAdd = _blendAdd;

		this->invalidateMaterial_();
	}
	////////////////////////////////////////////////////////////////////
	bool Video::isBlendAdd() const
	{
		return m_blendAdd;
	}
	////////////////////////////////////////////////////////////////////
}
