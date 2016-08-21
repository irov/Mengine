#	pragma once

#	include "Kernel/Surface.h"

#	include "ResourceVideo.h"

#	include "Interface/VideoCodecInterface.h"
#	include "Interface/COMMengineInterface.h"

namespace Menge
{
	class SurfaceVideo
		: public Surface
		, public Resource		
	{
	public:
		SurfaceVideo();
		~SurfaceVideo();

	public:
		void setResourceVideo( const ResourceVideoPtr & _resourceVideo );
		const ResourceVideoPtr & getResourceVideo() const;

	public:
		float getWidth() const;
		float getHeight() const;
		float getDuration() const;

	public:
		const mt::vec4f & getUV() const;
		
	protected:
		bool _compile() override;
		void _release() override;

	protected:
		void _setEventListener( const pybind::dict & _listener ) override;

	protected:
		void _update( float _current, float _timing ) override;

	protected:
		void _setTiming( float _timing ) override;
		float _getTiming() const override;

		void _setFirstFrame() override;
		void _setLastFrame() override;

	protected:
		bool _play( float _time ) override;
		bool _restart( float _time, uint32_t _enumerator ) override;
		void _pause( uint32_t _enumerator ) override;
		void _resume( float _time, uint32_t _enumerator ) override;
		void _stop( uint32_t _enumerator ) override;
		void _end( uint32_t _enumerator ) override;
		bool _interrupt( uint32_t _enumerator ) override;

	protected:
		RenderMaterialInterfacePtr _updateMaterial() const override;

	protected:
		void updateVideoBuffer_();

		bool sync_( float _timing );
		bool compileDecoder_();
		bool fillVideoBuffer_();
		void updateUV_();

	protected:
		ResourceHolder<ResourceVideo> m_resourceVideo;

		RenderTextureInterfacePtr m_textures[1];

		VideoDecoderInterfacePtr m_videoDecoder;

		float m_timing;

		bool m_needUpdate;
		bool m_invalidVideoTexture;

		mt::vec4f m_uv;
	};
	//////////////////////////////////////////////////////////////////////////
	typedef stdex::intrusive_ptr<SurfaceVideo> SurfaceVideoPtr;
}
