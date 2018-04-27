#include "SurfaceVideo.h"

#include "Interface/RenderSystemInterface.h"

#include "Core/RenderUtils.h"

#include "Logger/Logger.h"

#include <math.h>

namespace Mengine
{
	//////////////////////////////////////////////////////////////////////////
	SurfaceVideo::SurfaceVideo()
		: m_timing( 0.f )
		, m_needUpdate( false )
		, m_updateFirstFrame(false)
		, m_invalidVideoTexture( true )
        , m_size(0.f, 0.f)
	{
	}
	//////////////////////////////////////////////////////////////////////////
	SurfaceVideo::~SurfaceVideo()
	{
	}
	//////////////////////////////////////////////////////////////////////////
	void SurfaceVideo::setResourceVideo( const ResourceVideoPtr & _resourceVideo )
	{
		if( m_resourceVideo == _resourceVideo )
		{
			return;
		}

		m_resourceVideo = _resourceVideo;

		this->recompile();
	}
	//////////////////////////////////////////////////////////////////////////
	const ResourceVideoPtr & SurfaceVideo::getResourceVideo() const
	{
		return m_resourceVideo;
	}
	//////////////////////////////////////////////////////////////////////////
	float SurfaceVideo::getWidth() const
	{
		const VideoCodecDataInfo * dataInfo = m_videoDecoder->getCodecDataInfo();

		float width = (float)dataInfo->frameWidth;

		return width;
	}
	//////////////////////////////////////////////////////////////////////////
	float SurfaceVideo::getHeight() const
	{
		const VideoCodecDataInfo * dataInfo = m_videoDecoder->getCodecDataInfo();

		float height = (float)dataInfo->frameHeight;

		return height;
	}
	//////////////////////////////////////////////////////////////////////////
	float SurfaceVideo::getDuration() const
	{
		const VideoCodecDataInfo * dataInfo = m_videoDecoder->getCodecDataInfo();

		float duration = (float)dataInfo->duration;

		return duration;
	}
    ////////////////////////////////////////////////////////////////////
    void SurfaceVideo::updateSize_()
    {
        const VideoCodecDataInfo * dataInfo = m_videoDecoder->getCodecDataInfo();

        float width = (float)dataInfo->frameWidth;
        float height = (float)dataInfo->frameHeight;

        m_size.x = width;
        m_size.y = height;
    }
	////////////////////////////////////////////////////////////////////
	void SurfaceVideo::updateUV_()
	{
        const VideoCodecDataInfo * dataInfo = m_videoDecoder->getCodecDataInfo();

		const RenderTextureInterfacePtr & texture = m_textures[0];

		const RenderImageInterfacePtr & image = texture->getImage();

		uint32_t hwWidth = image->getHWWidth();
		uint32_t hwHeight = image->getHWHeight();

        float u = float( dataInfo->frameWidth ) / float( hwWidth );
        float v = float( dataInfo->frameHeight ) / float( hwHeight );

        m_uv.p0 = mt::vec2f( 0.f, 0.f );
        m_uv.p1 = mt::vec2f( u, 0.f );
        m_uv.p2 = mt::vec2f( u, v );
        m_uv.p3 = mt::vec2f( 0.f, v );
	}
	//////////////////////////////////////////////////////////////////////////
    bool SurfaceVideo::_update( float _current, float _timing )
	{
		if( this->isPlay() == false )
		{
			return false;
		}

		if( m_playTime > _current )
		{
			float deltha = m_playTime - _current;
			_timing -= deltha;
		}

		float speedFactor = this->getAnimationSpeedFactor();
		float timing = _timing * speedFactor;

		float scretch = this->getStretch();

		float totalTiming = timing / scretch;

		m_needUpdate = this->sync_( totalTiming );

		this->updateVideoBuffer_();

        return false;
	}
	//////////////////////////////////////////////////////////////////////////
	bool SurfaceVideo::_compile()
	{
		if( m_resourceVideo == nullptr )
		{
			LOGGER_ERROR("Video::_compile '%s' resource is null"
				, this->getName().c_str()
				);

			return false;
		}

		if( m_resourceVideo.compile() == false )
		{
			LOGGER_ERROR("Video::_compile '%s' resource '%s' is not compile"
				, this->getName().c_str()
				, m_resourceVideo->getName().c_str()
				);

			return false;
		}

		if( this->compileDecoder_() == false )
		{
			LOGGER_ERROR("Video::_compile %s can`t create video decoder '%s'"
				, this->getName().c_str()
				, m_resourceVideo->getName().c_str()
				);

			return false;
		}

		uint32_t channels;

		if( m_resourceVideo->isAlpha() == true )
		{
			channels = 4;
		}
		else
		{
			channels = 3;
		}

		const VideoCodecDataInfo * dataInfo = m_videoDecoder->getCodecDataInfo();

		RenderTextureInterfacePtr dynamicTexture = RENDERTEXTURE_SERVICE()
            ->createDynamicTexture( dataInfo->width, dataInfo->height, channels, 1, dataInfo->format );

		if( dynamicTexture == nullptr )
		{
			LOGGER_ERROR("Video::_compile '%s' resource '%s' can`t create dynamic texture"
				, this->getName().c_str()
				, m_resourceVideo->getName().c_str()
				);

			return false;
		}

		m_textures[0] = dynamicTexture;

		this->invalidateMaterial();
        this->updateSize_();
		this->updateUV_();

		m_timing = 0.f;
		m_needUpdate = true;
		m_updateFirstFrame = true;

		m_invalidVideoTexture = true;

		return true;
	}
	//////////////////////////////////////////////////////////////////////////
	bool SurfaceVideo::compileDecoder_()
	{
		m_videoDecoder = m_resourceVideo->createVideoDecoder();

		if( m_videoDecoder == nullptr )
		{
			LOGGER_ERROR("Video::_compileDecoder '%s' can't create video decoder"
				, this->getName().c_str()
				);

			return false;
		}

		return true;
	}
	//////////////////////////////////////////////////////////////////////////
	void SurfaceVideo::_release()
	{
		if( m_videoDecoder != nullptr )
		{
			m_resourceVideo->destroyVideoDecoder( m_videoDecoder );
			m_videoDecoder = nullptr;
		}

		m_resourceVideo.release();

		m_textures[0] = nullptr;

		this->releaseMaterial();
	}
    //////////////////////////////////////////////////////////////////////////
    const mt::vec2f & SurfaceVideo::getMaxSize() const
    {
        return m_size;
    }
    //////////////////////////////////////////////////////////////////////////
    const mt::vec2f & SurfaceVideo::getSize() const
    {
        return m_size;
    }
    //////////////////////////////////////////////////////////////////////////
    const mt::vec2f & SurfaceVideo::getOffset() const
    {
        return mt::vec2f::identity();
    }
    //////////////////////////////////////////////////////////////////////////
    uint32_t SurfaceVideo::getUVCount() const
    {
        return 1;
    }
    //////////////////////////////////////////////////////////////////////////
    const mt::uv4f & SurfaceVideo::getUV( uint32_t _index ) const
    {
        (void)_index;

        return m_uv;
    }
    //////////////////////////////////////////////////////////////////////////
    void SurfaceVideo::correctUV( uint32_t _index, mt::vec2f & _out, const mt::vec2f & _in )
    {
        (void)_index;

        mt::uv4_quad_point( _out, m_uv, _in );
    }
    //////////////////////////////////////////////////////////////////////////
    const ColourValue & SurfaceVideo::getColour() const
    {
        return ColourValue::identity();
    }
	//////////////////////////////////////////////////////////////////////////
	void SurfaceVideo::_stop( uint32_t _enumerator )
	{
		(void)_enumerator;

		m_needUpdate = false;

        EVENTABLE_METHOD( this, EVENT_ANIMATABLE_STOP )
            ->onAnimatableStop( _enumerator );
		//EVENTABLE_CALL( this, EVENT_VIDEO_END )(this, _enumerator, false);
		//EVENTABLE_CALL( this, EVENT_ANIMATABLE_END )(this, _enumerator, false);
	}
	//////////////////////////////////////////////////////////////////////////
	void SurfaceVideo::_end( uint32_t _enumerator )
	{
		(void)_enumerator;

		m_needUpdate = false;

        EVENTABLE_METHOD( this, EVENT_ANIMATABLE_END )
            ->onAnimatableEnd( _enumerator );
		//EVENTABLE_CALL( this, EVENT_VIDEO_END )(this, _enumerator, true);
		//EVENTABLE_CALL( this, EVENT_ANIMATABLE_END )(this, _enumerator, true);
	}
	//////////////////////////////////////////////////////////////////////////
	bool SurfaceVideo::_play( uint32_t _enumerator, float _time )
	{
        (void)_enumerator;
		(void)_time;

		m_timing = 0.f;
		m_needUpdate = true;

		return true;
	}
	//////////////////////////////////////////////////////////////////////////
	bool SurfaceVideo::_restart( uint32_t _enumerator, float _time )
	{
		(void)_time;
		(void)_enumerator;

		return true;
	}
	//////////////////////////////////////////////////////////////////////////
	void SurfaceVideo::_pause( uint32_t _enumerator )
	{
		EVENTABLE_METHOD( this, EVENT_ANIMATABLE_END )
			->onAnimatablePause( _enumerator );
	}
	//////////////////////////////////////////////////////////////////////////
	void SurfaceVideo::_resume( uint32_t _enumerator, float _time )
	{
		EVENTABLE_METHOD( this, EVENT_ANIMATABLE_END )
			->onAnimatableResume( _enumerator, _time );
	}
	//////////////////////////////////////////////////////////////////////////
	void SurfaceVideo::updateVideoBuffer_()
	{
		if( m_needUpdate == true )
		{
			if( this->fillVideoBuffer_() == false )
			{
				LOGGER_ERROR("Video::_render %s invalid fill video buffer (%s)"
					, this->getName().c_str()
					, m_resourceVideo->getName().c_str()
					);

				return;
			}

			m_needUpdate = false;
		}
	}
	////////////////////////////////////////////////////////////////////
	bool SurfaceVideo::sync_( float _timing )
	{
		m_timing += _timing;

		const VideoCodecDataInfo * dataInfo = m_videoDecoder->getCodecDataInfo();

		bool needUpdate = false;

		float frameTiming = dataInfo->getFrameTiming();
		float duration = dataInfo->duration;

		while( m_timing >= frameTiming || m_updateFirstFrame == true )
		{
			m_updateFirstFrame = false;

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

					return false;
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
						LOGGER_ERROR("Video::sync_ %s:%s invalid seek to %f"
							, this->getName().c_str()
							, m_resourceVideo->getName().c_str()
							, time
							);
					}
				}
			}
			else if( state == VDRS_FAILURE )
			{
				LOGGER_ERROR("Video::_sync: '%s' error reading frame timing %4.2f total timing %4.2f"
					, this->getName().c_str()
					, _timing
					, m_timing
					);

				needUpdate = false;

				break;
			}

			m_timing -= frameTiming;
		}

		return needUpdate;
	}
	////////////////////////////////////////////////////////////////////
	void SurfaceVideo::_setTiming( float _timing )
	{
		if( this->isCompile() == false )
		{
			LOGGER_ERROR("Video::_setTiming %s not compile"
				, this->getName().c_str()
				);

			return;
		}

		const VideoCodecDataInfo * dataInfo = m_videoDecoder->getCodecDataInfo();

		float frameTiming = dataInfo->getFrameTiming();

		float frameRate = m_resourceVideo->getFrameRate();

		if( frameRate > 0.f )
		{
			frameTiming = 1000.f / frameRate;
		}

		if( ::fabsf( m_timing - _timing ) < frameTiming )
		{
			return;
		}

		float seek_timing = _timing;

		float duration = dataInfo->duration;

		m_playIterator = this->getPlayCount();

		uint32_t skipIterator = (uint32_t)((m_intervalStart / duration) + 0.5f);

		if( skipIterator > 0 )
		{
			m_playIterator -= skipIterator;
		}

		m_timing = _timing;

		while( m_timing > duration )
		{
			m_timing -= duration;
		}

		if( m_videoDecoder->seek( seek_timing ) == false )
		{
			LOGGER_ERROR("Video::_setTiming %s:%s invalid seek to %f"
				, this->getName().c_str()
				, m_resourceVideo->getName().c_str()
				, seek_timing
				);

			return;
		}

		m_invalidVideoTexture = true;		
		m_needUpdate = false;
	}
	////////////////////////////////////////////////////////////////////
	float SurfaceVideo::_getTiming() const
	{
		float timing = m_videoDecoder->tell();

		return timing;
	}
	////////////////////////////////////////////////////////////////////
	void SurfaceVideo::_setFirstFrame()
	{
		this->setTiming( 0.f );
	}
	////////////////////////////////////////////////////////////////////
	void SurfaceVideo::_setLastFrame()
	{
		const VideoCodecDataInfo * dataInfo = m_videoDecoder->getCodecDataInfo();

		this->_setTiming( dataInfo->duration );
	}
	////////////////////////////////////////////////////////////////////
	bool SurfaceVideo::fillVideoBuffer_()
	{
		const RenderTextureInterfacePtr & texture = m_textures[0];

		const VideoCodecDataInfo * dataInfo = m_videoDecoder->getCodecDataInfo();

        const RenderImageInterfacePtr & image = texture->getImage();

        Rect rect;
		if( dataInfo->clamp == true )
		{
			rect.left = 0;
			rect.top = 0;
			rect.right = texture->getWidth();
			rect.bottom = texture->getHeight();
		}
		else
		{
			rect.left = 0;
			rect.top = 0;
			rect.right = image->getHWWidth();
			rect.bottom = image->getHWHeight();
		}

		size_t pitch = 0;
		void * lockRect = image->lock( &pitch, 0, rect, false );

		if( lockRect == nullptr )
		{
			LOGGER_ERROR("Video::fillVideoBuffer_ %s:%s invalid lock texture %d:%d"
				, this->getName().c_str()
				, m_resourceVideo->getName().c_str()
				, rect.right
				, rect.bottom
				);

			return false;
		}

		m_videoDecoder->setPitch( pitch );

        size_t bufferSize = Helper::getImageMemorySize( image );
		size_t bytes = m_videoDecoder->decode( lockRect, bufferSize );

        image->unlock( 0, true );

		m_invalidVideoTexture = false;

		return bytes != 0;
	}
	////////////////////////////////////////////////////////////////////
	bool SurfaceVideo::_interrupt( uint32_t _enumerator )
	{
		(void)_enumerator;

		return false;
	}
	////////////////////////////////////////////////////////////////////
	RenderMaterialInterfacePtr SurfaceVideo::_updateMaterial() const
	{
		RenderMaterialInterfacePtr material = this->makeTextureMaterial( m_textures, 1, false );

		if( material == nullptr )
		{
			LOGGER_ERROR("Video::_updateMaterial %s invalid make material"
				, this->getName().c_str()
				);

			return nullptr;
		}

		return material;
	}
	////////////////////////////////////////////////////////////////////
}
