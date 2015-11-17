#	pragma once

#   include "Interface/StreamInterface.h"
#   include "Interface/VideoCodecInterface.h"

#	include "Kernel/Node.h"
#	include "Kernel/Animatable.h"
#	include "Kernel/FixedVertices.h"
#	include "Kernel/Materialable.h"

#   include "ResourceVideo.h"

namespace Menge
{
    class FileServiceInterface;
    class CodecServiceInterface;
    class ResourceServiceInterface;
    class NodeServiceInterface;
    class RenderServiceInterface;

	class RenderTextureInterface;
	
	struct RenderMaterialGroup;

	class Video
		: public Node
		, public QuadVertices
		, public Animatable
		, public Materialable
	{
	public:
		Video();

	public:
		virtual void pause();
		
	public:
		void setResourceVideo( ResourceVideo * _resourceVideo );
		ResourceVideo * getResourceVideo() const;

	public:
		void _setTiming( float _timing ) override;
		float _getTiming() const override;

		void _setFirstFrame() override;
		void _setLastFrame() override;

	protected:
		void _update( float _current, float _timing ) override;
		void _render( const RenderObjectState * _state ) override;
		
    protected:
		bool _activate() override;
		void _deactivate() override;
		bool _compile() override;
		void _release() override;
		void _setEventListener( const pybind::dict & _listener ) override;
		void _invalidateWorldMatrix() override;
		void _updateBoundingBox( mt::box2f & _boundingBox ) const override;
		void _invalidateColor() override;
		
    protected:
		void updateUV_();

		bool sync_( float _timing );
		bool syncFirstFrame_();
		bool compileDecoder_();
        bool fillVideoBuffer_();

	protected:
		bool _play( float _time ) override;
		bool _restart( float _time, uint32_t _enumerator ) override;
		void _pause( uint32_t _enumerator ) override;
		void _resume( uint32_t _enumerator ) override;
		void _stop( uint32_t _enumerator ) override;
		void _end( uint32_t _enumerator ) override;
		bool _interrupt( uint32_t _enumerator ) override;

	protected:
		void _updateVertices( RenderVertex2D * _vertices, unsigned char _invalidateVertices ) const override;

	protected:
		RenderMaterialInterfacePtr _updateMaterial() const override;

	protected:
		ResourceHolder<ResourceVideo> m_resourceVideo;

		RenderTextureInterfacePtr m_textures[1];
		
		VideoDecoderInterfacePtr m_videoDecoder;
		mt::vec2f m_frameSize;
		mt::vec4f m_uv;

		float m_timing;        

		bool m_autoStart;
		bool m_needUpdate;
		bool m_invalidFirstFrame;
        bool m_invalidVideoTexture;
	};
}
